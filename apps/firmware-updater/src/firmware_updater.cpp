#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

namespace {
constexpr const char* kTag = "firmware-updater";
constexpr const char* kStateDir = "/etc/firmware-updater";
constexpr const char* kRunDir = "/tmp/firmware-updater";
constexpr const char* kStageDir = "/tmp/firmware-updater/stage";
constexpr const char* kLockFile = "/var/lock/firmware-updater.lock";
constexpr const char* kLogFile = "/tmp/firmware-updater.log";
constexpr const char* kManifest = "update-manifest.json";
constexpr const char* kManifestSignature = "update-manifest.json.sig";
constexpr const char* kOperatorCertificate = "operator-certificate.pem";

struct PackageSpec {
    std::string file;
    std::string sha256;
};

struct Manifest {
    long long schema_version = 0;
    std::string transaction_id;
    std::string device_id;
    std::string operator_role;
    long long not_before_epoch = 0;
    long long expires_epoch = 0;
    std::vector<PackageSpec> packages;
};

struct VerifiedBundle {
    Manifest manifest;
    std::vector<fs::path> packages;
    std::string operator_subject;
    std::string operator_fingerprint;
};

std::string quote(const std::string& value) {
    std::string out = "'";
    for (char c : value) out += c == '\'' ? "'\\''" : std::string(1, c);
    return out + "'";
}

std::string trim(std::string value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) value.pop_back();
    std::size_t first = 0;
    while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first]))) ++first;
    return value.substr(first);
}

bool run(const std::string& command) { return std::system(command.c_str()) == 0; }

std::string output(const std::string& command) {
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return {};
    std::string value;
    char buffer[512]{};
    while (fgets(buffer, sizeof(buffer), pipe)) value += buffer;
    if (pclose(pipe) != 0) return {};
    return trim(value);
}

std::vector<std::string> output_lines(const std::string& command) {
    std::istringstream input(output(command));
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
        line = trim(line);
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

std::string uci(const std::string& key, const std::string& fallback) {
    const std::string value = output("uci -q get firmware-updater.main." + key + " 2>/dev/null");
    return value.empty() ? fallback : value;
}

std::string clean(std::string value) {
    for (char& c : value) if (c == '\n' || c == '\r' || c == '|') c = ' ';
    return value;
}

void log_message(const std::string& message) {
    const std::string safe = clean(message);
    (void)run("logger -t " + quote(kTag) + " -- " + quote(safe));
    std::ofstream log(kLogFile, std::ios::app);
    if (log) log << safe << '\n';
}

bool audit(const std::string& event, const std::string& detail) {
    const fs::path path = uci("audit_log", "/mnt/mmcblk0/firmware-updater/security-audit.log");
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);
    if (ec) return false;
    (void)chmod(path.parent_path().c_str(), 0700);
    const std::string line = std::to_string(static_cast<long long>(std::time(nullptr))) + "|" +
                             clean(event) + "|" + clean(detail) + "\n";
    const int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd < 0) return false;
    const bool ok = write(fd, line.data(), line.size()) == static_cast<ssize_t>(line.size()) &&
                    fsync(fd) == 0;
    close(fd);
    return ok;
}

bool security_event(const std::string& event, const std::string& detail) {
    log_message(event + ": " + detail);
    if (audit(event, detail)) return true;
    log_message("Persistent security audit write failed");
    return uci("require_persistent_audit", "1") != "1";
}

bool tpm_available() {
    return access("/dev/tpmrm0", F_OK) == 0 &&
           run("command -v tpm2_getcap >/dev/null 2>&1") &&
           run("tpm2_getcap properties-fixed >/dev/null 2>&1");
}

std::string tpm_index() { return uci("tpm_nv_index", "0x1500016"); }

bool tpm_handle_ok(const std::string& handle) {
    return (handle.size() == 9 || handle.size() == 10) &&
           (handle.rfind("0x1", 0) == 0 || handle.rfind("0x01", 0) == 0) &&
           std::all_of(handle.begin() + 2, handle.end(), [](unsigned char c) {
               return std::isxdigit(c) != 0;
           });
}

bool read_tpm_public(const std::string& setting, const std::string& fallback,
                     const fs::path& destination) {
    const std::string handle = uci(setting, fallback);
    if (!tpm_handle_ok(handle)) return false;
    std::error_code ec;
    fs::remove(destination, ec);
    if (!run("tpm2_nvread -C o -o " + quote(destination.string()) + " " +
             quote(handle) + " >/dev/null 2>&1")) return false;
    const auto size = fs::file_size(destination, ec);
    if (ec || size == 0 || size > 2160 || fs::is_symlink(destination, ec)) {
        fs::remove(destination, ec);
        return false;
    }
    (void)chmod(destination.c_str(), 0600);
    return true;
}

bool tpm_counter_ready() {
    return run("tpm2_nvreadpublic " + quote(tpm_index()) + " >/dev/null 2>&1");
}

bool provision_tpm() {
    if (!tpm_available()) return false;
    if (tpm_counter_ready()) return true;
    if (!run("tpm2_nvdefine " + quote(tpm_index()) +
             " -C o -s 8 -a 'ownerread|ownerwrite|authread|nt=counter'")) return false;
    return run("tpm2_nvincrement " + quote(tpm_index()) + " -C o");
}

bool increment_tpm_counter() {
    return (tpm_counter_ready() || provision_tpm()) &&
           run("tpm2_nvincrement " + quote(tpm_index()) + " -C o");
}

std::string decode_mount(std::string value) {
    std::string decoded;
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 3 < value.size() &&
            value[i + 1] >= '0' && value[i + 1] <= '7' &&
            value[i + 2] >= '0' && value[i + 2] <= '7' &&
            value[i + 3] >= '0' && value[i + 3] <= '7') {
            decoded.push_back(static_cast<char>((value[i + 1] - '0') * 64 +
                                                (value[i + 2] - '0') * 8 +
                                                value[i + 3] - '0'));
            i += 3;
        } else decoded.push_back(value[i]);
    }
    return decoded;
}

bool removable(const std::string& device) {
    if (device.rfind("/dev/", 0) != 0) return false;
    std::error_code ec;
    const fs::path resolved = fs::canonical(
        fs::path("/sys/class/block") / fs::path(device).filename(), ec);
    if (ec) return false;
    auto flag = [](const fs::path& path) {
        std::ifstream file(path / "removable");
        int value = 0;
        return file && (file >> value) && value == 1;
    };
    return flag(resolved) || flag(resolved.parent_path());
}

std::vector<fs::path> usb_manifests(unsigned max_depth) {
    std::ifstream mounts("/proc/mounts");
    std::vector<fs::path> found;
    std::string device, mount_point, type;
    while (mounts >> device >> mount_point >> type) {
        std::string rest;
        std::getline(mounts, rest);
        device = decode_mount(device);
        mount_point = decode_mount(mount_point);
        if (!removable(device) || !fs::is_directory(mount_point)) continue;
        std::error_code ec;
        fs::recursive_directory_iterator it(
            mount_point, fs::directory_options::skip_permission_denied, ec), end;
        for (; it != end && !ec; it.increment(ec)) {
            if (it.depth() >= static_cast<int>(max_depth)) it.disable_recursion_pending();
            if (it->is_regular_file(ec) && it->path().filename() == kManifest) found.push_back(it->path());
        }
    }
    std::sort(found.begin(), found.end());
    found.erase(std::unique(found.begin(), found.end()), found.end());
    return found;
}

bool safe_identifier(const std::string& value, std::size_t minimum, std::size_t maximum) {
    if (value.size() < minimum || value.size() > maximum) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == ':';
    });
}

bool safe_apk_name(const std::string& value) {
    return safe_identifier(value, 5, 180) && fs::path(value).filename() == value &&
           fs::path(value).extension() == ".apk";
}

bool is_sha256(const std::string& value) {
    return value.size() == 64 && std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return std::isxdigit(c) != 0;
    });
}

bool is_decimal(const std::string& value) {
    return !value.empty() && std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return std::isdigit(c) != 0;
    });
}

std::string sha256(const fs::path& path) {
    std::string value = output("sha256sum " + quote(path.string()) + " 2>/dev/null");
    const std::size_t separator = value.find_first_of(" \t");
    if (separator == std::string::npos) return {};
    value.resize(separator);
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return is_sha256(value) ? value : std::string{};
}

std::string json(const fs::path& file, const std::string& expression) {
    return output("jsonfilter -q -i " + quote(file.string()) + " -e " + quote(expression) +
                  " 2>/dev/null");
}

std::vector<std::string> json_list(const fs::path& file, const std::string& expression) {
    return output_lines("jsonfilter -q -i " + quote(file.string()) + " -e " + quote(expression) +
                        " 2>/dev/null");
}

bool parse_manifest(const fs::path& path, Manifest& manifest, std::string& error) {
    const std::string schema = json(path, "@.schema_version");
    const std::string start = json(path, "@.not_before_epoch");
    const std::string expiry = json(path, "@.expires_epoch");
    if (!is_decimal(schema) || !is_decimal(start) || !is_decimal(expiry)) {
        error = "invalid numeric manifest fields";
        return false;
    }
    try {
        manifest.schema_version = std::stoll(schema);
        manifest.not_before_epoch = std::stoll(start);
        manifest.expires_epoch = std::stoll(expiry);
    } catch (...) {
        error = "manifest number outside accepted range";
        return false;
    }
    manifest.transaction_id = json(path, "@.transaction_id");
    manifest.device_id = json(path, "@.device_id");
    manifest.operator_role = json(path, "@.operator_role");
    if (manifest.schema_version != 1 || !safe_identifier(manifest.transaction_id, 16, 64) ||
        !safe_identifier(manifest.device_id, 3, 96) ||
        !safe_identifier(manifest.operator_role, 3, 64)) {
        error = "invalid manifest identity fields";
        return false;
    }
    const auto files = json_list(path, "@.packages[*].file");
    auto hashes = json_list(path, "@.packages[*].sha256");
    if (files.empty() || files.size() != hashes.size() || files.size() > 32) {
        error = "empty or inconsistent manifest package list";
        return false;
    }
    std::set<std::string> unique;
    for (std::size_t i = 0; i < files.size(); ++i) {
        std::transform(hashes[i].begin(), hashes[i].end(), hashes[i].begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        if (!safe_apk_name(files[i]) || !is_sha256(hashes[i]) || !unique.insert(files[i]).second) {
            error = "invalid or duplicate APK manifest entry";
            return false;
        }
        manifest.packages.push_back({files[i], hashes[i]});
    }
    return true;
}

bool copy_regular(const fs::path& source, const fs::path& destination) {
    std::error_code ec;
    if (!fs::is_regular_file(source, ec) || fs::is_symlink(source, ec)) return false;
    fs::copy_file(source, destination, fs::copy_options::overwrite_existing, ec);
    return !ec;
}

bool verify_certificate(const fs::path& certificate, VerifiedBundle& bundle, std::string& error) {
    const fs::path ca = fs::path(kStageDir) / "operator-ca.pem";
    const fs::path crl = fs::path(kStageDir) / "operator-crl.pem";
    if (!read_tpm_public("tpm_operator_ca_index", "0x1500017", ca) ||
        !read_tpm_public("tpm_operator_crl_index", "0x1500018", crl)) {
        error = "operator CA or CRL is not provisioned in TPM NV";
        return false;
    }
    std::string command = "openssl verify -purpose any -CAfile " + quote(ca.string());
    command += " -crl_check -CRLfile " + quote(crl.string());
    command += " " + quote(certificate.string()) + " >/dev/null 2>&1";
    if (!run(command)) {
        error = "operator certificate chain, time, or revocation validation failed";
        return false;
    }
    const std::string constraints = output("openssl x509 -in " + quote(certificate.string()) +
                                           " -noout -ext basicConstraints 2>/dev/null");
    const std::string usage = output("openssl x509 -in " + quote(certificate.string()) +
                                     " -noout -ext keyUsage 2>/dev/null");
    if (usage.find("Digital Signature") == std::string::npos ||
        constraints.find("CA:FALSE") == std::string::npos) {
        error = "operator certificate lacks digitalSignature or is a CA certificate";
        return false;
    }
    bundle.operator_subject = output("openssl x509 -in " + quote(certificate.string()) +
                                     " -noout -subject -nameopt RFC2253 2>/dev/null");
    bundle.operator_fingerprint = output("openssl x509 -in " + quote(certificate.string()) +
                                         " -noout -fingerprint -sha256 2>/dev/null");
    const std::string required_role = uci("operator_role_subject", "OU=Firmware Update Operators");
    const auto subject_attributes = output_lines(
        "openssl x509 -in " + quote(certificate.string()) +
        " -noout -subject -nameopt sep_multiline,sname 2>/dev/null");
    if (required_role.empty() ||
        std::find(subject_attributes.begin(), subject_attributes.end(), required_role) ==
            subject_attributes.end()) {
        error = "operator certificate lacks the firmware-update authority role";
        return false;
    }
    return true;
}

bool verify_manifest_signature(const fs::path& certificate, const fs::path& manifest,
                               const fs::path& signature, std::string& error) {
    const fs::path public_key = fs::path(kStageDir) / "operator-public-key.pem";
    if (!run("openssl x509 -in " + quote(certificate.string()) + " -pubkey -noout >" +
             quote(public_key.string()) + " 2>/dev/null") ||
        !run("openssl dgst -sha256 -verify " + quote(public_key.string()) + " -signature " +
             quote(signature.string()) + " " + quote(manifest.string()) + " >/dev/null 2>&1")) {
        error = "manifest signature verification failed";
        return false;
    }
    return true;
}

bool validate_policy(const Manifest& manifest, std::string& error) {
    if (uci("device_id", "").empty()) {
        error = "device_id is not provisioned";
        return false;
    }
    if (manifest.device_id != uci("device_id", "") ||
        manifest.operator_role != uci("operator_role", "firmware-update")) {
        error = "authorization targets a different device or role";
        return false;
    }
    long long maximum_lifetime = 0;
    try { maximum_lifetime = std::stoll(uci("max_authorization_seconds", "86400")); }
    catch (...) { error = "invalid maximum authorization lifetime configuration"; return false; }
    const long long now = static_cast<long long>(std::time(nullptr));
    if (manifest.not_before_epoch > now || manifest.expires_epoch < now ||
        manifest.expires_epoch <= manifest.not_before_epoch ||
        manifest.expires_epoch - manifest.not_before_epoch > maximum_lifetime) {
        error = "authorization is expired, premature, or valid for too long";
        return false;
    }
    return true;
}

fs::path transaction_path(const std::string& id) {
    return fs::path(kStateDir) / "transactions" / id;
}

bool consume_transaction(const VerifiedBundle& bundle) {
    const fs::path directory = fs::path(kStateDir) / "transactions";
    std::error_code ec;
    fs::create_directories(directory, ec);
    if (ec) return false;
    (void)chmod(directory.c_str(), 0700);
    const int fd = open(transaction_path(bundle.manifest.transaction_id).c_str(),
                        O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0) return false;
    const std::string record = "device_id=" + bundle.manifest.device_id + "\noperator=" +
        clean(bundle.operator_fingerprint) + "\nconsumed_epoch=" +
        std::to_string(static_cast<long long>(std::time(nullptr))) + "\n";
    const bool ok = write(fd, record.data(), record.size()) == static_cast<ssize_t>(record.size()) &&
                    fsync(fd) == 0;
    close(fd);
    return ok;
}

bool verify_apk_signatures(const std::vector<fs::path>& packages, std::string& error) {
    const fs::path keys = fs::path(kStageDir) / "manufacturer-keys";
    std::error_code ec;
    fs::create_directories(keys, ec);
    if (ec) { error = "cannot stage manufacturer key"; return false; }
    (void)chmod(keys.c_str(), 0700);
    if (!read_tpm_public("tpm_manufacturer_key_index", "0x1500019",
                         keys / "ccli-release.pem")) {
        error = "manufacturer APK key is not provisioned in TPM NV";
        return false;
    }
    for (const auto& package : packages) {
        if (!run("apk --keys-dir " + quote(keys.string()) + " --no-network verify " +
                 quote(package.string()) + " >/dev/null 2>&1")) {
            error = "manufacturer APK signature failed for " + package.filename().string();
            return false;
        }
    }
    return true;
}

bool stage_and_verify(const fs::path& source_manifest, VerifiedBundle& bundle, std::string& error) {
    const fs::path source = source_manifest.parent_path();
    std::error_code ec;
    fs::remove_all(kStageDir, ec);
    ec.clear();
    fs::create_directories(kStageDir, ec);
    if (ec) { error = "cannot create staging directory"; return false; }
    (void)chmod(kStageDir, 0700);
    const fs::path manifest = fs::path(kStageDir) / kManifest;
    const fs::path signature = fs::path(kStageDir) / kManifestSignature;
    const fs::path certificate = fs::path(kStageDir) / kOperatorCertificate;
    if (!copy_regular(source_manifest, manifest) ||
        !copy_regular(source / kManifestSignature, signature) ||
        !copy_regular(source / kOperatorCertificate, certificate)) {
        error = "bundle lacks manifest, signature, or operator certificate";
        return false;
    }
    if (!verify_certificate(certificate, bundle, error) ||
        !verify_manifest_signature(certificate, manifest, signature, error) ||
        !parse_manifest(manifest, bundle.manifest, error) ||
        !validate_policy(bundle.manifest, error)) return false;
    if (fs::exists(transaction_path(bundle.manifest.transaction_id))) {
        error = "one-time transaction was already consumed";
        return false;
    }
    std::set<std::string> listed;
    for (const auto& item : bundle.manifest.packages) {
        listed.insert(item.file);
        const fs::path staged = fs::path(kStageDir) / item.file;
        if (!copy_regular(source / item.file, staged) || sha256(staged) != item.sha256) {
            error = "APK missing or hash mismatch: " + item.file;
            return false;
        }
        bundle.packages.push_back(staged);
    }
    for (const auto& entry : fs::directory_iterator(source, ec)) {
        if (entry.is_regular_file(ec) && entry.path().extension() == ".apk" &&
            listed.count(entry.path().filename().string()) == 0) {
            error = "bundle contains an unsigned-manifest APK";
            return false;
        }
    }
    return verify_apk_signatures(bundle.packages, error);
}

std::string apk_add() {
    return "apk --keys-dir " + quote(std::string(kStageDir) + "/manufacturer-keys") +
           " --no-network --force-missing-repositories add";
}

bool transaction_needed(const std::vector<fs::path>& packages) {
    std::string command = apk_add() + " --simulate --upgrade";
    for (const auto& package : packages) command += " " + quote(package.string());
    command += " >" + quote(std::string(kRunDir) + "/apk-probe.log") + " 2>&1";
    if (!run(command)) return false;
    std::ifstream probe(std::string(kRunDir) + "/apk-probe.log");
    std::string line;
    while (std::getline(probe, line)) {
        if (line.find(" Installing ") != std::string::npos ||
            line.find(" Upgrading ") != std::string::npos) return true;
    }
    return false;
}

bool service_name_ok(const std::string& value) {
    return safe_identifier(value, 1, 48) && value.find('.') == std::string::npos &&
           value.find(':') == std::string::npos;
}

bool service_running(const std::string& service) {
    // rc.common status can succeed with "active with no instances".
    return output("ubus call service list " + quote("{\"name\":\"" + service + "\"}") +
                  " | jsonfilter -e '@.*.instances.*.running' 2>/dev/null").find("true") != std::string::npos;
}

bool ccli_ready(const std::string& service) {
    if (!service_running(service)) return false;
    if (service != "ccli") return true;
    return !fs::exists("/etc/ccli-upgrade.pending") && !fs::exists("/etc/ccli-upgrade.hold") &&
        run("pidof ccli >/dev/null && netstat -lnt 2>/dev/null | grep -Eq ':3782[[:space:]].*LISTEN'");
}

// Record every existing credential/configuration file before apk runs. New
// package defaults may be added, but existing files must survive byte-for-byte.
bool ccli_snapshot(std::vector<std::pair<fs::path, std::string>>& files) {
    try {
        if (fs::exists("/etc/config/ccli"))
            files.emplace_back("/etc/config/ccli", sha256("/etc/config/ccli"));
        if (fs::exists("/etc/ccli"))
            for (const auto& entry : fs::recursive_directory_iterator("/etc/ccli"))
                if (entry.is_regular_file()) files.emplace_back(entry.path(), sha256(entry.path()));
        for (const auto& item : files) if (item.second.empty()) return false;
        return true;
    } catch (const fs::filesystem_error&) { return false; }
}

bool complete_ccli_upgrade(const std::vector<std::pair<fs::path, std::string>>& files) {
    if (fs::exists("/etc/ccli-upgrade.pending")) return false;
    for (const auto& item : files)
        if (sha256(item.first) != item.second) return false;
    if (!run("test -x /usr/sbin/ccli && /usr/sbin/ccli --version >/dev/null 2>&1")) return false;
    std::error_code ec;
    fs::remove("/etc/ccli-upgrade.hold", ec);
    return !ec;
}

constexpr int kCcliStopPolls = 5;

bool deactivate_ccli(const std::string& service, bool& was_running, bool& service_present) {
    if (!service_name_ok(service)) return false;
    service_present = fs::is_regular_file(fs::path("/etc/init.d") / service);
    // A first CCLI installation, a recovery after CCLI removal, or a bundle
    // for another APK has no running CCLI process to quiesce.
    if (!service_present) {
        was_running = false;
        return true;
    }
    was_running = service_running(service);
    if (!was_running) return true;
    // Some procd init scripts can return a non-zero status after successfully
    // signalling their service. The safety condition is that CCLI is actually
    // inactive before the APK transaction, not the exit status alone.
    (void)run("/etc/init.d/" + service + " stop >/dev/null 2>&1");
    for (int poll = 0; poll < kCcliStopPolls; ++poll) {
        sleep(1);
        if (!service_running(service)) return true;
    }
    return false;
}

bool ensure_ccli_running(const std::string& service) {
    if (!service_name_ok(service)) return false;
    if (!fs::is_regular_file(fs::path("/etc/init.d") / service)) return true;
    if (ccli_ready(service)) return true;
    (void)run("/etc/init.d/" + service + " start >/dev/null 2>&1");
    int stable = 0;
    for (int poll = 0; poll < 30; ++poll) {
        sleep(1);
        stable = ccli_ready(service) ? stable + 1 : 0;
        if (stable >= 3) return true;
    }
    return false;
}

bool bundle_contains_ccli(const std::vector<fs::path>& packages) {
    for (const auto& package : packages) {
        const std::string name = package.filename().string();
        if (name.rfind("ccli-", 0) == 0 && package.extension() == ".apk") return true;
    }
    return false;
}

bool install(const VerifiedBundle& bundle) {
    const std::string id = bundle.manifest.transaction_id;
    if (!transaction_needed(bundle.packages)) {
        security_event("UPDATE_NOT_APPLICABLE", "transaction=" + id);
        return false;
    }
    const std::string service = uci("ccli_service", "ccli");
    std::vector<std::pair<fs::path, std::string>> preserved;
    const bool updates_ccli = bundle_contains_ccli(bundle.packages);
    if (updates_ccli && (fs::exists("/etc/ccli-upgrade.pending") ||
                        fs::exists("/etc/ccli-upgrade.hold") || !ccli_snapshot(preserved))) {
        security_event("UPDATE_ABORTED", "transaction=" + id + " unfinished CCLI upgrade or unreadable configuration");
        return false;
    }
    bool was_running = false;
    bool service_present = false;
    if (uci("require_ccli_deactivation", "1") == "1" &&
        !deactivate_ccli(service, was_running, service_present)) {
        const bool restored = ensure_ccli_running(service);
        security_event(restored ? "CCLI_RESTORED" : "CCLI_RESTORE_FAILED",
                       "transaction=" + id);
        security_event("UPDATE_ABORTED", "transaction=" + id + " CCLI deactivation failed");
        return false;
    }
    if (!security_event(service_present ? "CCLI_DEACTIVATED" : "CCLI_NOT_INSTALLED",
                        "transaction=" + id + " service=" + service)) {
        (void)ensure_ccli_running(service);
        return false;
    }
    // Authorize only after every pre-install safety condition, including CCLI
    // quiescence, has been met. The transaction itself is recorded after the
    // APK manager reports success, so a rejected simulation or failed install
    // can be corrected and retried with the same authorization.
    if (!security_event("AUTHORIZATION_ACCEPTED", "transaction=" + id + " " +
                        bundle.operator_subject + " " + bundle.operator_fingerprint)) {
        (void)ensure_ccli_running(service);
        security_event("CCLI_RESTORED", "transaction=" + id);
        return false;
    }

    std::string command = apk_add() + " --simulate --upgrade";
    for (const auto& package : bundle.packages) command += " " + quote(package.string());
    command += " >>" + quote(kLogFile) + " 2>&1";
    bool success = run(command);
    bool apk_installed = false;
    if (success) {
        command = apk_add() + " --upgrade";
        for (const auto& package : bundle.packages) command += " " + quote(package.string());
        command += " >>" + quote(kLogFile) + " 2>&1";
        security_event("INSTALL_STARTED", "transaction=" + id +
                       " packages=" + std::to_string(bundle.packages.size()));
        apk_installed = run(command);
        success = apk_installed;
    }
    // scan_once serializes installs; O_EXCL makes this durable record atomic.
    // It is deliberately written only after the package manager confirms an
    // actual installation, never for a pre-install failure.
    if (apk_installed && !consume_transaction(bundle)) {
        security_event("UPDATE_FAILED", "transaction=" + id +
                       " installed APK but could not record transaction");
        success = false;
    }
    if (success) success = increment_tpm_counter();
    const bool ccli_present_after_install =
        fs::is_regular_file(fs::path("/etc/init.d") / service);
    const bool preservation_ok = !updates_ccli ||
        (apk_installed && complete_ccli_upgrade(preserved));
    if (!preservation_ok)
        security_event("CCLI_VERIFICATION_FAILED", "transaction=" + id + " startup hold retained");
    bool restored = preservation_ok && ensure_ccli_running(service);
    if (!service_present && ccli_present_after_install && bundle_contains_ccli(bundle.packages)) {
        security_event(restored ? "CCLI_STARTED" : "CCLI_START_FAILED", "transaction=" + id);
    } else if (ccli_present_after_install) {
        security_event(restored ? "CCLI_RESTORED" : "CCLI_RESTORE_FAILED", "transaction=" + id);
    }
    success = success && restored;
    security_event(success ? "UPDATE_SUCCEEDED" : "UPDATE_FAILED", "transaction=" + id);
    return success;
}

int process(const fs::path& manifest, bool do_install) {
    struct StageCleanup {
        ~StageCleanup() {
            std::error_code ec;
            fs::remove_all(kStageDir, ec);
        }
    } cleanup;
    VerifiedBundle bundle;
    std::string error;
    if (!stage_and_verify(manifest, bundle, error)) {
        security_event("AUTHORIZATION_REJECTED", manifest.string() + " " + error);
        return 1;
    }
    if (!security_event("BUNDLE_VERIFIED", "transaction=" + bundle.manifest.transaction_id +
                        " device=" + bundle.manifest.device_id)) return 1;
    return do_install && !install(bundle) ? 1 : 0;
}

int scan_once() {
    fs::create_directories(kStateDir);
    fs::create_directories(kRunDir);
    fs::create_directories("/var/lock");
    const int lock = open(kLockFile, O_CREAT | O_RDWR, 0600);
    if (lock < 0 || flock(lock, LOCK_EX | LOCK_NB) != 0) {
        if (lock >= 0) close(lock);
        return 0;
    }
    if (uci("require_tpm", "1") == "1" && !tpm_available()) {
        security_event("UPDATE_DEFERRED", "TPM unavailable");
        close(lock);
        return 1;
    }
    const auto manifests = usb_manifests(static_cast<unsigned>(std::stoul(uci("max_depth", "8"))));
    int result = 0;
    for (const auto& manifest : manifests) result |= process(manifest, true);
    close(lock);
    return result;
}

void status() {
    std::cout << "Updater log: " << kLogFile << '\n'
              << "Persistent audit: " << uci("audit_log",
                 "/mnt/mmcblk0/firmware-updater/security-audit.log") << '\n'
              << "Device ID: " << uci("device_id", "NOT PROVISIONED") << '\n'
              << "Operator CA TPM NV: " << uci("tpm_operator_ca_index", "0x1500017") << '\n'
              << "Operator CRL TPM NV: " << uci("tpm_operator_crl_index", "0x1500018") << '\n'
              << "Manufacturer key TPM NV: " << uci("tpm_manufacturer_key_index", "0x1500019") << '\n'
              << "TPM: " << (tpm_available() ? "available" : "unavailable") << '\n'
              << "TPM NV counter: " << (tpm_counter_ready() ? tpm_index() : "not provisioned") << '\n';
}
} // namespace

int main(int argc, char** argv) {
    const std::string command = argc > 1 ? argv[1] : "--status";
    if (command == "--once" || command == "once") return scan_once();
    if (command == "--status" || command == "status") { status(); return 0; }
    if (command == "--provision-tpm" || command == "provision-tpm") return provision_tpm() ? 0 : 1;
    if ((command == "--verify-bundle" || command == "verify-bundle") && argc == 3)
        return process(fs::path(argv[2]) / kManifest, false);
    if (command == "--daemon" || command == "daemon") {
        for (;;) {
            scan_once();
            sleep(static_cast<unsigned>(std::stoul(uci("interval", "15"))));
        }
    }
    std::cerr << "Usage: firmware-updater [--once|--daemon|--status|--provision-tpm|"
                 "--verify-bundle DIRECTORY]\n";
    return 2;
}

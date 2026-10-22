#include "event/event_store.hpp"
#include "event/o14_catalog.hpp"

#include <cstdio>
#include <fstream>
#include <string>

namespace {

bool write_file(const char* path, const char* body) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return false;
    }
    out << body;
    return out.good();
}

}  // namespace

int main() {
    const std::string path = "ccli-event-store-unit.jsonl";
    const std::string marker = cci::core::firmware_marker_path(path);
    std::remove(path.c_str());
    std::remove(marker.c_str());

    if (cci::core::o14::category_for("mms", "wlim_on") !=
        std::string(cci::core::o14::kDsoCommand)) {
        std::fprintf(stderr, "category wlim failed\n");
        return 1;
    }
    if (cci::core::o14::category_for("mms", "auth_reject_mechanism") !=
        std::string(cci::core::o14::kAuth)) {
        std::fprintf(stderr, "category auth failed\n");
        return 1;
    }

    if (!cci::core::EventStore::run_wrap_test(path)) {
        std::fprintf(stderr, "wrap test failed\n");
        std::remove(path.c_str());
        return 1;
    }
    std::remove(path.c_str());

    cci::core::EventStore store({true, path, false, "127.0.0.1", 514});
    store.append({1, "mms", "client_connect", ""});
    if (store.size() != 1) {
        std::fprintf(stderr, "size after append failed\n");
        return 1;
    }
    const auto rows = store.recent(1);
    if (rows.empty() || rows[0].category != cci::core::o14::kCommsDso) {
        std::fprintf(stderr, "category fill failed got=%s\n",
                     rows.empty() ? "(empty)" : rows[0].category.c_str());
        return 1;
    }

    const auto ts = cci::core::format_o14_timestamp(0);
    if (ts.size() != 19 || ts[4] != '/' || ts[7] != '/' || ts[10] != ' ') {
        std::fprintf(stderr, "timestamp format failed: %s\n", ts.c_str());
        return 1;
    }

    cci::core::EventRecord ev{1000, "mms", "auth_reject_empty_cert",
                              cci::core::o14::kAuth};
    const auto rfc = cci::core::format_rfc5424(ev, "dut", "ccli");
    if (rfc.find("<132>") != 0 || rfc.find("auth_reject_empty_cert") == std::string::npos) {
        std::fprintf(stderr, "rfc5424 failed: %s\n", rfc.c_str());
        return 1;
    }

    auto first = cci::core::EventStore::record_firmware_boot(path, "0.1.0-r35");
    if (first.kind != cci::core::FirmwareBootKind::First) {
        std::fprintf(stderr, "firmware first boot failed\n");
        return 1;
    }
    auto same = cci::core::EventStore::record_firmware_boot(path, "0.1.0-r35");
    if (same.kind != cci::core::FirmwareBootKind::Unchanged) {
        std::fprintf(stderr, "firmware unchanged failed\n");
        return 1;
    }
    auto upd = cci::core::EventStore::record_firmware_boot(path, "0.1.0-r36");
    if (upd.kind != cci::core::FirmwareBootKind::Updated ||
        upd.detail.find("0.1.0-r35->0.1.0-r36") == std::string::npos) {
        std::fprintf(stderr, "firmware update failed: %s\n", upd.detail.c_str());
        return 1;
    }

    if (!cci::core::EventStore::user_overwrite_forbidden()) {
        std::fprintf(stderr, "overwrite flag failed\n");
        return 1;
    }

    (void)write_file;
    std::remove(path.c_str());
    std::remove(marker.c_str());
    return 0;
}

#!/usr/bin/env python3
"""Clone and configure Annex CCLI certificate/end-entity profiles in EJBCA Admin (lab).

Uses Admin Web HTTP (TLS_SETUP_ENABLED=simple — no client cert) + ejbca.sh CLI edits.
Run once after fresh EJBCA volume; idempotent if profiles already exist.
"""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
from html.parser import HTMLParser
from pathlib import Path

import requests
import urllib3

REPO = Path(__file__).resolve().parents[1]
BASE = "https://localhost:8443/ejbca/adminweb"
CP_URL = f"{BASE}/ca/editcertificateprofiles/editcertificateprofiles.xhtml"
CP_CLONE_URL = f"{BASE}/ca/editcertificateprofiles/clonecertificateprofile.xhtml"
EE_URL = f"{BASE}/ra/editendentityprofiles/editendentityprofiles.xhtml"
CONTAINER = "ccli-ejbca-ce"
EJBCA_SH = "/opt/keyfactor/bin/ejbca.sh"

CERT_A = "CCLI-Cert-A-TLS"
CERT_B = "CCLI-Cert-B-E2E-MMS"
EE_PROFILE = "CCLI-Lab-EE"
EE_EDIT = f"{BASE}/ra/editendentityprofiles/endentityprofilepage.xhtml"


def _run_cli(args: list[str]) -> subprocess.CompletedProcess[str]:
    cmd = ["docker", "exec", CONTAINER, EJBCA_SH] + args
    return subprocess.run(cmd, capture_output=True, text=True)


def _edit_cp(name: str, field: str, value: str) -> None:
    cp = _run_cli(["ca", "editcertificateprofile", name, "--field", field, "--value", value])
    if cp.returncode != 0:
        raise RuntimeError(f"editcertificateprofile {name} {field}: {cp.stderr or cp.stdout}")


def _viewstate(html: str) -> str:
    m = re.search(r'name="jakarta\.faces\.ViewState"[^>]*value="([^"]+)"', html)
    if not m:
        raise RuntimeError("ViewState not found in Admin page")
    return m.group(1)


def _profile_exists(name: str) -> bool:
    chk = _run_cli(["ra", "addendentity", "--help"])
    return name in (chk.stdout or "")


def _ee_profile_exists(name: str) -> bool:
    chk = _run_cli(["ra", "addendentity", "--help"])
    out = chk.stdout or ""
    return f"Existing endentity profiles:" in out and name in out.split("Existing endentity profiles:")[-1]


class _EeFormParser(HTMLParser):
    """Parse eeProfiles JSF form; capture input values and selected option values only."""

    def __init__(self) -> None:
        super().__init__()
        self.fields: dict[str, str | list[str]] = {}
        self._in_form = False
        self._select: str | None = None

    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        ad = {k: v for k, v in attrs if v is not None}
        if tag == "form" and ad.get("id") == "eeProfiles":
            self._in_form = True
        if not self._in_form:
            return
        if tag == "input":
            name = ad.get("name")
            if not name or ad.get("type") in ("submit", "button", "image"):
                return
            typ = ad.get("type", "text")
            if typ == "checkbox":
                if "checked" in ad:
                    self._set(name, ad.get("value", "on"))
                return
            if typ == "radio" and "checked" not in ad:
                return
            self._set(name, ad.get("value", ""))
        elif tag == "select":
            self._select = ad.get("name")
        elif tag == "option" and self._select and "selected" in ad:
            self._set(self._select, ad.get("value", ""))

    def handle_endtag(self, tag: str) -> None:
        if tag == "select":
            self._select = None
        elif tag == "form":
            self._in_form = False

    def _set(self, name: str, val: str) -> None:
        if name.endswith(("availableCertProfiles", "availableCA", "availableToken")):
            cur = self.fields.get(name)
            if isinstance(cur, list):
                cur.append(val)
            elif cur is None:
                self.fields[name] = [val]
            else:
                self.fields[name] = [cur, val]
        else:
            self.fields[name] = val


def _option_id(html: str, select_name: str, label: str) -> str | None:
    block = re.search(rf'id="{re.escape(select_name)}"[^>]*>(.*?)</select>', html, re.S)
    if not block:
        return None
    m = re.search(rf'<option value="([^"]+)">[^<]*{re.escape(label)}', block.group(1))
    return m.group(1) if m else None


def _cert_profile_option_ids(html: str, *names: str) -> list[str]:
    return [i for n in names if (i := _option_id(html, "eeProfiles:defaultCertificateProfile", n))]


def _open_ee_add_form(session: requests.Session) -> tuple[dict[str, str | list[str]], str, str]:
    r = session.get(EE_URL, timeout=60)
    vs = _viewstate(r.text)
    r2 = session.post(
        EE_URL,
        data={
            "manageEndEntityProfiles": "manageEndEntityProfiles",
            "manageEndEntityProfiles:addButton": "Add",
            "jakarta.faces.ViewState": vs,
        },
        timeout=60,
    )
    p = _EeFormParser()
    p.feed(r2.text)
    return p.fields, _viewstate(r2.text), r2.text


def _create_ee_profile(session: requests.Session) -> None:
    if _ee_profile_exists(EE_PROFILE):
        print(f"  end entity profile {EE_PROFILE} exists — skip")
        return
    fields, vs, html = _open_ee_add_form(session)
    cert_ids = _cert_profile_option_ids(html, CERT_A, CERT_B)
    if len(cert_ids) < 2:
        raise RuntimeError(f"Cert profile IDs not found for {CERT_A}/{CERT_B}")

    fields["eeProfiles:endEntityProfileName"] = EE_PROFILE
    fields["eeProfiles:autoGenerateUserNameCheckBox_input"] = "on"
    fields["eeProfiles:mergeDnCheckBox_input"] = "on"
    fields["eeProfiles:subjectDnComponents:0:dnModifiableCheckBox_input"] = "on"
    fields["eeProfiles:textFieldPasswordUseCheckBox_input"] = "on"
    fields["eeProfiles:textFieldPasswordModifiableCheckBox_input"] = "on"
    fields["eeProfiles:defaultTokenMenu"] = "1"
    fields["eeProfiles:availableToken"] = ["1"]
    fields["eeProfiles:p12cipher"] = "PKCS12_AES256_AES128"
    fields["eeProfiles:defaultCertificateProfile"] = cert_ids[0]
    fields["eeProfiles:availableCertProfiles"] = cert_ids
    ca = _option_id(html, "eeProfiles:defaultCAMenu", "CCLI-Lab-CA")
    if not ca:
        raise RuntimeError("CCLI-Lab-CA not found on EE add form")
    fields["eeProfiles:defaultCAMenu"] = ca
    fields["eeProfiles:availableCA"] = [ca]
    fields["eeProfiles:saveButton"] = "Save"
    fields["jakarta.faces.ViewState"] = vs

    post: list[tuple[str, str]] = []
    for k, v in fields.items():
        if k.startswith("eeProfiles:subjectDnAttributeType"):
            continue
        if k.startswith("eeProfiles:subjectAltNameAttributeType"):
            continue
        if k.startswith("eeProfiles:subjectDirectoryAttributeType"):
            continue
        if isinstance(v, list):
            for item in v:
                post.append((k, item))
        else:
            post.append((k, v))

    r = session.post(EE_EDIT, data=post, timeout=120)
    r.raise_for_status()
    if not _ee_profile_exists(EE_PROFILE):
        if "ui-messages-error-summary" in r.text:
            errs = re.findall(r"ui-messages-error-summary\">([^<]+)", r.text)
            raise RuntimeError(f"EE profile save failed: {'; '.join(errs[:5])}")
        raise RuntimeError(f"EE profile {EE_PROFILE} not visible after save")
    print(f"  created end entity profile {EE_PROFILE}")


def _clone_cert_profile(session: requests.Session, template_row: int, new_name: str) -> None:
    if _profile_exists(new_name):
        print(f"  cert profile {new_name} exists — skip clone")
        return
    r = session.get(CP_URL, timeout=60)
    r.raise_for_status()
    vs = _viewstate(r.text)
    clone_name = f"editcertificateprofilesForm:editcertificateprofilesTable:{template_row}:j_idt64"
    data = {
        "editcertificateprofilesForm": "editcertificateprofilesForm",
        clone_name: "Clone",
        "jakarta.faces.ViewState": vs,
    }
    r2 = session.post(CP_URL, data=data, timeout=60)
    r2.raise_for_status()
    if "addFromTemplateProfileNew" not in r2.text:
        raise RuntimeError(f"Clone form not returned for {new_name}")
    form_m = re.search(r'<form id="([^"]+)"[^>]*action="[^"]*clonecertificateprofile', r2.text)
    if not form_m:
        raise RuntimeError("Clone form id not found")
    form_id = form_m.group(1)
    vs2 = _viewstate(r2.text)
    post = {
        form_id: form_id,
        f"{form_id}:addFromTemplateProfileNew": new_name,
        f"{form_id}:cloneConfirmButton": "Create from template",
        "jakarta.faces.ViewState": vs2,
    }
    r3 = session.post(CP_CLONE_URL, data=post, timeout=60)
    r3.raise_for_status()
    if not _profile_exists(new_name):
        raise RuntimeError(f"Failed to clone cert profile {new_name}")
    print(f"  cloned cert profile {new_name}")


def _configure_cert_profiles() -> None:
    # Cert A — TLS (62351-3 / 62351-9): serverAuth + clientAuth, SAN, RSA 2048
    for field, value in (
        ("useExtendedKeyUsage", "true"),
        ("extendedKeyUsageCritical", "false"),
        ("useSubjectAlternativeName", "true"),
        ("allowDNOverride", "true"),
        ("allowDNOverrideByEndEntityInformation", "true"),
        ("allowExtensionOverride", "true"),
    ):
        try:
            _edit_cp(CERT_A, field, value)
        except RuntimeError as exc:
            print(f"  WARN {CERT_A}.{field}: {exc}")

    try:
        _edit_cp(CERT_A, "extendedKeyUsageOids", "1.3.6.1.5.5.7.3.1;1.3.6.1.5.5.7.3.2")
    except RuntimeError as exc:
        print(f"  WARN {CERT_A}.extendedKeyUsageOids: {exc}")

    # Cert B — E2E MMS (G.5.2): digitalSignature + keyAgreement, no TLS EKU
    for field, value in (
        ("useExtendedKeyUsage", "false"),
        ("allowDNOverride", "true"),
        ("allowDNOverrideByEndEntityInformation", "true"),
    ):
        try:
            _edit_cp(CERT_B, field, value)
        except RuntimeError as exc:
            print(f"  WARN {CERT_B}.{field}: {exc}")

    # keyUsage boolean[9]: digitalSignature, keyAgreement (index 0 and 4)
    try:
        _edit_cp(CERT_B, "keyUsage", "true,false,false,false,true,false,false,false,false")
    except RuntimeError as exc:
        print(f"  WARN {CERT_B}.keyUsage: {exc}")

    print("  configured cert profile extensions via CLI")


def setup_profiles(skip_clone: bool = False) -> None:
    urllib3.disable_warnings()
    session = requests.Session()
    session.verify = False

    if not skip_clone:
        print("Cloning certificate profiles (SERVER -> Cert A, ENDUSER -> Cert B)...")
        # Row indices from editcertificateprofiles table: SERVER=3, ENDUSER=0
        _clone_cert_profile(session, 3, CERT_A)
        _clone_cert_profile(session, 0, CERT_B)

    print("Configuring Annex extensions on cloned profiles...")
    _configure_cert_profiles()

    print("Creating end entity profile (allows CCLI cert profiles)...")
    _create_ee_profile(session)

    print("OK — EJBCA Annex profiles ready.")


def main() -> int:
    ap = argparse.ArgumentParser(description="Setup EJBCA Annex CCLI cert profiles")
    ap.add_argument("--skip-clone", action="store_true", help="Only run CLI extension edits")
    args = ap.parse_args()
    try:
        setup_profiles(skip_clone=args.skip_clone)
    except Exception as exc:  # noqa: BLE001
        print(f"FAIL: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

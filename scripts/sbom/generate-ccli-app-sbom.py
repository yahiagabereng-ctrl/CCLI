#!/usr/bin/env python3
"""Generate CycloneDX 1.5 SBOM for HiTEKS ccli application (compile-time inventory)."""
from __future__ import annotations

import argparse
import datetime as dt
import json
import re
import subprocess
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
VERSION_FILE = REPO_ROOT / "apps" / "ccli" / "VERSION"
DEPLOY_LATEST = REPO_ROOT / "lab" / "tg544-openwrt" / "deploy-manifests" / "CCLI_DEPLOY_LATEST.md"


def read_version() -> tuple[str, str, str]:
    lines = [ln.strip() for ln in VERSION_FILE.read_text(encoding="utf-8").splitlines() if ln.strip()]
    # expected: semver, release N, codename
    semver = lines[0] if lines else "0.0.0"
    release = lines[1] if len(lines) > 1 else "0"
    codename = lines[2] if len(lines) > 2 else ""
    return semver, release, codename


def git_describe(repo_rel: str) -> str:
    path = REPO_ROOT / repo_rel
    if not (path / ".git").exists():
        return "unknown"
    try:
        out = subprocess.check_output(
            ["git", "-C", str(path), "describe", "--tags", "--always"],
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
        return out
    except subprocess.CalledProcessError:
        return "unknown"


def git_head() -> str:
    try:
        return subprocess.check_output(
            ["git", "-C", str(REPO_ROOT), "rev-parse", "--short", "HEAD"],
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
    except subprocess.CalledProcessError:
        return "unknown"


def sha256_from_deploy_latest() -> str:
    if not DEPLOY_LATEST.is_file():
        return ""
    text = DEPLOY_LATEST.read_text(encoding="utf-8")
    m = re.search(r"\*\*SHA256 \(staging\)\*\*\s*\|\s*`([0-9a-f]{64})`", text)
    return m.group(1) if m else ""


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument(
        "-o",
        "--output",
        type=Path,
        default=REPO_ROOT
        / "lab"
        / "evidence"
        / "EVD-SM-9-External-Components"
        / "CCLI_APP_SBOM_0.1.0-r64.cdx.json",
    )
    args = ap.parse_args()

    semver, release, codename = read_version()
    app_version = f"{semver}-r{release}"
    head = git_head()
    elf_sha = sha256_from_deploy_latest()

    components = [
        {
            "type": "library",
            "name": "libiec61850",
            "version": git_describe("apps/ccli/third_party/libiec61850"),
            "bom-ref": "pkg:github/mz-automation/libiec61850",
            "purl": "pkg:github/mz-automation/libiec61850",
            "description": "IEC 61850 MMS/GOOSE — statically linked into ccli",
            "licenses": [{"license": {"name": "GPL-3.0 / commercial (MZ Automation)"}}],
            "properties": [{"name": "ccli:link", "value": "static"}],
        },
        {
            "type": "library",
            "name": "lib60870",
            "version": git_describe("apps/ccli/third_party/lib60870"),
            "bom-ref": "pkg:github/mz-automation/lib60870",
            "purl": "pkg:github/mz-automation/lib60870",
            "description": "IEC 60870-5-104 CS104 — statically linked",
            "licenses": [{"license": {"name": "GPL-3.0 / commercial (MZ Automation)"}}],
            "properties": [{"name": "ccli:link", "value": "static"}],
        },
        {
            "type": "library",
            "name": "mbedtls",
            "version": "3.6.0",
            "bom-ref": "pkg:github/Mbed-TLS/mbedtls@3.6.0",
            "purl": "pkg:github/Mbed-TLS/mbedtls@3.6.0",
            "description": "Vendored under libiec61850/third_party/mbedtls — MMS/104 TLS (62351-3)",
            "licenses": [{"license": {"id": "Apache-2.0"}}],
            "properties": [{"name": "ccli:link", "value": "static-via-libiec61850"}],
        },
        {
            "type": "library",
            "name": "libmodbus",
            "version": "3.1.12-r1",
            "bom-ref": "pkg:apk/tesproos/libmodbus@3.1.12-r1",
            "purl": "pkg:apk/tesproos/libmodbus@3.1.12-r1",
            "description": "OpenWrt SDK runtime link at build (DUT apk list 2026-11-12)",
            "licenses": [{"license": {"id": "LGPL-2.1-or-later"}}],
            "properties": [{"name": "ccli:link", "value": "dynamic-platform"}],
        },
        {
            "type": "library",
            "name": "ccli_core",
            "version": app_version,
            "description": "PF2, measurement, events, config, net policy — HiTEKS",
            "properties": [{"name": "ccli:path", "value": "apps/ccli/core/"}],
        },
        {
            "type": "library",
            "name": "ccli_adapters",
            "version": app_version,
            "description": "MMS, 104, Modbus, GOOSE, GPIO adapters — HiTEKS",
            "properties": [{"name": "ccli:path", "value": "apps/ccli/adapters/"}],
        },
        {
            "type": "library",
            "name": "ccli_services",
            "version": app_version,
            "description": "svc_pf2, svc_modbus, svc_mms, svc_goose, svc_io — HiTEKS",
            "properties": [{"name": "ccli:path", "value": "apps/ccli/services/"}],
        },
        {
            "type": "library",
            "name": "ccli_program",
            "version": app_version,
            "description": "SK0153 program/ supervisor (main.c) — HiTEKS",
            "properties": [{"name": "ccli:path", "value": "apps/ccli/program/"}],
        },
        {
            "type": "library",
            "name": "ccli_platform",
            "version": app_version,
            "description": "platform_tg500 HAL — HiTEKS",
            "properties": [{"name": "ccli:path", "value": "apps/ccli/platform/platform_tg500/"}],
        },
    ]

    bom = {
        "bomFormat": "CycloneDX",
        "specVersion": "1.5",
        "serialNumber": f"urn:uuid:ccli-app-sbom-{app_version}-{head}",
        "version": 1,
        "metadata": {
            "timestamp": dt.datetime.now(dt.timezone.utc).replace(microsecond=0).isoformat(),
            "tools": [{"vendor": "HiTEKS", "name": "generate-ccli-app-sbom", "version": "1"}],
            "component": {
                "type": "application",
                "name": "ccli",
                "version": app_version,
                "description": codename or "CCI Central Plant Controller firmware",
                "properties": [
                    {"name": "ccli:codename", "value": codename},
                    {"name": "ccli:git-repo", "value": head},
                    {"name": "ccli:elf-sha256", "value": elf_sha},
                    {"name": "ccli:build-platform", "value": "tg500 / aarch64 musl (TesproOS)"},
                    {"name": "ccli:entry", "value": "program/main.c (default); CCLI_LEGACY_MAIN=ccli_main.cpp"},
                    {"name": "ccli:product-mirror", "value": "product/apps/ccli (SK0153 promote tree)"},
                    {
                        "name": "ccli:platform-sbom",
                        "value": "lab/evidence/EVD-SM-9-External-Components/intake/20261112-104506/platform-runtime-inventory.cdx.json",
                    },
                ],
            },
        },
        "components": components,
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(bom, indent=2) + "\n", encoding="utf-8")
    print(f"SBOM {app_version} ({len(components)} components) -> {args.output}")


if __name__ == "__main__":
    main()

# TesPro APK bundle vs Test Suite Pro (P3-07)

**Date:** 2026-09-29

## User note

Supplier folder `TG500_iec61850_apk_install` contains four TesPro packages installed on TG544 for vendor IEC 61850 feature — **not** Triangle **Test Suite Pro** source code.

## Do not conflate

| Artifact | Role in lab |
|----------|-------------|
| TesPro `iec61850-*.apk` | Vendor **MMS client/collector** (poll IEDs :102) |
| TMW **Test Suite Pro** | Windows **tester** against DUT `:3782` |
| **`ccli`** on DUT | **DSO MMS server** under P3-07 test |

## TMW support email

State clearly: **DUT under test is `ccli`**, not TesPro `iec61850-mmsd`. Attach pcap + TX_ACCEPT hex from `ccli` sessions.

TesPro APKs are **background context** for V-005 (vendor 61850 coexistence) only.

#!/usr/bin/env python3
"""Validate RAG chunk_quality_score for selected source_ids (Chronos pack aware)."""
from __future__ import annotations

import argparse
import json
import statistics
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

RAG = "http://127.0.0.1:8001"
PDF_PREFIX = "ccli-chronos-pdf-"
MD_MIN_CHUNKS = 1
PDF_MIN_MEAN = 0.50


def grade(mean: float) -> str:
    if mean >= 0.90:
        return "A"
    if mean >= 0.80:
        return "B"
    if mean >= 0.70:
        return "C"
    if mean >= 0.50:
        return "D"
    return "F"


def is_pdf_source(source_id: str) -> bool:
    return source_id.startswith(PDF_PREFIX)


def fetch_chunks(source_id: str) -> tuple[int, list[float], list[tuple[float, str]]]:
    scores: list[float] = []
    lowest: list[tuple[float, str]] = []
    total = 0
    offset = 0
    limit = 500
    while True:
        q = urllib.parse.urlencode(
            {
                "tenant_id": "telematry",
                "bot_id": "default",
                "source_ids": source_id,
                "chunk_type": "all",
                "limit": limit,
                "offset": offset,
            }
        )
        url = f"{RAG}/v1/classification/chunks?{q}"
        with urllib.request.urlopen(url, timeout=120) as resp:
            page = json.load(resp)
        items = page.get("items") or []
        total = int(page.get("total") or 0)
        for it in items:
            meta = it.get("metadata") or {}
            qv = meta.get("chunk_quality_score")
            if qv is None:
                continue
            qf = float(qv)
            scores.append(qf)
            preview = (it.get("text") or meta.get("preview") or "")[:80].replace("\n", " ")
            lowest.append((qf, preview))
        offset += limit
        if offset >= total:
            break
    lowest.sort(key=lambda x: x[0])
    return total, scores, lowest[:3]


def write_scorecard(source_id: str, total: int, scores: list[float], lowest: list[tuple[float, str]]) -> None:
    repo = Path(__file__).resolve().parents[2]
    out_dir = repo / "_chunk_score"
    out_dir.mkdir(exist_ok=True)
    if not scores:
        return
    mean = statistics.mean(scores)
    payload = {
        "source_id": source_id,
        "chunk_count": total,
        "scored_chunks": len(scores),
        "quality": {
            "mean": round(mean, 4),
            "median": round(statistics.median(scores), 4),
            "min": round(min(scores), 4),
            "max": round(max(scores), 4),
            "grade": grade(mean),
        },
        "lowest": [{"q": q, "preview": p} for q, p in lowest],
    }
    safe = source_id.replace("/", "_")[:80]
    (out_dir / f"{safe}_chunk_scorecard.json").write_text(
        json.dumps(payload, indent=2), encoding="utf-8"
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--chronos-pack", action="store_true", help="Apply MD vs PDF pass rules")
    parser.add_argument("source_ids", nargs="*", default=[])
    args = parser.parse_args()
    ids = args.source_ids or [
        "ccli-chronos-emt432-vendor-requirements",
        "ccli-chronos-sta-test-reports",
    ]

    try:
        with urllib.request.urlopen(f"{RAG}/health", timeout=8) as resp:
            health = json.load(resp)
        if health.get("status") != "ok":
            print("RAG health not ok:", health)
            return 2
    except Exception as exc:
        print("RAG unreachable:", exc)
        return 2

    print(f"RAG OK — checking {len(ids)} source_id(s)\n")
    report_rows: list[dict] = []
    exit_code = 0

    for sid in ids:
        row: dict = {"source_id": sid, "kind": "pdf" if is_pdf_source(sid) else "md"}
        try:
            total, scores, low = fetch_chunks(sid)
        except urllib.error.HTTPError as exc:
            print(f"{sid}: HTTP {exc.code}")
            row["verdict"] = "HTTP_ERROR"
            report_rows.append(row)
            exit_code = 1
            continue

        row["chunks"] = total
        row["scored"] = len(scores)

        if total == 0:
            print(f"{sid}: MISSING (0 chunks)")
            row["verdict"] = "MISSING"
            report_rows.append(row)
            exit_code = 1
            continue

        if is_pdf_source(sid):
            if not scores:
                print(f"{sid}: PDF chunks={total} but no chunk_quality_score yet (wait for worker)")
                row["verdict"] = "PDF_PENDING_SCORE"
                exit_code = 1
            else:
                mean = statistics.mean(scores)
                g = grade(mean)
                row["mean_quality"] = round(mean, 4)
                row["grade"] = g
                ok = mean >= PDF_MIN_MEAN
                row["verdict"] = "PASS" if ok else "FAIL_LOW_SCORE"
                print(
                    f"{sid}: PDF chunks={total} scored={len(scores)} "
                    f"mean={mean:.4f} grade={g} -> {'PASS' if ok else 'FAIL'}"
                )
                if not ok:
                    exit_code = 1
                for q, prev in low:
                    print(f"  low {q:.4f}: {prev!r}")
                write_scorecard(sid, total, scores, low)
        else:
            ok = total >= MD_MIN_CHUNKS
            row["verdict"] = "PASS_MD" if ok else "FAIL_MD"
            print(
                f"{sid}: MD chunks={total} (structured ingest; chunk_quality_score N/A) -> "
                f"{'PASS' if ok else 'FAIL'}"
            )
            if not ok:
                exit_code = 1

        report_rows.append(row)

    repo = Path(__file__).resolve().parents[2]
    out = repo / "lab" / "evidence" / "CHRONOS_RAG_CHUNK_REPORT.json"
    out.write_text(
        json.dumps({"sources": report_rows, "chronos_pack": args.chronos_pack}, indent=2),
        encoding="utf-8",
    )
    print(f"\nReport: {out}")
    return exit_code


if __name__ == "__main__":
    raise SystemExit(main())

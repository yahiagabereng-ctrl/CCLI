#!/usr/bin/env python3
"""Assemble CCI Master Reference LaTeX from manifest + Markdown sources."""

from __future__ import annotations

import json
import re
import subprocess
import sys
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "scripts" / "master-reference-manifest.json"
PREAMBLE = ROOT / "scripts" / "templates" / "master_reference_preamble.tex"
BUILD_DIR = ROOT / "knowledge-base" / "_build"
OUT_TEX = BUILD_DIR / "CCI_Master_Reference.tex"
OUT_TEX_PUBLIC = ROOT / "knowledge-base" / "CCI_Master_Reference.tex"

MOJIBAKE = {
    "â€”": "—",
    "â€“": "–",
    "â€˜": "'",
    "â€™": "'",
    "â€œ": '"',
    "â€": '"',
    "â€¦": "…",
    "â†’": "→",
    "â†”": "↔",
    "â†": "←",
    "Â·": "·",
    "Â§": "§",
    "â‰ˆ": "≈",
    "â‰¥": "≥",
    "Ã—": "×",
    "â‚¬": "€",
    "â„¢": "™",
    "âˆ’": "−",
}


def fix_mojibake(text: str) -> str:
    for bad, good in MOJIBAKE.items():
        text = text.replace(bad, good)
    text = re.sub(r"[\u009d\u009c\u0080-\u009f]", "", text)
    for ch, repl in {
        "✅": "[OK]",
        "⚠️": "[!]",
        "⚠": "[!]",
        "→": "->",
        "←": "<-",
        "↔": "<->",
        "—": "-",
        "–": "-",
        "−": "-",
        "×": "x",
        "≥": ">=",
        "≤": "<=",
        "≈": "~",
        "§": "Sec.",
        "″": '"',
        "′": "'",
    }.items():
        text = text.replace(ch, repl)
    return text


def esc(s: str) -> str:
    s = fix_mojibake(s)
    repl = {
        "\\": r"\textbackslash{}",
        "&": r"\&",
        "%": r"\%",
        "$": r"\$",
        "#": r"\#",
        "_": r"\_",
        "{": r"\{",
        "}": r"\}",
        "~": r"\textasciitilde{}",
        "^": r"\textasciicircum{}",
    }
    out = []
    i = 0
    while i < len(s):
        if s[i : i + 2] == "**":
            end = s.find("**", i + 2)
            if end != -1:
                out.append(r"\textbf{" + esc(s[i + 2 : end]) + "}")
                i = end + 2
                continue
        if s[i : i + 1] == "`":
            end = s.find("`", i + 1)
            if end != -1:
                out.append(r"\file{" + esc(s[i + 1 : end]) + "}")
                i = end + 1
                continue
        if s[i : i + 1] == "*":
            end = s.find("*", i + 1)
            if end != -1:
                out.append(r"\emph{" + esc(s[i + 1 : end]) + "}")
                i = end + 1
                continue
        c = s[i]
        out.append(repl.get(c, c))
        i += 1
    return "".join(out)


def status_replacements(line: str) -> str:
    line = line.replace("**HAVE / INGESTED**", r"\statushave{} / \statusingested{}")
    mapping = {
        "INGESTED": r"\statusingested{}",
        "MISSING": r"\statusmissing{}",
        "PARTIAL": r"\statuspartial{}",
        "HAVE": r"\statushave{}",
    }
    for word, cmd in mapping.items():
        line = line.replace(f"**{word}**", cmd)
        line = re.sub(rf"\b{word}\b", lambda _m, c=cmd: c, line)
    return line


def _xcols(weights: list[float]) -> str:
    """Proportional tabularx X columns. Weights must sum to len(weights)."""
    n = len(weights)
    total = sum(weights)
    if abs(total - n) > 1e-6:
        weights = [w * n / total for w in weights]
    parts = []
    for w in weights:
        parts.append(
            f">{{\\hsize={w:.3f}\\hsize\\RaggedRight\\arraybackslash}}X"
        )
    return "".join(parts)


def table_colspec(ncols: int, headers: list[str] | None = None) -> tuple[str, str]:
    """Return (inner colspec, env). Prefer X ratios so tabcolsep never clips columns."""
    headers_l = [h.lower() for h in (headers or [])]
    joined = " ".join(headers_l)

    if ncols == 7:
        weights = [0.55, 1.45, 0.85, 0.45, 0.70, 1.25, 1.75]
    elif ncols == 6 and "status" in joined:
        weights = [0.70, 1.50, 0.90, 0.55, 0.85, 1.50]
    elif ncols == 5 and "url" in joined:
        weights = [1.10, 1.90, 0.70, 1.20, 1.10]
    elif ncols == 5 and "priority" in joined:
        weights = [0.70, 0.90, 1.70, 0.60, 1.10]
    elif ncols == 5:
        weights = [0.85, 1.25, 1.10, 0.90, 0.90]
    elif ncols == 4 and "url" in joined:
        weights = [1.00, 1.60, 0.70, 0.70]
    elif ncols == 4:
        weights = [0.90, 1.30, 0.90, 0.90]
    elif ncols == 3:
        weights = [1.00, 1.00, 1.00]
    elif ncols == 2:
        weights = [1.00, 1.00]
    else:
        weights = [1.0] * max(ncols, 1)
    return _xcols(weights), "tabularx"


def format_cell(text: str) -> str:
    """Escape cell text; wrap bare URLs so xurl can break them."""
    raw = fix_mojibake(text.strip())
    if re.fullmatch(r"https?://\S+", raw):
        safe = raw.replace("\\", "").replace("%", r"\%").replace("#", r"\#")
        return r"\url{" + safe + "}"
    return status_replacements(esc(raw))


def parse_table(lines: list[str], start: int) -> tuple[str, int]:
    rows: list[list[str]] = []
    i = start
    while i < len(lines):
        line = lines[i].strip()
        if not line.startswith("|"):
            break
        cells = [c.strip() for c in line.strip("|").split("|")]
        if all(re.fullmatch(r":?-+:?", c.replace(" ", "")) for c in cells):
            i += 1
            continue
        rows.append(cells)
        i += 1
    if not rows:
        return "", start

    ncols = max(len(r) for r in rows)
    headers = rows[0] if rows else []
    inner, env = table_colspec(ncols, headers)
    has_url = any("url" in h.lower() for h in headers) or any(
        re.search(r"https?://", c) for row in rows for c in row
    )
    font_size = "\\scriptsize" if ncols >= 6 or has_url else "\\small"
    sep = "2.0pt" if ncols >= 6 or has_url else "3pt"
    stretch = "1.12" if ncols >= 6 else "1.15"
    if env == "tabularx":
        begin = (
            "\\par\\smallskip\\noindent\n"
            f"{font_size}\n"
            f"\\setlength{{\\tabcolsep}}{{{sep}}}\n"
            f"\\renewcommand{{\\arraystretch}}{{{stretch}}}\n"
            f"\\begin{{tabularx}}{{\\linewidth}}{{{inner}}}\n\\toprule\n"
        )
        end = (
            "\\bottomrule\n\\end{tabularx}\n"
            "\\normalsize\n\\par\\smallskip"
        )
    else:
        begin = (
            "\\par\\smallskip\n"
            f"{font_size}\n"
            f"\\setlength{{\\tabcolsep}}{{{sep}}}\n"
            f"\\renewcommand{{\\arraystretch}}{{{stretch}}}\n"
            f"\\begin{{longtable}}{{{inner}}}\n\\toprule\n"
        )
        end = "\\bottomrule\n\\end{longtable}\n\\normalsize\n\\par\\smallskip"

    body = []
    for ri, row in enumerate(rows):
        while len(row) < ncols:
            row.append("")
        cells = [format_cell(c) for c in row]
        body.append(" & ".join(cells) + r" \\")
        if ri == 0:
            body.append("\\midrule")
    tex = begin + "\n".join(body) + "\n" + end + "\n"
    return tex, i


def md_to_latex(md: str) -> str:
    md = fix_mojibake(md)
    lines = md.splitlines()
    out: list[str] = []
    i = 0
    in_code = False
    in_ul = False
    in_ol = False

    def close_lists():
        nonlocal in_ul, in_ol
        if in_ul:
            out.append("\\end{itemize}")
            in_ul = False
        if in_ol:
            out.append("\\end{enumerate}")
            in_ol = False

    while i < len(lines):
        raw = lines[i]
        line = raw.rstrip()

        if line.strip().startswith("```"):
            close_lists()
            in_code = not in_code
            if in_code:
                out.append("\\begin{verbatim}")
            else:
                out.append("\\end{verbatim}")
            i += 1
            continue

        if in_code:
            out.append(line)
            i += 1
            continue

        if not line.strip():
            close_lists()
            i += 1
            continue

        if re.match(r"^#{1,4}\s", line):
            close_lists()
            level = len(re.match(r"^(#+)", line).group(1))
            title = line[level:].strip()
            title = re.sub(r"^#+\s*", "", title)
            title = re.sub(r"^\d+\.\s+", "", title)
            if level == 1:
                out.append(f"\\Needspace{{5\\baselineskip}}\\section{{{esc(title)}}}")
            elif level == 2:
                out.append(f"\\Needspace{{4\\baselineskip}}\\subsection{{{esc(title)}}}")
            elif level == 3:
                out.append(f"\\Needspace{{3\\baselineskip}}\\subsubsection{{{esc(title)}}}")
            else:
                out.append(f"\\paragraph{{{esc(title)}}}")
            i += 1
            continue

        if line.strip() == "---":
            close_lists()
            out.append(
                "\\par\\vspace{0.35em}\\noindent"
                "\\textcolor{HiteksOrange}{\\rule{\\linewidth}{0.4pt}}"
                "\\par\\vspace{0.35em}"
            )
            i += 1
            continue

        if line.strip().startswith("|"):
            close_lists()
            tbl, i = parse_table(lines, i)
            out.append(tbl)
            continue

        if re.match(r"^[-*]\s+", line):
            if in_ol:
                out.append("\\end{enumerate}")
                in_ol = False
            if not in_ul:
                out.append("\\begin{itemize}")
                in_ul = True
            item = re.sub(r"^[-*]\s+", "", line.strip())
            out.append(f"  \\item {status_replacements(esc(item))}")
            i += 1
            continue

        if re.match(r"^\d+\.\s+", line):
            if in_ul:
                out.append("\\end{itemize}")
                in_ul = False
            if not in_ol:
                out.append("\\begin{enumerate}")
                in_ol = True
            item = re.sub(r"^\d+\.\s+", "", line.strip())
            out.append(f"  \\item {status_replacements(esc(item))}")
            i += 1
            continue

        if line.strip().startswith(">"):
            close_lists()
            quote = re.sub(r"^>\s?", "", line.strip())
            out.append(f"\\begin{{quote}}\n{status_replacements(esc(quote))}\n\\end{{quote}}")
            i += 1
            continue

        close_lists()
        out.append(status_replacements(esc(line.strip())))
        i += 1

    close_lists()
    return "\n".join(out)


def load_manifest() -> dict:
    with MANIFEST.open(encoding="utf-8") as f:
        return json.load(f)


def count_sources(manifest: dict) -> int:
    return sum(len(s["files"]) for s in manifest["sections"])


def assemble() -> Path:
    manifest = load_manifest()
    BUILD_DIR.mkdir(parents=True, exist_ok=True)

    preamble = PREAMBLE.read_text(encoding="utf-8")
    preamble = preamble.replace("%%DOC_ID%%", manifest["document_id"])
    preamble = preamble.replace("%%REVISION%%", manifest["revision"])
    preamble = preamble.replace("%%TITLE%%", manifest["title"])
    preamble = preamble.replace("%%SUBTITLE%%", manifest["subtitle"])
    preamble = preamble.replace("%%BUILD_DATE%%", date.today().isoformat())
    preamble = preamble.replace("%%SOURCE_COUNT%%", str(count_sources(manifest)))

    parts: list[str] = [preamble]

    for section in manifest["sections"]:
        parts.append(f"\\clearpage\n\\part{{{esc(section['part'])}}}")
        for entry in section["files"]:
            src = ROOT / entry["path"]
            if not src.exists():
                print(f"WARNING: missing source {src}", file=sys.stderr)
                parts.append(
                    f"\\chapter{{{esc(entry['label'])}}}\n"
                    f"\\textcolor{{missred}}{{\\textbf{{Source file missing:}} \\file{{{esc(entry['path'])}}}}}"
                )
                continue
            md = src.read_text(encoding="utf-8", errors="replace")
            body = md_to_latex(md)
            label = entry["path"].replace("/", "-").replace("\\", "-")
            parts.append(
                f"\\chapter{{{esc(entry['label'])}}}\n\\label{{src:{label}}}\n{body}"
            )

    parts.append("\\end{document}\n")
    OUT_TEX.write_text("\n".join(parts), encoding="utf-8")
    OUT_TEX_PUBLIC.write_text(OUT_TEX.read_text(encoding="utf-8"), encoding="utf-8")
    print(f"Wrote {OUT_TEX}")
    print(f"LaTeX: {OUT_TEX_PUBLIC}")
    return OUT_TEX


def main() -> int:
    tex = assemble()
    if "--tex-only" in sys.argv:
        return 0

    pdflatex = "pdflatex"
    pdf_src = BUILD_DIR / "CCI_Master_Reference.pdf"
    for _ in range(2):
        subprocess.run(
            [pdflatex, "-interaction=nonstopmode", "-output-directory", str(BUILD_DIR), str(tex.name)],
            cwd=BUILD_DIR,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
        )

    if not pdf_src.exists():
        log = BUILD_DIR / "CCI_Master_Reference.log"
        tail = log.read_text(encoding="utf-8", errors="replace")[-4000:] if log.exists() else ""
        print(tail, file=sys.stderr)
        print("pdflatex did not produce PDF", file=sys.stderr)
        return 1

    pdf_dst = ROOT / "knowledge-base" / "CCI_Master_Reference.pdf"
    pdf_dst.write_bytes(pdf_src.read_bytes())
    print(f"Output: {pdf_dst}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

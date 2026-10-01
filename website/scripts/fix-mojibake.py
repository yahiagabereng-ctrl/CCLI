"""Fix UTF-8 mojibake in website source strings."""
from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1] / "src"

# Common UTF-8 sequences that were decoded as Windows-1252 / Latin-1
REPLACEMENTS = [
    ("â‚¬", "€"),
    ("â€“", "–"),
    ("â€”", "—"),
    ("â€˜", "‘"),
    ("â€™", "’"),
    ("â€œ", "“"),
    ("â€", "”"),
    ("â€¦", "…"),
    ("â†’", "→"),
    ("â‰¥", "≥"),
    ("â‰¤", "≤"),
    ("Ã—", "×"),
    ("Â·", "·"),
    ("Â ", " "),
]


def fix_text(text: str) -> str:
    for bad, good in REPLACEMENTS:
        text = text.replace(bad, good)
    return text


def main() -> None:
    changed: list[str] = []
    for path in ROOT.rglob("*"):
        if path.suffix.lower() not in {".ts", ".tsx", ".css", ".json", ".md", ".html"}:
            continue
        raw = path.read_bytes()
        try:
            text = raw.decode("utf-8")
        except UnicodeDecodeError:
            continue
        fixed = fix_text(text)
        if fixed != text:
            path.write_text(fixed, encoding="utf-8", newline="\n")
            changed.append(str(path.relative_to(ROOT)))
    print("fixed", len(changed), "files")
    for c in changed:
        print(" ", c)


if __name__ == "__main__":
    main()

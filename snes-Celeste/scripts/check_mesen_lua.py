import pathlib
import re
import sys


ROOT = pathlib.Path(__file__).resolve().parents[1]
SCRIPTS_DIR = ROOT / "scripts"
WRAM_LITERAL_RE = re.compile(r"0x7[EF][0-9A-Fa-f]{4,6}")


def strip_line_comment(line: str) -> str:
    comment = line.find("--")
    if comment >= 0:
        return line[:comment]
    return line


def main() -> int:
    failures = []
    for path in sorted(SCRIPTS_DIR.glob("*.lua")):
        if path.name == "mesen_testlib.lua":
            continue
        with path.open("r", encoding="utf-8") as f:
            for line_no, raw_line in enumerate(f, start=1):
                line = strip_line_comment(raw_line)
                for match in WRAM_LITERAL_RE.finditer(line):
                    failures.append(f"{path.relative_to(ROOT)}:{line_no}: hardcoded CPU WRAM address {match.group(0)}")

    if failures:
        sys.stderr.write("Mesen Lua harness check failed.\n")
        sys.stderr.write("Use mesen_symbols.lua via scripts/mesen_testlib.lua instead of hardcoded 0x7E/0x7F WRAM addresses.\n")
        for failure in failures:
            sys.stderr.write(failure + "\n")
        return 1

    print("Mesen Lua harness check passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
import re
import sys
from pathlib import Path


TEXT_PALETTE_SLOT = 7
PALETTE_MASK = 0x07


def strip_c_comments(text):
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return re.sub(r"//.*", "", text)


def parse_words(body):
    return [
        int(token, 16) if token.lower().startswith("0x") else int(token)
        for token in re.findall(r"0x[0-9A-Fa-f]+|\b\d+\b", body)
    ]


def parse_array(text, decl):
    match = re.search(
        r"const\s+unsigned\s+short\s+%s\s*\[\]\s*=\s*\{(.*?)\};" % re.escape(decl),
        text,
        re.S,
    )
    if match is None:
        return None
    return parse_words(match.group(1))


def main():
    repo = Path(__file__).resolve().parents[1]
    level_dir = repo / "src" / "levelDat"
    failures = []

    for path in sorted(
        level_dir.glob("tilemap_level*.h"),
        key=lambda p: int(re.search(r"level(\d+)", p.name).group(1)),
    ):
        level = int(re.search(r"level(\d+)", path.name).group(1))
        text = strip_c_comments(path.read_text(errors="ignore"))

        bg2_entries = parse_array(text, "tilemap_level%d_bg2" % level)
        if bg2_entries is None:
            failures.append("%s: missing bg2 tilemap" % path.name)
            continue

        bg2_slot_users = [
            i
            for i, entry in enumerate(bg2_entries)
            if entry != 0 and (((entry >> 10) & PALETTE_MASK) == TEXT_PALETTE_SLOT)
        ]
        if bg2_slot_users:
            failures.append(
                "%s: BG2 uses zero-based palette slot %d at tilemap offsets %s"
                % (path.name, TEXT_PALETTE_SLOT, bg2_slot_users[:8])
            )

        palette = parse_array(text, "palette_level%d" % level)
        if palette is None:
            failures.append("%s: missing palette" % path.name)
            continue

        if len(palette) < 32:
            failures.append("%s: palette has fewer than 8 BG slots" % path.name)
            continue

        remap_match = re.search(
            r"#define\s+BG2_TEXT_PALETTE_REMAP_LEVEL%d\s+(\d+)" % level,
            text,
        )
        if remap_match is None:
            failures.append("%s: missing BG2 text palette remap define" % path.name)
            continue
        remap = int(remap_match.group(1))
        if remap != 255:
            if remap >= TEXT_PALETTE_SLOT:
                failures.append("%s: invalid BG2 text palette remap slot %d" % (path.name, remap))
            if not any(entry != 0 and (((entry >> 10) & PALETTE_MASK) == remap) for entry in bg2_entries):
                failures.append("%s: BG2 text remap slot %d is not used after remap" % (path.name, remap))

    if failures:
        for failure in failures:
            print("FAIL:", failure)
        return 1

    print(
        "PASS: SNES BG2 tilemaps do not use zero-based palette slot %d; BG2 room-load remaps are valid"
        % TEXT_PALETTE_SLOT
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())

#!/bin/bash
# Test all 5 tracks - patch, build, render, restore
set -e
SPC_DIR="C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700"
MAINC="$SPC_DIR/main.c"

# Save original
cp "$MAINC" "$MAINC.bak"

declare -a NAMES=(mus0 mus10 mus20 mus30 mus40)
declare -a PATS=(0 10 20 30 40)
declare -a DURS=(47 45 59 30 8)

for i in 0 1 2 3 4; do
    name="${NAMES[$i]}"
    pat="${PATS[$i]}"
    dur="${DURS[$i]}"

    # Patch current_pattern
    sed -i "s/current_pattern = [0-9]*;  \/\/ patched/current_pattern = ${pat};  \/\/ patched/" "$MAINC"

    # Build
    cd "$SPC_DIR"
    bash build.sh 2>/dev/null

    # Render
    /c/cygwin64/bin/bash.exe -l -c "cd \"$SPC_DIR\" && ./spc2wav.exe main.spc data_${name}.wav ${dur}" 2>&1

    echo "Rendered data_${name}.wav (pattern ${pat}, ${dur}s)"
done

# Restore
cp "$MAINC.bak" "$MAINC"
bash build.sh 2>/dev/null
rm -f "$MAINC.bak"
echo "Done - all tracks rendered"

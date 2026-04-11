#!/usr/bin/env python
"""
Convert level animation PNGs (Level4A.png, Level4B.png) to NES data.

Each image contains 12 animation frames laid out horizontally (3072x480).
Each frame is 256x480 (two NES nametables: NT0 + NT2).

Builds its OWN tile dictionary from all frames of both images (like
convert_title_screen.py). Generates a dedicated CHR bank (level4_chr.bin)
with the unique BG tiles + the game's sprite pattern table.

Outputs:
  - level4_chr.bin         : 8KB CHR bank (BG tiles + sprite tiles)
  - level4_anim_data.h     : C header with RLE nametables, attributes, deltas, palette
"""

import os
import numpy as np
from PIL import Image

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
LEVEL_DIR = os.path.join(SCRIPT_DIR, "overrides", "nes", "levels")
OUTPUT_DIR = SCRIPT_DIR

FRAME_COUNT = 12
FRAME_WIDTH = 256
FRAME_HEIGHT = 480
TILES_X = 32
TILES_Y = 60

# The game area is 32 tile rows (256px) starting at image tile row 28 (pixel 224).
# Map these to NT0 rows 0-29 + NT2 rows 0-1 (matching write_nametable layout).
GAME_START_ROW = 28  # First image tile row with game content
GAME_ROWS = 32       # 16 game tiles * 2 NES tiles each
NT0_ROWS = 30        # NES nametable 0 rows
NT2_GAME_ROWS = 2    # Overflow into NT2

NES_PALETTE = [
    (84,84,84),    (0,30,116),    (8,16,144),    (48,0,136),
    (68,0,100),    (92,0,48),     (84,4,0),      (60,24,0),
    (32,42,0),     (8,58,0),      (0,64,0),      (0,60,0),
    (0,50,60),     (0,0,0),       (0,0,0),        (0,0,0),
    (152,150,152), (8,76,196),    (48,50,236),   (92,30,228),
    (136,20,176),  (160,20,100),  (152,34,32),   (120,60,0),
    (84,90,0),     (40,114,0),    (8,124,0),     (0,118,40),
    (0,102,120),   (0,0,0),       (0,0,0),        (0,0,0),
    (236,238,236), (76,154,236),  (120,124,236), (176,98,236),
    (228,84,236),  (236,88,180),  (236,106,100), (212,136,32),
    (160,170,0),   (116,196,0),   (76,208,32),   (56,204,108),
    (56,180,204),  (60,60,60),    (0,0,0),        (0,0,0),
    (236,238,236), (168,204,236), (188,188,236), (212,178,236),
    (236,174,236), (236,174,212), (236,180,176), (228,196,144),
    (204,210,120), (180,222,120), (168,226,144), (152,226,180),
    (160,214,228), (160,162,160), (0,0,0),        (0,0,0),
]

def rgb_to_nes_index(r, g, b):
    if r == 0 and g == 0 and b == 0:
        return 0x0F
    best_idx = 0x0F
    best_dist = float('inf')
    for idx, (pr, pg, pb) in enumerate(NES_PALETTE):
        if idx in (0x0D, 0x0E, 0x0F, 0x1D, 0x1E, 0x1F, 0x2D, 0x2E, 0x2F, 0x3D, 0x3E, 0x3F):
            if not (r == 0 and g == 0 and b == 0):
                continue
        dist = (r - pr) ** 2 + (g - pg) ** 2 + (b - pb) ** 2
        if dist < best_dist:
            best_dist = dist
            best_idx = idx
    return best_idx

def tile_to_chr(tile_pixels):
    """Convert 8x8 tile (2-bit pixel values 0-3) to 16-byte NES CHR format."""
    chr_data = bytearray(16)
    for row in range(8):
        bp0 = 0
        bp1 = 0
        for col in range(8):
            pixel = tile_pixels[row, col] & 3
            bp0 |= ((pixel >> 0) & 1) << (7 - col)
            bp1 |= ((pixel >> 1) & 1) << (7 - col)
        chr_data[row] = bp0
        chr_data[row + 8] = bp1
    return bytes(chr_data)

def rle_encode(data):
    encoded = []
    i = 0
    while i < len(data):
        val = data[i]
        run = 1
        while i + run < len(data) and data[i + run] == val and run < 128:
            run += 1
        encoded.append(run - 1)
        encoded.append(val)
        i += run
    return encoded

def get_collapse_tile_positions():
    """Get nametable positions occupied by collapse tiles (type 23) on level 4.
    Returns set of NT positions (0-959 for NT0, 960+ for NT2)."""
    # Level 4 objects: (type, game_x, game_y)
    objects = [
        (28, 2, 4), (23, 12, 6), (23, 13, 6), (23, 11, 9), (23, 12, 9),
        (23, 14, 11), (23, 15, 11), (23, 12, 13), (23, 13, 13),
        (23, 8, 14), (23, 9, 14), (23, 4, 15), (23, 5, 15),
    ]
    positions = set()
    for otype, gx, gy in objects:
        if otype != 23:
            continue
        nx, ny = gx * 2, gy * 2  # Game tile -> NES tile coords
        for dy in range(2):
            for dx in range(2):
                row = ny + dy
                col = nx + dx
                if row < NT0_ROWS:
                    positions.add(row * TILES_X + col)
                else:
                    positions.add(960 + (row - NT0_ROWS) * TILES_X + col)
    return positions

# Nametable positions reserved for collapse tiles (animation must not touch these)
COLLAPSE_TILE_POSITIONS = get_collapse_tile_positions()

def process_image(img, tile_dict):
    """Process one level animation image using the tile dictionary.

    Extracts the 32-row game area (starting at GAME_START_ROW in the image)
    and maps it to NT0 rows 0-29 + NT2 rows 0-1, matching the normal
    write_nametable() layout. Empty tile (index 0, black) fills unused rows.
    Collapse tile positions are set to tile 0 (game BG system fills them in).
    """
    data = np.array(img)
    # Find the empty tile index (all-zero pixels)
    empty_key = np.zeros((8, 8), dtype=data.dtype).tobytes()
    empty_idx = tile_dict.get(empty_key, 0)

    frames = []
    for f in range(FRAME_COUNT):
        frame = data[:, f * FRAME_WIDTH:(f + 1) * FRAME_WIDTH]

        # Extract game area tiles and palettes (32 rows from GAME_START_ROW)
        game_tiles = []  # 32 rows of 32 tile indices
        game_pals = []   # 32 rows of 32 palette values
        for gy in range(GAME_ROWS):
            img_row = GAME_START_ROW + gy
            row_tiles = []
            row_pal = []
            for tx in range(TILES_X):
                tile = frame[img_row * 8:(img_row + 1) * 8, tx * 8:(tx + 1) * 8]
                key = tile.tobytes()
                row_tiles.append(tile_dict[key])
                nonzero = tile[tile > 0]
                row_pal.append(int(nonzero[0]) // 4 if len(nonzero) > 0 else 0)
            game_tiles.append(row_tiles)
            game_pals.append(row_pal)

        # Build NT0: game rows 0-29 (first 30 of 32 game tile rows)
        nt0 = []
        nt0_pals = []
        for row in range(NT0_ROWS):
            if row < GAME_ROWS:
                nt0.extend(game_tiles[row])
                nt0_pals.append(game_pals[row])
            else:
                nt0.extend([empty_idx] * TILES_X)
                nt0_pals.append([0] * TILES_X)

        # Build NT2: game rows 30-31, then empty for the rest
        nt2 = []
        nt2_pals = []
        for row in range(30):  # Full 30-row nametable
            game_row = NT0_ROWS + row  # Offset from game area
            if game_row < GAME_ROWS:
                nt2.extend(game_tiles[game_row])
                nt2_pals.append(game_pals[game_row])
            else:
                nt2.extend([empty_idx] * TILES_X)
                nt2_pals.append([0] * TILES_X)

        # Build attribute tables
        def build_attributes(tile_pals):
            attrs = []
            for ay in range(8):
                for ax in range(8):
                    tx_base = ax * 4
                    ty_base = ay * 4
                    def get_pal(tx, ty):
                        if ty >= len(tile_pals) or tx >= TILES_X:
                            return 0
                        return tile_pals[ty][tx]
                    tl = get_pal(tx_base, ty_base)
                    tr = get_pal(tx_base + 2, ty_base)
                    bl = get_pal(tx_base, ty_base + 2)
                    br = get_pal(tx_base + 2, ty_base + 2)
                    attrs.append((tl & 3) | ((tr & 3) << 2) | ((bl & 3) << 4) | ((br & 3) << 6))
            return attrs

        # Clear collapse tile positions — the game's BG tile system draws them.
        for pos in COLLAPSE_TILE_POSITIONS:
            if pos < 960:
                nt0[pos] = empty_idx
            else:
                nt2_pos = pos - 960
                if nt2_pos < len(nt2):
                    nt2[nt2_pos] = empty_idx

        nt0_attrs = build_attributes(nt0_pals)
        nt2_attrs = build_attributes(nt2_pals)
        frames.append((nt0, nt2, nt0_attrs, nt2_attrs))
    return frames

def main():
    img_a = Image.open(os.path.join(LEVEL_DIR, "Level4A.png"))
    img_b = Image.open(os.path.join(LEVEL_DIR, "Level4B.png"))
    print(f"Level4A: {img_a.size}, Level4B: {img_b.size}")

    # Reserve CHR tile indices used by game objects (collapse tiles, breakable walls)
    # so they can coexist with animation tiles at $1000.
    # The game copies these tile patterns from game CHR to $1000 at load time.
    RESERVED_INDICES = set(range(73, 85))  # Collapse tile CHR indices 73-84
    print(f"Reserved CHR indices for game objects: {sorted(RESERVED_INDICES)}")

    # Build combined tile dictionary from both images, skipping reserved indices
    tile_dict = {}   # key: tile.tobytes() -> index
    tile_list = []   # list of (index, numpy 8x8 tile) - index may skip reserved slots
    next_index = 0

    def alloc_index():
        nonlocal next_index
        while next_index in RESERVED_INDICES:
            next_index += 1
        idx = next_index
        next_index += 1
        return idx

    for img in [img_a, img_b]:
        data = np.array(img)
        for f in range(FRAME_COUNT):
            frame = data[:, f * FRAME_WIDTH:(f + 1) * FRAME_WIDTH]
            for ty in range(TILES_Y):
                for tx in range(TILES_X):
                    tile = frame[ty * 8:(ty + 1) * 8, tx * 8:(tx + 1) * 8]
                    key = tile.tobytes()
                    if key not in tile_dict:
                        idx = alloc_index()
                        tile_dict[key] = idx
                        tile_list.append((idx, tile))

    max_index = max(idx for idx, _ in tile_list) if tile_list else 0
    print(f"Total unique tiles: {len(tile_list)}, max index: {max_index}")
    assert max_index <= 255, f"Tile index overflow: {max_index} > 255"

    # Generate CHR tile data (2bpp planar format) with tiles at their assigned indices.
    # Reserved indices are left as zeros (game tiles will be copied at runtime).
    chr_bin = bytearray(4096)  # 256 tiles × 16 bytes, initialized to zero
    for idx, tile in tile_list:
        tile_2bit = tile & 3
        chr_bytes = tile_to_chr(tile_2bit)
        chr_bin[idx * 16:(idx + 1) * 16] = chr_bytes

    # Sprite pattern table: copy from sprite_chr_combined.bin second half
    # so game sprites still work when CHR bank 2 is active
    sprite_chr_path = os.path.join(SCRIPT_DIR, "sprite_chr_combined.bin")
    with open(sprite_chr_path, "rb") as f:
        sprite_chr_data = f.read()
    sprite_pattern_table = sprite_chr_data[4096:8192]  # Second 4KB = sprite tiles
    chr_bin.extend(sprite_pattern_table)

    assert len(chr_bin) == 8192, f"CHR bank should be 8KB, got {len(chr_bin)}"

    chr_path = os.path.join(OUTPUT_DIR, "level4_chr.bin")
    with open(chr_path, "wb") as f:
        f.write(chr_bin)
    print(f"Wrote {chr_path} ({len(chr_bin)} bytes, {len(tile_list)} unique BG tiles)")

    # Process nametable frames using tile dictionary
    frames_a = process_image(img_a, tile_dict)
    frames_b = process_image(img_b, tile_dict)

    # Extract palette
    pal_rgb = img_a.getpalette()
    nes_palette = []
    for i in range(16):
        r, g, b = pal_rgb[i * 3], pal_rgb[i * 3 + 1], pal_rgb[i * 3 + 2]
        nes_palette.append(rgb_to_nes_index(r, g, b))

    # Generate C header
    h_path = os.path.join(OUTPUT_DIR, "level4_anim_data.h")
    with open(h_path, "w") as f:
        f.write("// Auto-generated by convert_level_anim.py\n")
        f.write("// Level 4 animation data for NES Celeste\n")
        f.write("// Uses dedicated CHR bank 2 with custom tiles\n")
        f.write("#ifndef LEVEL4_ANIM_DATA_H\n")
        f.write("#define LEVEL4_ANIM_DATA_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define LEVEL4_ANIM_FRAME_COUNT {FRAME_COUNT}\n\n")

        # Palette in bank 4
        f.write("__attribute__((section(\".prg_rom_4\")))\n")
        f.write("static const unsigned char level4_anim_bg_palette[16] = {\n")
        for pal in range(4):
            colors = [f"0x{nes_palette[pal * 4 + c]:02X}" for c in range(4)]
            f.write(f"    {', '.join(colors)},  // Palette {pal}\n")
        f.write("};\n\n")

        # Animation BG tile CHR data — full range from tile 0 to max_index.
        # Reserved indices (collapse tiles etc.) are zero-filled here;
        # the runtime copies game tile patterns into those slots.
        chr_tile_count = max_index + 1
        f.write(f"#define LEVEL4_ANIM_CHR_TILE_COUNT {chr_tile_count}\n\n")
        f.write("__attribute__((section(\".prg_rom_4\")))\n")
        f.write(f"static const unsigned char level4_anim_chr_tiles[{chr_tile_count * 16}] = {{\n")
        # Build index -> tile mapping
        idx_to_tile = {idx: tile for idx, tile in tile_list}
        for i in range(chr_tile_count):
            if i in idx_to_tile:
                tile_2bit = idx_to_tile[i] & 3
                chr_bytes = tile_to_chr(tile_2bit)
            else:
                chr_bytes = bytes(16)  # Reserved or unused — zero fill
            f.write("    " + ", ".join(f"0x{b:02X}" for b in chr_bytes) + ",\n")
        f.write("};\n\n")

        bank = 4
        section_attr = f'__attribute__((section(".prg_rom_{bank}")))'

        for variant_idx, (variant_name, frames) in enumerate([("a", frames_a), ("b", frames_b)]):
            nt0_0, nt2_0, attr0_nt0, attr0_nt2 = frames[0]

            rle_nt0 = rle_encode(nt0_0)
            rle_nt0.append(0xFF)
            f.write(f"{section_attr}\nstatic const unsigned char level4_anim_nt0_rle_{variant_name}[] = {{\n")
            for i in range(0, len(rle_nt0), 16):
                chunk = rle_nt0[i:i + 16]
                f.write(f"    {', '.join(f'0x{x:02X}' for x in chunk)},\n")
            f.write("};\n\n")

            rle_nt2 = rle_encode(nt2_0)
            rle_nt2.append(0xFF)
            f.write(f"{section_attr}\nstatic const unsigned char level4_anim_nt2_rle_{variant_name}[] = {{\n")
            for i in range(0, len(rle_nt2), 16):
                chunk = rle_nt2[i:i + 16]
                f.write(f"    {', '.join(f'0x{x:02X}' for x in chunk)},\n")
            f.write("};\n\n")

            f.write(f"{section_attr}\nstatic const unsigned char level4_anim_attr0_nt0_{variant_name}[64] = {{\n")
            for i in range(0, 64, 8):
                f.write(f"    {', '.join(f'0x{x:02X}' for x in attr0_nt0[i:i+8])},\n")
            f.write("};\n\n")

            f.write(f"{section_attr}\nstatic const unsigned char level4_anim_attr0_nt2_{variant_name}[64] = {{\n")
            for i in range(0, 64, 8):
                f.write(f"    {', '.join(f'0x{x:02X}' for x in attr0_nt2[i:i+8])},\n")
            f.write("};\n\n")

            delta_data_all = []
            delta_offsets = []
            max_changes = 0
            all_deltas_for_codegen = []  # List of (changes, cur_attr0, next_attr0) per delta
            for frame_idx in range(FRAME_COUNT):
                next_idx = (frame_idx + 1) % FRAME_COUNT
                cur_nt0, cur_nt2, cur_attr0, _ = frames[frame_idx]
                next_nt0, next_nt2, next_attr0, next_attr2 = frames[next_idx]
                delta_offsets.append(len(delta_data_all))
                changes = []
                for pos in range(960):
                    if pos not in COLLAPSE_TILE_POSITIONS and cur_nt0[pos] != next_nt0[pos]:
                        changes.append((pos, next_nt0[pos]))
                for pos in range(960):
                    if (pos + 960) not in COLLAPSE_TILE_POSITIONS and cur_nt2[pos] != next_nt2[pos]:
                        changes.append((pos + 960, next_nt2[pos]))
                max_changes = max(max_changes, len(changes))
                count = len(changes)
                # Delta format: [count_lo, count_hi, ppu_hi, ppu_lo, tile, ...]
                delta_data_all.append(count & 0xFF)
                delta_data_all.append((count >> 8) & 0xFF)
                for pos, tile in changes:
                    if pos < 960:
                        ppu_addr = 0x2000 + pos
                    else:
                        ppu_addr = 0x2800 + (pos - 960)
                    delta_data_all.append((ppu_addr >> 8) & 0xFF)
                    delta_data_all.append(ppu_addr & 0xFF)
                    delta_data_all.append(tile)
                delta_data_all.extend(next_attr0)
                delta_data_all.extend(next_attr2)
                all_deltas_for_codegen.append((changes, list(cur_attr0), list(next_attr0)))

            # Save for inline code generation
            if variant_name == "a":
                codegen_a = all_deltas_for_codegen
            else:
                codegen_b = all_deltas_for_codegen

            print(f"  Variant {variant_name.upper()}: max {max_changes} changes, delta {len(delta_data_all)} bytes")

            f.write(f"{section_attr}\nstatic const uint16_t level4_anim_delta_offsets_{variant_name}[LEVEL4_ANIM_FRAME_COUNT] = {{\n")
            for i in range(0, FRAME_COUNT, 6):
                f.write(f"    {', '.join(str(x) for x in delta_offsets[i:i+6])},\n")
            f.write("};\n\n")

            f.write(f"{section_attr}\nstatic const unsigned char level4_anim_delta_{variant_name}[] = {{\n")
            for i in range(0, len(delta_data_all), 16):
                chunk = delta_data_all[i:i + 16]
                f.write(f"    {', '.join(f'0x{x:02X}' for x in chunk)},\n")
            f.write("};\n\n")

        f.write("#endif // LEVEL4_ANIM_DATA_H\n")
    print(f"Wrote {h_path}")

    # Generate inline flush code — one function per (delta, chunk) pair.
    # No if/switch chains — avoids LTO pulling switch tables into wrong bank's rodata.
    # Optimizations: consecutive PPU runs merged; attr deltas only.
    # Variant A in bank 8, variant B in bank 9.
    WRITES_PER_CHUNK = 30

    flush_path = os.path.join(SCRIPT_DIR, "..", "src", "port", "nes_level4_flush.c")
    with open(flush_path, "w") as f:
        f.write("// Auto-generated by convert_level_anim.py\n")
        f.write("// Inline PPU writes for level 4 animation deltas.\n")
        f.write("// One function per (delta, chunk) pair — no branching.\n")
        f.write("// Consecutive PPU addresses merged into runs; attr deltas only.\n")
        f.write("#include <stdint.h>\n\n")
        f.write("#define PPU_ADDR (*(volatile uint8_t *)0x2006)\n")
        f.write("#define PPU_DATA (*(volatile uint8_t *)0x2007)\n\n")
        f.write("typedef void (*flush_fn_t)(void);\n\n")

        def pos_to_ppu_addr(pos):
            """Convert nametable position to PPU address."""
            if pos < 960:
                return 0x2000 + pos
            else:
                return 0x2800 + (pos - 960)

        for variant_idx, (variant_name, all_deltas) in enumerate([("a", codegen_a), ("b", codegen_b)]):
            bank = 8 + variant_idx
            section_text = f'__attribute__((section(".prg_rom_{bank}.text"), noinline))'
            section_data = f'__attribute__((section(".prg_rom_{bank}")))'

            # Generate one function per (delta, chunk) pair
            func_names = []
            chunk_counts = []
            offsets = []

            for d_idx, (changes, cur_attr, next_attr) in enumerate(all_deltas):
                num_chunks = (len(changes) + WRITES_PER_CHUNK - 1) // WRITES_PER_CHUNK
                if num_chunks == 0:
                    num_chunks = 1
                chunk_counts.append(num_chunks)
                offsets.append(len(func_names))

                for c_idx in range(num_chunks):
                    fname = f"lf_{variant_name}_d{d_idx}_c{c_idx}"
                    func_names.append(fname)
                    start = c_idx * WRITES_PER_CHUNK
                    end = min(start + WRITES_PER_CHUNK, len(changes))
                    chunk_changes = changes[start:end]

                    # Merge consecutive PPU addresses into runs
                    runs = []
                    if chunk_changes:
                        run_start_pos = chunk_changes[0][0]
                        run_tiles = [chunk_changes[0][1]]
                        for k in range(1, len(chunk_changes)):
                            pos, tile = chunk_changes[k]
                            prev_pos = chunk_changes[k-1][0]
                            # Consecutive if PPU addresses are sequential
                            if pos_to_ppu_addr(pos) == pos_to_ppu_addr(prev_pos) + 1:
                                run_tiles.append(tile)
                            else:
                                runs.append((run_start_pos, run_tiles))
                                run_start_pos = pos
                                run_tiles = [tile]
                        runs.append((run_start_pos, run_tiles))

                    f.write(f"{section_text}\n")
                    f.write(f"static void {fname}(void) {{\n")
                    for run_pos, run_tiles in runs:
                        ppu_addr = pos_to_ppu_addr(run_pos)
                        f.write(f"    PPU_ADDR=0x{(ppu_addr>>8)&0xFF:02X};PPU_ADDR=0x{ppu_addr&0xFF:02X};")
                        for tile in run_tiles:
                            f.write(f"PPU_DATA=0x{tile:02X};")
                        f.write("\n")
                    f.write(f"}}\n\n")

            # Function pointer table (flat)
            f.write(f"// Flush function pointer table — variant {variant_name.upper()} ({len(func_names)} entries)\n")
            f.write(f"{section_data}\n")
            f.write(f"const flush_fn_t la_flush_table_{variant_name}[] = {{\n")
            for fname in func_names:
                f.write(f"    {fname},\n")
            f.write(f"}};\n\n")

            # Offset array (start index per delta)
            f.write(f"{section_data}\n")
            f.write(f"const uint16_t la_flush_offsets_{variant_name}[{len(offsets)}] = {{\n")
            f.write(f"    {', '.join(str(x) for x in offsets)},\n")
            f.write(f"}};\n\n")

            # Chunk counts per delta
            f.write(f"{section_data}\n")
            f.write(f"const uint8_t la_chunk_counts_{variant_name}[{len(chunk_counts)}] = {{\n")
            f.write(f"    {', '.join(str(x) for x in chunk_counts)},\n")
            f.write(f"}};\n\n")

            # Attr functions — only write changed bytes (delta attrs)
            attr_func_names = []
            for d_idx, (changes, cur_attr, next_attr) in enumerate(all_deltas):
                fname = f"la_{variant_name}_d{d_idx}"
                attr_func_names.append(fname)
                f.write(f"{section_text}\n")
                f.write(f"static void {fname}(void) {{\n")
                attr_changes = [(i, next_attr[i]) for i in range(64) if cur_attr[i] != next_attr[i]]
                if attr_changes:
                    attr_runs = []
                    run_start = attr_changes[0][0]
                    run_vals = [attr_changes[0][1]]
                    for k in range(1, len(attr_changes)):
                        idx, val = attr_changes[k]
                        if idx == attr_changes[k-1][0] + 1:
                            run_vals.append(val)
                        else:
                            attr_runs.append((run_start, run_vals))
                            run_start = idx
                            run_vals = [val]
                    attr_runs.append((run_start, run_vals))
                    for attr_pos, attr_vals in attr_runs:
                        ppu_addr = 0x23C0 + attr_pos
                        f.write(f"    PPU_ADDR=0x{(ppu_addr>>8)&0xFF:02X};PPU_ADDR=0x{ppu_addr&0xFF:02X};")
                        for v in attr_vals:
                            f.write(f"PPU_DATA=0x{v:02X};")
                        f.write("\n")
                f.write(f"}}\n\n")

            # Attr function pointer table
            f.write(f"{section_data}\n")
            f.write(f"const flush_fn_t la_attr_table_{variant_name}[] = {{\n")
            for fname in attr_func_names:
                f.write(f"    {fname},\n")
            f.write(f"}};\n\n")

    print(f"Wrote {flush_path}")

if __name__ == "__main__":
    main()

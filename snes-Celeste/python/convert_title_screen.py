#!/usr/bin/env python
"""
Convert title screen PNGs (nametableA.png, nametableB.png) to NES data.

Outputs:
  - title_chr.bin          : CHR tile data (2bpp planar) for CHR-ROM bank 1
  - title_screen_data.h    : C header with RLE nametables, attributes, palette
"""

import os
import sys
import numpy as np
from PIL import Image

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TITLE_DIR = os.path.join(SCRIPT_DIR, "overrides", "nes", "title")
OUTPUT_DIR = SCRIPT_DIR

# NES master palette (2C02) - 64 entries, RGB values
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
    """Find the closest NES palette color index for an RGB value."""
    if r == 0 and g == 0 and b == 0:
        return 0x0F  # Use $0F for black
    best_idx = 0x0F
    best_dist = float('inf')
    for idx, (pr, pg, pb) in enumerate(NES_PALETTE):
        if idx in (0x0D, 0x0E, 0x0F, 0x1D, 0x1E, 0x1F, 0x2D, 0x2E, 0x2F, 0x3D, 0x3E, 0x3F):
            if not (r == 0 and g == 0 and b == 0):
                continue  # skip blacks/mirrors unless we want black
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
        chr_data[row] = bp0      # Bitplane 0 (low bit)
        chr_data[row + 8] = bp1  # Bitplane 1 (high bit)
    return bytes(chr_data)

def rle_encode(data):
    """RLE encode: [count, value] pairs. count=1..128 stored as 0..127."""
    encoded = []
    i = 0
    while i < len(data):
        val = data[i]
        run = 1
        while i + run < len(data) and data[i + run] == val and run < 128:
            run += 1
        encoded.append(run - 1)  # 0 = run of 1
        encoded.append(val)
        i += run
    return encoded

def process_image(img, tile_dict, tile_list):
    """Process one nametable image. Returns list of (nametable, attributes) per frame."""
    data = np.array(img)
    frames = []
    for f in range(30):
        frame = data[:, f * 256:(f + 1) * 256]
        nametable = []
        # Track palette per 8x8 tile for attribute table
        tile_palettes = []
        for ty in range(30):
            row_pal = []
            for tx in range(32):
                tile = frame[ty * 8:(ty + 1) * 8, tx * 8:(tx + 1) * 8]
                key = tile.tobytes()
                tile_idx = tile_dict[key]
                nametable.append(tile_idx)
                # Determine palette from non-zero pixels
                nonzero = tile[tile > 0]
                if len(nonzero) > 0:
                    pal = nonzero[0] // 4
                else:
                    pal = 0
                row_pal.append(pal)
            tile_palettes.append(row_pal)

        # Build attribute table (64 bytes)
        # Each byte covers 4x4 tiles (32x32 pixels)
        # Bits: [BR BR BL BL TR TR TL TL] (each 2-bit palette index)
        attributes = []
        for ay in range(8):  # 8 attribute rows (covers 32 tile rows, but screen is 30)
            for ax in range(8):  # 8 attribute columns
                tx_base = ax * 4
                ty_base = ay * 4
                # Get palette for each 2x2 tile quadrant
                def get_pal(tx, ty):
                    if ty >= 30 or tx >= 32:
                        return 0
                    return tile_palettes[ty][tx]
                # Top-left 2x2
                tl = get_pal(tx_base, ty_base)
                # Top-right 2x2
                tr = get_pal(tx_base + 2, ty_base)
                # Bottom-left 2x2
                bl = get_pal(tx_base, ty_base + 2)
                # Bottom-right 2x2
                br = get_pal(tx_base + 2, ty_base + 2)
                attr_byte = (tl & 3) | ((tr & 3) << 2) | ((bl & 3) << 4) | ((br & 3) << 6)
                attributes.append(attr_byte)

        frames.append((nametable, attributes))
    return frames

def main():
    img_a = Image.open(os.path.join(TITLE_DIR, "nametableA.png"))
    img_b = Image.open(os.path.join(TITLE_DIR, "nametableB.png"))

    # Build combined tile dictionary from both images
    tile_dict = {}
    tile_list = []  # Each entry: 8x8 numpy array of palette indices (0-15)

    for img in [img_a, img_b]:
        data = np.array(img)
        for f in range(30):
            frame = data[:, f * 256:(f + 1) * 256]
            for ty in range(30):
                for tx in range(32):
                    tile = frame[ty * 8:(ty + 1) * 8, tx * 8:(tx + 1) * 8]
                    key = tile.tobytes()
                    if key not in tile_dict:
                        tile_dict[key] = len(tile_list)
                        tile_list.append(tile)

    print(f"Total unique tiles: {len(tile_list)}")
    assert len(tile_list) <= 256, f"Too many unique tiles: {len(tile_list)} > 256"

    # Generate CHR tile data (2bpp planar format)
    # Each tile's pixels are stored as color-within-palette (index & 3)
    chr_bin = bytearray()
    for tile in tile_list:
        tile_2bit = tile & 3  # Keep only low 2 bits (color within palette)
        chr_bin.extend(tile_to_chr(tile_2bit))

    # Pad to fill BG pattern table ($0000-$0FFF = 4096 bytes = 256 tiles)
    while len(chr_bin) < 4096:
        chr_bin.extend(b'\x00' * 16)

    # Add empty sprite pattern table ($1000-$1FFF) to make full 8KB CHR bank
    chr_bin.extend(b'\x00' * 4096)

    chr_path = os.path.join(OUTPUT_DIR, "title_chr.bin")
    with open(chr_path, "wb") as f:
        f.write(chr_bin)
    print(f"Wrote {chr_path} ({len(chr_bin)} bytes)")

    # Process nametable frames
    frames_a = process_image(img_a, tile_dict, tile_list)
    frames_b = process_image(img_b, tile_dict, tile_list)

    # Extract palette from image A (both use same palette)
    pal_rgb = img_a.getpalette()
    nes_palette = []
    for i in range(16):
        r, g, b = pal_rgb[i * 3], pal_rgb[i * 3 + 1], pal_rgb[i * 3 + 2]
        nes_idx = rgb_to_nes_index(r, g, b)
        nes_palette.append(nes_idx)
        print(f"  Palette color {i}: ({r},{g},{b}) -> NES ${nes_idx:02X}")

    # Generate C header
    h_path = os.path.join(OUTPUT_DIR, "title_screen_data.h")
    with open(h_path, "w") as f:
        f.write("// Auto-generated by convert_title_screen.py\n")
        f.write("// Title screen nametable data for NES Celeste\n")
        f.write("#ifndef TITLE_SCREEN_DATA_H\n")
        f.write("#define TITLE_SCREEN_DATA_H\n\n")
        f.write("#include <stdint.h>\n\n")

        f.write(f"#define TITLE_TILE_COUNT {len(tile_list)}\n")
        f.write("#define TITLE_FRAME_COUNT 30\n")
        f.write("#define TITLE_VARIANT_COUNT 2\n\n")

        # Bank assignments: variant A in bank 2, variant B in bank 3
        bank_map = {"a": 2, "b": 3}

        # Palette data (4 BG palettes x 4 colors) - no section attr, tiny
        f.write("// Background palettes for title screen (NES palette indices)\n")
        f.write("static const unsigned char title_bg_palette[16] = {\n")
        for pal in range(4):
            colors = [f"0x{nes_palette[pal * 4 + c]:02X}" for c in range(4)]
            f.write(f"    {', '.join(colors)},  // Palette {pal}\n")
        f.write("};\n\n")

        # For each variant:
        #   Frame 0: full RLE nametable + full attributes (loaded with PPU off at init)
        #   Frames 1-29: delta tile changes + attribute diffs (applied during vblank)
        #
        # Delta format per frame:
        #   [count_lo, count_hi]  -- number of tile changes (little-endian uint16)
        #   For each change: [pos_lo, pos_hi, tile]  -- nametable position (0-959) + new tile
        #   Then 64 bytes of attribute data (always written in full, it's small)
        #
        for variant_idx, (variant_name, frames) in enumerate([("a", frames_a), ("b", frames_b)]):
            bank = bank_map[variant_name]
            section_attr = f'__attribute__((section(".prg_rom_{bank}")))'

            # Frame 0: full RLE nametable
            nt0, attr0 = frames[0]
            rle0 = rle_encode(nt0)
            rle0.append(0xFF)  # End marker

            f.write(f"// Full RLE nametable for frame 0, variant {variant_name.upper()} (PRG bank {bank})\n")
            f.write(f"{section_attr}\n")
            f.write(f"static const unsigned char title_nt_rle_{variant_name}[] = {{\n")
            for i in range(0, len(rle0), 16):
                chunk = rle0[i:i + 16]
                f.write(f"    {', '.join(f'0x{x:02X}' for x in chunk)},\n")
            f.write(f"}};\n\n")

            # Frame 0 attributes
            f.write(f"{section_attr}\n")
            f.write(f"static const unsigned char title_attr0_{variant_name}[64] = {{\n")
            for i in range(0, 64, 8):
                chunk = attr0[i:i + 8]
                f.write(f"    {', '.join(f'0x{x:02X}' for x in chunk)},\n")
            f.write(f"}};\n\n")

            # Deltas for frames 1-29 (and frame 0 wrapping from frame 29)
            # We store 30 deltas: delta[i] transitions from frame i to frame (i+1)%30
            delta_data_all = []
            delta_offsets = []
            max_changes = 0
            all_deltas_codegen = []
            for frame_idx in range(30):
                next_idx = (frame_idx + 1) % 30
                cur_nt, cur_attr = frames[frame_idx]
                next_nt, next_attr = frames[next_idx]

                delta_offsets.append(len(delta_data_all))

                # Compute tile changes
                changes = []
                for pos in range(960):
                    if cur_nt[pos] != next_nt[pos]:
                        changes.append((pos, next_nt[pos]))

                max_changes = max(max_changes, len(changes))

                # Save for inline code generation (pos-based for run merging)
                all_deltas_codegen.append((changes, list(cur_attr), list(next_attr)))

                # Write count (little-endian uint16)
                count = len(changes)
                delta_data_all.append(count & 0xFF)
                delta_data_all.append((count >> 8) & 0xFF)

                # Write tile changes: pos_lo, pos_hi, tile
                for pos, tile in changes:
                    delta_data_all.append(pos & 0xFF)
                    delta_data_all.append((pos >> 8) & 0xFF)
                    delta_data_all.append(tile)

                # Write attribute table (always full 64 bytes)
                delta_data_all.extend(next_attr)

            if variant_name == "a":
                codegen_a = all_deltas_codegen
            else:
                codegen_b = all_deltas_codegen

            print(f"  Variant {variant_name.upper()}: max {max_changes} tile changes per delta, "
                  f"delta data {len(delta_data_all)} bytes")

            # Tile change counts per delta (for inline flush chunk calculation)
            f.write(f"static const unsigned char title_delta_counts_{variant_name}[TITLE_FRAME_COUNT] = {{\n")
            counts = [len(cg[0]) for cg in all_deltas_codegen]  # cg[0] = changes list
            for i in range(0, 30, 10):
                chunk = counts[i:i + 10]
                f.write(f"    {', '.join(str(x) for x in chunk)},\n")
            f.write(f"}};\n\n")

            f.write(f"// Delta data for variant {variant_name.upper()}: transitions between consecutive frames\n")
            f.write(f"// delta[i] transitions from frame i to frame (i+1)%%30\n")
            f.write(f"{section_attr}\n")
            f.write(f"static const uint16_t title_delta_offsets_{variant_name}[TITLE_FRAME_COUNT] = {{\n")
            for i in range(0, 30, 10):
                chunk = delta_offsets[i:i + 10]
                f.write(f"    {', '.join(str(x) for x in chunk)},\n")
            f.write(f"}};\n\n")

            f.write(f"{section_attr}\n")
            f.write(f"static const unsigned char title_delta_{variant_name}[] = {{\n")
            for i in range(0, len(delta_data_all), 16):
                chunk = delta_data_all[i:i + 16]
                f.write(f"    {', '.join(f'0x{x:02X}' for x in chunk)},\n")
            f.write(f"}};\n\n")

        f.write("#endif // TITLE_SCREEN_DATA_H\n")

    print(f"Wrote {h_path}")

    print(f"\nSize summary:")
    print(f"  CHR tiles: {len(tile_list)} tiles, {len(tile_list) * 16} bytes")

    # Generate inline flush code for title screen — one function per (delta, chunk) pair.
    # No if/switch chains — avoids LTO pulling switch tables into wrong bank's rodata.
    # Optimizations:
    #   1. Merge consecutive PPU addresses into runs (set addr once, write multiple DATA)
    #   2. Attr functions write only changed bytes (delta attrs, not full 64 bytes)
    WRITES_PER_CHUNK = 40
    flush_path = os.path.join(OUTPUT_DIR, "..", "src", "port", "nes_title_flush.c")
    with open(flush_path, "w") as f:
        f.write("// Auto-generated by convert_title_screen.py\n")
        f.write("// Inline PPU writes for title screen animation deltas.\n")
        f.write("// One function per (delta, chunk) pair — no branching.\n")
        f.write("// Consecutive PPU addresses merged into runs; attr deltas only.\n")
        f.write("#include <stdint.h>\n\n")
        f.write("#define PPU_ADDR (*(volatile uint8_t *)0x2006)\n")
        f.write("#define PPU_DATA (*(volatile uint8_t *)0x2007)\n\n")
        f.write("typedef void (*flush_fn_t)(void);\n\n")

        for variant_idx, (variant_name, all_deltas) in enumerate([("a", codegen_a), ("b", codegen_b)]):
            bank = 10 + variant_idx
            section_text = f'__attribute__((section(".prg_rom_{bank}.text"), noinline))'
            section_data = f'__attribute__((section(".prg_rom_{bank}")))'

            # Generate one function per (delta, chunk) pair
            func_names = []  # flat list of all function names
            chunk_counts = []  # chunks per delta
            offsets = []  # start index per delta

            for d_idx, (changes, cur_attr, next_attr) in enumerate(all_deltas):
                num_chunks = (len(changes) + WRITES_PER_CHUNK - 1) // WRITES_PER_CHUNK
                if num_chunks == 0:
                    num_chunks = 1
                chunk_counts.append(num_chunks)
                offsets.append(len(func_names))

                for c_idx in range(num_chunks):
                    fname = f"tf_{variant_name}_d{d_idx}_c{c_idx}"
                    func_names.append(fname)
                    start = c_idx * WRITES_PER_CHUNK
                    end = min(start + WRITES_PER_CHUNK, len(changes))
                    chunk_changes = changes[start:end]

                    # Merge consecutive positions into runs
                    runs = []
                    if chunk_changes:
                        run_start_pos, run_start_tile = chunk_changes[0]
                        run_tiles = [run_start_tile]
                        for k in range(1, len(chunk_changes)):
                            pos, tile = chunk_changes[k]
                            if pos == chunk_changes[k-1][0] + 1:
                                run_tiles.append(tile)
                            else:
                                runs.append((run_start_pos, run_tiles))
                                run_start_pos = pos
                                run_tiles = [tile]
                        runs.append((run_start_pos, run_tiles))

                    f.write(f"{section_text}\n")
                    f.write(f"static void {fname}(void) {{\n")
                    for run_pos, run_tiles in runs:
                        ppu_addr = 0x2000 + run_pos
                        f.write(f"    PPU_ADDR=0x{(ppu_addr>>8)&0xFF:02X};PPU_ADDR=0x{ppu_addr&0xFF:02X};")
                        for tile in run_tiles:
                            f.write(f"PPU_DATA=0x{tile:02X};")
                        f.write("\n")
                    f.write(f"}}\n\n")

            # Generate function pointer table (flat)
            f.write(f"// Flush function pointer table — variant {variant_name.upper()} ({len(func_names)} entries)\n")
            f.write(f"{section_data}\n")
            f.write(f"const flush_fn_t title_flush_table_{variant_name}[] = {{\n")
            for fname in func_names:
                f.write(f"    {fname},\n")
            f.write(f"}};\n\n")

            # Generate offset array (start index per delta)
            f.write(f"{section_data}\n")
            f.write(f"const uint16_t title_flush_offsets_{variant_name}[{len(offsets)}] = {{\n")
            for i in range(0, len(offsets), 10):
                chunk = offsets[i:i + 10]
                f.write(f"    {', '.join(str(x) for x in chunk)},\n")
            f.write(f"}};\n\n")

            # Generate chunk counts per delta
            f.write(f"{section_data}\n")
            f.write(f"const uint8_t title_chunk_counts_{variant_name}[{len(chunk_counts)}] = {{\n")
            f.write(f"    {', '.join(str(x) for x in chunk_counts)},\n")
            f.write(f"}};\n\n")

            # Generate one attr function per delta — only write changed bytes
            attr_func_names = []
            for d_idx, (changes, cur_attr, next_attr) in enumerate(all_deltas):
                fname = f"ta_{variant_name}_d{d_idx}"
                attr_func_names.append(fname)
                f.write(f"{section_text}\n")
                f.write(f"static void {fname}(void) {{\n")
                # Find changed attr bytes and group consecutive ones into runs
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
            f.write(f"const flush_fn_t title_attr_table_{variant_name}[] = {{\n")
            for fname in attr_func_names:
                f.write(f"    {fname},\n")
            f.write(f"}};\n\n")

    print(f"Wrote {flush_path}")

if __name__ == "__main__":
    main()

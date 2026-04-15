#!/usr/bin/env python3
"""Generate chunked C payload from spc700/main.spc (active RAM range only)."""
from pathlib import Path

SPC_ROOT = Path(__file__).resolve().parents[2]
SPC = SPC_ROOT / "main.spc"
OUT = SPC_ROOT / "testrom" / "spc_payload.h"

raw = SPC.read_bytes()
if len(raw) < 0x10100:
    raise SystemExit(f"SPC file too small: {len(raw)} bytes")

pc = raw[0x25] | (raw[0x26] << 8)
ram = raw[0x100:0x10100]

nz = [i for i, v in enumerate(ram) if v != 0]
if not nz:
    raise SystemExit("No non-zero SPC RAM bytes found")

# Keep exact loaded program region. Force lower bound to reset vector target area.
start = min(nz[0], pc)
end = nz[-1] + 1
region = ram[start:end]

chunk_size = 256
chunk_count = (len(region) + chunk_size - 1) // chunk_size
padded = region + bytes((chunk_count * chunk_size) - len(region))

lines = []
lines.append("// Auto-generated from spc700/main.spc. Do not edit by hand.")
lines.append("#ifndef SPC_PAYLOAD_H")
lines.append("#define SPC_PAYLOAD_H")
lines.append("#include <stdint.h>")
lines.append("")
lines.append(f"static const uint16_t spc_start_pc = 0x{pc:04X};")
lines.append(f"static const uint16_t spc_load_addr = 0x{start:04X};")
lines.append(f"static const uint16_t spc_load_size = {len(region)}u;")
lines.append(f"#define SPC_CHUNK_SIZE {chunk_size}u")
lines.append(f"#define SPC_CHUNK_COUNT {chunk_count}u")
lines.append("")

for ci in range(chunk_count):
    lines.append(f"static const uint8_t spc_chunk_{ci:03d}[SPC_CHUNK_SIZE] = {{")
    chunk = padded[ci * chunk_size:(ci + 1) * chunk_size]
    for i in range(0, chunk_size, 16):
        row = ", ".join(f"0x{b:02X}" for b in chunk[i:i+16])
        lines.append(f"    {row},")
    lines.append("};")
    lines.append("")

lines.append("static const uint8_t *const spc_chunks[SPC_CHUNK_COUNT] = {")
for ci in range(chunk_count):
    lines.append(f"    spc_chunk_{ci:03d},")
lines.append("};")
lines.append("")
lines.append("#endif")
lines.append("")

OUT.write_text("\n".join(lines), encoding="ascii")
print(f"Wrote {OUT} (load ${start:04X}-${end-1:04X}, {len(region)} bytes)")

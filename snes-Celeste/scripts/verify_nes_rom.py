#!/usr/bin/env python
"""Verify NES Celeste ROM: check for bank mapping issues, BRK at function addresses."""
import sys, struct, re, os

def read_rom(path):
    with open(path, "rb") as f:
        data = f.read()
    # Parse iNES header
    assert data[:4] == b"NES\x1a", "Not an iNES ROM"
    prg_banks = data[4]
    chr_banks = data[5]
    prg_size = prg_banks * 16384
    header_size = 16
    prg_data = data[header_size:header_size + prg_size]
    return prg_data, prg_banks, chr_banks

def get_bank(prg_data, bank_num, prg_banks):
    """Get 16KB bank data. Last bank is the fixed bank."""
    offset = bank_num * 16384
    return prg_data[offset:offset + 16384]

def check_not_brk(bank_data, offset, name):
    """Check that the byte at offset is NOT BRK (0x00)."""
    if offset >= len(bank_data):
        return f"  FAIL: {name} at offset 0x{offset:04X} — out of range"
    byte = bank_data[offset]
    if byte == 0x00:
        return f"  FAIL: {name} at offset 0x{offset:04X} = BRK (0x00) — wrong bank or dead code!"
    return f"  OK:   {name} at offset 0x{offset:04X} = 0x{byte:02X}"

def parse_map_file(map_path):
    """Extract symbol addresses from the linker map file."""
    symbols = {}
    with open(map_path) as f:
        for line in f:
            # Match: "   78242    78242        0     1                 FamiToneInit"
            m = re.match(r'\s+([0-9a-f]+)\s+([0-9a-f]+)\s+\S+\s+\S+\s+(\S+)$', line)
            if m:
                lma = int(m.group(1), 16)
                vma = int(m.group(2), 16)
                name = m.group(3)
                symbols[name] = (lma, vma)
    return symbols

def main():
    rom_path = sys.argv[1] if len(sys.argv) > 1 else "build-nes/celeste-nes.nes"
    map_path = sys.argv[2] if len(sys.argv) > 2 else "build-nes/celeste-nes.map"

    if not os.path.exists(rom_path):
        print(f"ROM not found: {rom_path}")
        sys.exit(1)

    prg_data, prg_banks, chr_banks = read_rom(rom_path)
    print(f"ROM: {rom_path}")
    print(f"PRG: {prg_banks} banks ({prg_banks*16}KB), CHR: {chr_banks} banks ({chr_banks*8}KB)")
    print()

    # Check vectors (last 6 bytes of PRG)
    fixed_bank = prg_data[-16384:]
    nmi_vec = struct.unpack_from("<H", fixed_bank, 16384 - 6)[0]
    reset_vec = struct.unpack_from("<H", fixed_bank, 16384 - 4)[0]
    irq_vec = struct.unpack_from("<H", fixed_bank, 16384 - 2)[0]
    print(f"Vectors: NMI=0x{nmi_vec:04X} RESET=0x{reset_vec:04X} IRQ=0x{irq_vec:04X}")

    # Check reset vector points to valid code (should be 0xC000+)
    if reset_vec < 0xC000:
        print(f"  WARN: RESET vector 0x{reset_vec:04X} not in fixed bank!")
    reset_offset = reset_vec - 0xC000
    first_instr = fixed_bank[reset_offset]
    print(f"  First instruction at RESET: 0x{first_instr:02X} ({'SEI' if first_instr == 0x78 else 'NOT SEI!'})")
    print()

    # Parse map file for key symbols
    if os.path.exists(map_path):
        symbols = parse_map_file(map_path)

        # Check key functions are NOT BRK
        checks = {
            "FamiToneUpdate": 7,      # Should be in bank 7
            "FamiToneInit": 7,         # Should be in bank 7
            "__push_music_bank": -1,   # Should be in fixed bank
            "__pop_music_bank": -1,    # Should be in fixed bank
            "processObject": 6,        # Should be in bank 6
            "playerInit": 6,           # Should be in bank 6
        }

        print("Symbol checks (verifying no BRK at function starts):")
        all_ok = True
        for sym, expected_bank in checks.items():
            if sym not in symbols:
                print(f"  SKIP: {sym} not found in map")
                continue
            lma, vma = symbols[sym]

            if expected_bank == -1:
                # Fixed bank
                if vma < 0xC000 or vma >= 0x10000:
                    print(f"  FAIL: {sym} VMA=0x{vma:04X} — should be in fixed bank (0xC000+)")
                    all_ok = False
                    continue
                offset = vma - 0xC000
                result = check_not_brk(fixed_bank, offset, f"{sym} (fixed bank)")
            else:
                # Banked
                bank_data = get_bank(prg_data, expected_bank, prg_banks)
                offset = vma - 0x8000 if vma >= 0x8000 else lma - (expected_bank * 16384)
                if vma < 0x8000 or vma >= 0xC000:
                    # LMA-based
                    offset = lma - (expected_bank * 0x10000 + 0x8000)
                result = check_not_brk(bank_data, offset, f"{sym} (bank {expected_bank})")

            print(result)
            if "FAIL" in result:
                all_ok = False

        # Check that bank 7 has actual code (not all zeros/BRK)
        print()
        bank7 = get_bank(prg_data, 7, prg_banks)
        nonzero = sum(1 for b in bank7[:4096] if b != 0)
        print(f"Bank 7 content: {nonzero} non-zero bytes in first 4KB")
        if nonzero < 100:
            print("  FAIL: Bank 7 appears mostly empty — music data/code missing!")
            all_ok = False
        else:
            print(f"  OK:   Bank 7 has content ({nonzero} bytes)")

        print()
        if all_ok:
            print("=== ALL CHECKS PASSED ===")
        else:
            print("=== SOME CHECKS FAILED ===")
            sys.exit(1)
    else:
        print(f"Map file not found: {map_path}, skipping symbol checks")

if __name__ == "__main__":
    main()

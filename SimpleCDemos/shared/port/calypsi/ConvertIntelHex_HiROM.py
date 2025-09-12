import sys
import os
from intelhex import IntelHex


def main():
    # Check if correct number of arguments provided
    if len(sys.argv) != 3:
        print("Error: Invalid number of parameters")
        print("Usage: python ConvertIntelHex_HiROM.py <input_path> <output_path>")
        print(f"Received {len(sys.argv) - 1} arguments: {sys.argv[1:]}")
        sys.exit(1)

    inputPath = sys.argv[1]
    outputPath = sys.argv[2]

    # Validate input file exists
    if not os.path.exists(inputPath):
        print(f"Error: Input file does not exist: {inputPath}")
        sys.exit(1)

    # Validate input file is readable
    if not os.access(inputPath, os.R_OK):
        print(f"Error: Input file is not readable: {inputPath}")
        sys.exit(1)

    # Check if output directory exists and is writable
    output_dir = os.path.dirname(outputPath)
    if output_dir and not os.path.exists(output_dir):
        print(f"Error: Output directory does not exist: {output_dir}")
        sys.exit(1)

    if output_dir and not os.access(output_dir, os.W_OK):
        print(f"Error: Output directory is not writable: {output_dir}")
        sys.exit(1)

    print(f"Converting Intel Hex file to HiROM: {inputPath}")
    print(f"Output file: {outputPath}")

    try:
        ih = IntelHex(inputPath)
    except Exception as e:
        print(f"Error: Failed to read Intel Hex file: {e}")
        sys.exit(1)

    # Access data as a dictionary, where keys are addresses and values are bytes:
    data = ih.todict()

    # Count total bytes to convert
    total_bytes = len([addr for addr in data.keys() if addr != "start_addr"])
    print(f"Total bytes to convert: {total_bytes}")

    def pc_to_hirom(offset):
        """Convert PC address to HiROM file offset"""
        # HiROM mapping for Calypsi-generated Intel hex files:
        # Calypsi already generates HiROM addresses, so we need to convert them to file offsets
        # Bank 0: SNES 0x8000-0xFFFF -> file offset 0x8000-0xFFFF
        # Bank 1: SNES 0xC10000-0xC1FFFF -> file offset 0x10000-0x1FFFF
        # Bank 2: SNES 0xC20000-0xC2FFFF -> file offset 0x20000-0x2FFFF
        # etc.

        #if offset < 0x8000:
        #    return None  # Invalid address (should start at 0x8000)

        # Handle bank 0 (0x8000-0xFFFF) - map directly to file offset 0x8000-0xFFFF
        if offset <= 0xFFFF:
            return offset
        #SA-1 appears to do the LoROM thing for the lower addresses
        #if offset <= 0xFFFF:
        #    return offset

        # Handle HiROM banks (0xC10000+) - convert to consecutive file offsets
        if offset >= 0xC10000:
            bank = (offset >> 16) & 0xFF
            addr = offset & 0xFFFF
            # Convert HiROM bank to file offset: 0xC1 -> 0x01, 0xC2 -> 0x02, etc.
            # Bank 1 starts at file offset 0x10000, bank 2 at 0x20000, etc.
            file_bank = bank - 0xC0
            return (file_bank << 16) + addr

        # Handle other banks (0x10000+) - map to consecutive file offsets (fallback for non-HiROM addresses)
        if offset >= 0x10000:
            bank = (offset >> 16) & 0xFF
            addr = offset & 0xFFFF
            # Map to consecutive file offsets: bank 1 = 0x10000-0x1FFFF, bank 2 = 0x20000-0x2FFFF, etc.
            return (bank << 16) + (addr - 0x8000)

        return None  # Invalid HiROM address

    try:
        # Create a dictionary to map HiROM addresses to bytes
        hirom_data = {}
        max_address = 0

        # Convert all addresses to HiROM format
        converted_count = 0
        for address, byte in data.items():
            if address == "start_addr":  # Intel hex way of saying start address
                continue  # Skip because reset vector in the hex does it

            # Convert to HiROM address
            hirom_addr = pc_to_hirom(address)
            if hirom_addr is None:
                raise Exception(f"Error: Invalid HiROM address: 0x{address:06X}")

            hirom_data[hirom_addr] = byte
            max_address = max(max_address, hirom_addr)
            converted_count += 1

            # SA-1 duplication: If this is bank 0 data (0x8000-0xFFFF), also write it to HiROM location (0xC00000-0xC0FFFF)
            if address >= 0x8000 and address <= 0xFFFF:
                # Map to HiROM bank 0: 0x8000-0xFFFF -> 0x000000-0x00FFFF
                hirom_duplicate_addr = 0x000000 + (address - 0x8000)
                hirom_data[hirom_duplicate_addr] = byte
                max_address = max(max_address, hirom_duplicate_addr)
                converted_count += 1            

            # Show progress for first few conversions
            if converted_count <= 5:
                print(f"  PC 0x{address:06X} -> HiROM 0x{hirom_addr:06X} = 0x{byte:02X}")

        print(f"Converted {converted_count} bytes to HiROM format")

        # Open output file in binary write mode
        with open(outputPath, "wb") as f:
            # Write all data in order
            for addr in sorted(hirom_data.keys()):
                f.seek(addr)
                f.write(bytes([hirom_data[addr]]))

            # Add/update cartridge header for SRAM support
            # Only update specific header fields, preserving existing data
            print("Updating cartridge header for SRAM support...")

            # Calculate ROM size for header
            current_size = f.tell()
            rom_size_kb = max(256, (current_size + 1023) // 1024)  # Round up to nearest KB, minimum 256KB

            # Find the power of 2 that gives us at least this size
            rom_size_power = 0
            while (1 << rom_size_power) * 1024 < rom_size_kb * 1024:
                rom_size_power += 1

            # Cap at 12 (4MB) for safety
            rom_size_power = min(rom_size_power, 12)

            # Update specific header fields for SRAM support
            header_updates = {
                # $FFC0-$FFD4: Cartridge title (21 bytes, uppercase ASCII, padded with spaces)
                0xFFC0: b"LUA 5.4.8 SNES"[:21].ljust(21, b' '),

                # $FFD5: ROM speed and memory map mode
                # Bit 7: 0 = Slow ROM, 1 = Fast ROM
                # Bit 6: 0 = LoROM, 1 = HiROM
                # Bit 5: 0 = No FastROM, 1 = FastROM capable
                # Bit 4: 0 = No ExHiROM, 1 = ExHiROM capable
                # Bit 3-0: Reserved
                0xFFD5: bytes([0x21]),  # Fast ROM + HiROM + FastROM capable

                # $FFD6: Chipset
                0xFFD6: bytes([0x35]),  # SRAM

                # $FFD7: ROM size (1<<N kilobytes)
                0xFFD7: bytes([rom_size_power]),

                # $FFD8: RAM size (1<<N kilobytes) - 128KB (maximum)
                0xFFD8: bytes([7]),  # 2^7 = 128KB

                # $FFD9: Country - USA/Canada
                0xFFD9: bytes([1]),

                # $FFDA: Developer ID
                0xFFDA: bytes([0x33]),  # Custom ID

                # $FFDB: ROM version
                0xFFDB: bytes([0]),  # First version
            }

            # Apply header updates
            for addr, data_bytes in header_updates.items():
                f.seek(addr)
                f.write(data_bytes)

            # Preserve existing interrupt vectors from the original data
            # Only write vectors that exist in the original Intel hex data
            vector_addresses = [addr for addr in hirom_data.keys() if addr >= 0xFFE0 and addr < 0x10000]
            if vector_addresses:
                print("Preserving existing interrupt vectors...")
                for addr in sorted(vector_addresses):
                    f.seek(addr)
                    f.write(bytes([hirom_data[addr]]))

            print(f"Header updated: ROM size={rom_size_kb}KB, RAM size=128KB")

            # Pad file to nearest multiple of 64K (65536 bytes) for HiROM
            current_size = f.tell()
            if current_size > 0:
                padding_needed = (65536 - (current_size % 65536)) % 65536
                if padding_needed > 0:
                    f.write(b'\x00' * padding_needed)

        print(f"Successfully converted {inputPath} to HiROM format: {outputPath}")

    except Exception as e:
        print(f"Error: Failed to write output file: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()

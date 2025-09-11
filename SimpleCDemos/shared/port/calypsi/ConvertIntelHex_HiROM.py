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
        
        if offset < 0x8000:
            return None  # Invalid address (should start at 0x8000)
        
        # Handle bank 0 (0x8000-0xFFFF) - map directly to file offset 0x8000-0xFFFF
        if offset <= 0xFFFF:
            return offset
        
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

import sys
import os
from intelhex import IntelHex

def main():
    # Check if correct number of arguments provided
    if len(sys.argv) != 3:
        print("Error: Invalid number of parameters")
        print("Usage: python ConvertIntelHex.py <input_path> <output_path>")
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
    
    print(f"Converting Intel Hex file: {inputPath}")
    print(f"Output file: {outputPath}")
    
    try:
        ih = IntelHex(inputPath)
    except Exception as e:
        print(f"Error: Failed to read Intel Hex file: {e}")
        sys.exit(1)
    
    # Access data as a dictionary, where keys are addresses and values are bytes:
    data = ih.todict()

    def pc_to_lorom(offset):
        if offset < 0x8000 or offset >= 0x800000:
            return None  # Invalid LoROM address
        bank = (offset >> 16) & 0x7F  # Only 0x00–0x7D valid for LoROM
        addr = offset & 0x7FFF
        return (bank * 0x8000) + addr

    try:
        # Open output file in binary write mode
        with open(outputPath, "wb") as f:
            # Write each byte to file
            for address, byte in data.items():
                if address == "start_addr":  #Intel hex way of saying start address
                    continue  #Skip because reset vector in the hex does it
                # Seek to the correct address before writing
                #print(hex(pc_to_lorom(address)))
                if address < 0x8000:
                    #Should only be nothing
                    raise Exception("Error ROM data placed in invalid area")
                f.seek(pc_to_lorom(address))

                f.write(bytes([byte]))

            # Pad file to nearest multiple of 32K (32768 bytes)
            current_size = f.tell()
            padding_needed = (32768 - (current_size % 32768)) % 32768
            f.write(b'\x00' * padding_needed)
        
        print(f"Successfully converted {inputPath} to {outputPath}")
        
    except Exception as e:
        print(f"Error: Failed to write output file: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()

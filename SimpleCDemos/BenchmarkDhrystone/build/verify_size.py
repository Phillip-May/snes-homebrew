from intelhex import IntelHex

ih = IntelHex('calypsi.hex')
data = ih.todict()
addrs = [addr for addr in data.keys() if addr != 'start_addr']

print('Sample mappings:')
for addr in sorted(addrs)[:5]:
    bank = (addr >> 16) & 0xFF
    if addr >= 0x10000:
        file_offset = (bank << 16) + (addr - 0x8000)
    else:
        file_offset = addr - 0x8000
    print(f'PC 0x{addr:06X} -> File offset 0x{file_offset:06X}')

print('...')

for addr in sorted(addrs)[-5:]:
    bank = (addr >> 16) & 0xFF
    if addr >= 0x10000:
        file_offset = (bank << 16) + (addr - 0x8000)
    else:
        file_offset = addr - 0x8000
    print(f'PC 0x{addr:06X} -> File offset 0x{file_offset:06X}')

print(f'\nMax PC address: 0x{max(addrs):06X}')
max_bank = (max(addrs) >> 16) & 0xFF
max_file_offset = (max_bank << 16) + (max(addrs) - 0x8000)
print(f'Max file offset: 0x{max_file_offset:06X}')
print(f'Expected file size: {max_file_offset + 1} bytes')

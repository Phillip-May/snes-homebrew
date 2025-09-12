from intelhex import IntelHex

ih = IntelHex('calypsi.hex')
data = ih.todict()
addrs = [addr for addr in data.keys() if addr != 'start_addr']

print('Sample mappings:')
for addr in sorted(addrs)[:5]:
    bank = (addr >> 16) & 0xFF
    hirom = 0xC00000 + (bank << 16) + (addr & 0xFFFF) if addr >= 0x10000 else addr
    print(f'PC 0x{addr:06X} -> HiROM 0x{hirom:06X}')

print('...')

for addr in sorted(addrs)[-5:]:
    bank = (addr >> 16) & 0xFF
    hirom = 0xC00000 + (bank << 16) + (addr & 0xFFFF) if addr >= 0x10000 else addr
    print(f'PC 0x{addr:06X} -> HiROM 0x{hirom:06X}')

print(f'\nMax PC address: 0x{max(addrs):06X}')
max_bank = (max(addrs) >> 16) & 0xFF
max_hirom = 0xC00000 + (max_bank << 16) + (max(addrs) & 0xFFFF)
print(f'Max HiROM address: 0x{max_hirom:06X}')

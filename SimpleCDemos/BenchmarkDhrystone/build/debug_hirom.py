from intelhex import IntelHex

ih = IntelHex('calypsi.hex')
data = ih.todict()
addrs = [addr for addr in data.keys() if addr != 'start_addr']

print('Sample addresses and their HiROM mappings:')
for i, addr in enumerate(sorted(addrs)[:10]):
    if addr >= 0x10000:
        bank = (addr >> 16) & 0xFF
        hirom_addr = 0xC00000 + (bank << 16) + (addr & 0xFFFF)
    else:
        hirom_addr = addr
    print(f'  PC 0x{addr:06X} -> HiROM 0x{hirom_addr:06X}')

print(f'\nMax PC address: 0x{max(addrs):06X}')
print(f'Max HiROM address would be: 0x{0xC00000 + ((max(addrs) >> 16) & 0xFF) * 0x10000 + (max(addrs) & 0xFFFF):06X}')

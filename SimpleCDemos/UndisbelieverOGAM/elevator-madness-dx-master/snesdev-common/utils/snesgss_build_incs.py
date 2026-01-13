#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim: set fenc=utf-8 ai ts=4 sw=4 sts=4 et:

"""
Generates a `snesgss.inc` and `snesgss.inc.h` inside the export
directory of an exported project by SNESGSS.
"""

import os
import re
import sys

BANK_SIZE = 32 * 1024


def read_sounds_h(filename):
    sfx_enum = list()
    music_enum = list()

    with open(filename, 'r') as fp:
        for l in fp:
            m = re.match(r"^\s*SFX_([A-Z_]+=\d+),?$", l)
            if m:
                sfx_enum.append(m.group(1))

            m = re.match(r"^\s*MUS_([A-Z_]+=\d+),?$", l)
            if m:
                music_enum.append(m.group(1))

    return {
        'sfx_enum': sfx_enum,
        'music_enum': music_enum
    }


def get_music_bins(export_dir):
    ret = list()

    for f in os.listdir(export_dir):
        if f.startswith('music_') and f.endswith('.bin'):
            ret.append(f)

    ret.sort()
    return ret


def write_header(hdata, fp):
    def writeln(s):
        fp.write(s)
        fp.write("\n")

    writeln(".enum SnesGssMusic")

    for s in hdata['music_enum']:
        writeln("\t" + s)

    writeln(".endenum\n")

    writeln(".enum SnesGssSfx")

    for s in hdata['sfx_enum']:
        writeln("\t" + s)

    writeln(".endenum\n")


def write_include(export_dir, data, fp):
    def writeln(s):
        fp.write(s)
        fp.write("\n")

    def file_size(fn):
        return os.path.getsize(os.path.join(export_dir, fn))

    assert len(data['music_enum']) == len(data['music_bin']), "Unexpected number of music"

    writeln(".global SnesGss__Module")
    writeln(".global SnesGss__ModulePart2")
    writeln(".global SnesGss__MusicTable")
    writeln(".exportzp SnesGss__MusicTable_Count = {}\n".format(len(data['music_enum'])))

    writeln(".proc SnesGssData\n")
    writeln(".segment SNESGSS_BANK1\n")
    writeln("LABEL SnesGss__Module\n")

    if file_size("spc700.bin") > 32768:
        writeln("\t.incbin \"spc700.bin\", 0, 32768")
        writeln(".segment SNESGSS_BANK2")
        if BANK_SIZE == 32768:
            writeln("LABEL SnesGss__ModulePart2")
        writeln("\t.incbin \"spc700.bin\", 32768")
    else:
        writeln("\t.incbin \"spc700.bin\"")
        writeln("LABEL SnesGss__ModulePart2")


    writeln("\n")



    bank = int(file_size("spc700.bin") / BANK_SIZE)
    current_pos = file_size("spc700.bin") % BANK_SIZE

    current_pos += len(data['music_bin']) * 3
    if current_pos >= BANK_SIZE:
        bank += 1
        current_pos %= BANK_SIZE
        writeln(".segment SNESGSS_BANK{}\n".format(bank))

    writeln("LABEL SnesGss__MusicTable")

    for mfn in data['music_bin']:
        m = os.path.splitext(mfn)[0]
        writeln("\t.faraddr {}_data".format(m))

    writeln("\n")


    for mfn in data['music_bin']:
        current_pos += file_size(mfn)
        if current_pos >= BANK_SIZE:
            bank += 1
            current_pos %= BANK_SIZE
            writeln("\n.segment SNESGSS_BANK{}\n".format(bank))

        m = os.path.splitext(mfn)[0]
        writeln("{}_data: .incbin \"{}\"".format(m, mfn))

    writeln(".endproc\n")


def main(export_dir):
    data = read_sounds_h(os.path.join(export_dir, "sounds.h"))

    data['music_bin'] = get_music_bins(export_dir)

    with open(os.path.join(export_dir, 'snesgss.inc.h'), 'w') as fp:
        write_header(data, fp)

    with open(os.path.join(export_dir, 'snesgss.inc'), 'w') as fp:
        write_include(export_dir, data, fp)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(__doc__, file=sys.stderr)
        print("USAGE: {} <export_dir>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)
    else:
        main(sys.argv[1])



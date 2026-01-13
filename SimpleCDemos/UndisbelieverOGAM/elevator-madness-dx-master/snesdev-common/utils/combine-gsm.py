#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim: set fenc=utf-8 ai ts=4 sw=4 sts=4 et:

"""
Combines multiple .gsm files into a single combined gsm via stdout.
"""

import re
import sys
from collections import OrderedDict
import configparser

N_SONGS = 99
N_INSTRUMENTS = 99

SONG_CONFIG = (
        'Name',
        'Length',
        'LoopStart',
        'Measure',
        'Effect',
)

INSTRUMENT_CONFIG = (
		'Name',
		'EnvAR',
		'EnvDR',
		'EnvSL',
		'EnvSR',
		'Length',
		'LoopStart',
		'LoopEnd',
		'LoopEnable',
		'SourceLength',
		'SourceRate',
		'SourceVolume',
		'WavLoopStart',
		'WavLoopEnd',
		'EQLow',
		'EQMid',
		'EQHigh',
		'ResampleType',
		'DownsampleFactor',
		'RampEnable',
		'LoopUnroll',
		'SourceData',
)

DEFAULT_SONG_CONFIG = {
    'Name':         'untitled',
    'Length':       9999,
    'LoopStart':    0,
    'Measure':      4,
    'Effect':       0
}

def name_to_id(name):
    s = re.sub(r"[^a-zA-Z0-9_]+", '_', name)
    return s.upper()


def get_instruments_in_songs(songs):
    ret = set()
    ret.add(1)

    regex = re.compile(r"^\d\d\d\d..." + r"...(..)....." * 8 + r"$")

    for sid, s in songs.items():
        for line in s['Text'].splitlines():
            m = regex.match(line)
            if m:
                for mi in range(1, 8 + 1):
                    try:
                        i = int(m.group(mi))
                        ret.add(int(i))
                    except:
                        continue
            else:
                raise ValueError("Bad Regex", line)
    return ret


def replace_song_instruments(song_text, replacemap):
    regex = re.compile(r"^(\d\d\d\d..." + r"...)(..)(....." * 8 + r")$")

    lines = list()

    for l in song_text.splitlines():
        m = regex.match(l)
        sep = list(m.groups())

        for i in range(1, 8 * 2, 2):
            s = sep[i]
            if s in replacemap:
                sep[i] = replacemap[s]
            else:
                assert s == '..', "Unknown instrument - This shouldn't happen"

        lines.append("".join(sep))

    return "\n".join(lines)


def read_gsm(in_filename):
    config = configparser.RawConfigParser(allow_no_value=True)
    config.read(in_filename)

    if 'SNESGSS Module' not in config:
        raise ValueError('Not a SNESGSS module', in_filename)

    if 'EX VOL' not in config:
        raise ValueError("File is of the old format, please update the file", in_filename)


    songs = dict()
    instruments = dict()

    cdata = config['EX VOL']

    for s in range(0, N_SONGS):
        ss = "Song{}".format(s)
        if ss in config:
            song_text = "\n".join(config[ss])
            if song_text.rstrip():
                song = {
                        'Text': song_text.upper(),
                }

                for c in SONG_CONFIG:
                    ssc = ss + c
                    song[c] = cdata[ssc]

                sid = name_to_id(song['Name'])
                songs[sid] = song


    for iid in get_instruments_in_songs(songs):
        si = "Instrument{}".format(iid - 1)

        inst = dict()

        for c in INSTRUMENT_CONFIG:
            sic = si + c
            if sic in cdata:
                inst[c] = cdata[sic]

        instruments[iid] = inst


    return {
            'Songs': songs,
            'Instruments': instruments
    }



def combine_data(in_data):
    new_inst_id = dict() # Mapping of instrument data to new id

    instruments = dict()
    songs = dict()

    for source, data in in_data.items():
        replacemap = dict()

        for iid, idata in data['Instruments'].items():
            # Can't hash a dict, but can hash a string
            # ::ANNOY str does not guarantee keys are in a fixed order::
            idata_str = str(sorted(idata.items(), key=lambda t: t[0]))

            if idata_str not in new_inst_id:
                nid = len(new_inst_id) + 1
                new_inst_id[idata_str] = nid
                instruments[nid] = idata

            siid = "{:02}".format(iid)
            replacemap[siid] = "{:02}".format(new_inst_id[idata_str])

        for sname, song in data['Songs'].items():
            if sname in songs:
                raise KeyError('Song id already exists', sname, source)

            new_song = song.copy()
            new_song['Text'] = replace_song_instruments(song['Text'], replacemap)

            songs[sname] = new_song

    return {
        'Instruments': instruments,
        'Songs': songs
    }


def write_new_file(data, fp):
    def writeln(s):
        fp.write(s)
        fp.write("\n")

    if len(data['Instruments']) >= N_INSTRUMENTS:
        raise ValueError('Too many Instruments')

    if len(data['Songs']) >= N_SONGS:
        raise ValueError('Too many Songs')

    writeln('[SNESGSS Module]')
    writeln('[EX VOL]')

    for iid, idata in data['Instruments'].items():
        si = "Instrument{}".format(iid - 1)

        for c in INSTRUMENT_CONFIG:
            if c in idata:
                writeln("{}{}={}".format(si, c, idata[c]))

    writeln("\n")

    songs = data['Songs'].values()
    for sid, sdata in enumerate(songs):
        si = "Song{}".format(sid)

        for c in SONG_CONFIG:
            if c in sdata:
                writeln("{}{}={}".format(si, c, sdata[c]))

    # ::BUGFIX prevent division by zero SNESGSS Editor::
    for sid in range(len(data['Songs']), N_SONGS):
        si = "Song{}".format(sid)

        for c in SONG_CONFIG:
            writeln("{}{}={}".format(si, c, DEFAULT_SONG_CONFIG[c]))


    for sid, sdata in enumerate(songs):
        writeln("\n")
        writeln("[Song{}]".format(sid))
        writeln(sdata['Text'])

    # ::BUGFIX prevent division by zero SNESGSS Editor::
    for sid in range(len(data['Songs']), N_SONGS):
        writeln("\n[Song{}]".format(sid))

    writeln("")



def main(source_filenames):
    in_data = dict()

    for in_filename in source_filenames:
        in_data[in_filename] = read_gsm(in_filename)

    new_data = combine_data(in_data)

    write_new_file(new_data, sys.stdout)


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print(__doc__, file=sys.stderr)
        print("USAGE: {} <sources>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)
    else:
        main(sys.argv[1:])



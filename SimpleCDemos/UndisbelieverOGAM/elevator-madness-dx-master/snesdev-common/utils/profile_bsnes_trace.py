#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim: set fenc=utf-8 ai ts=4 sw=4 sts=4 et:

"""
RoutineProfiles a bsnes assembly trace.
"""

import re
import sys
import argparse
import os.path


SNES_SCANLINES = 262
SNES_HTIME     = 1374
COMMON_INTERRUPT_NAMES = (
        'VBLANK', 'VBlank', 'NmiHandler', 'NMI',
        'IrqHandler', 'IRQ',
        'CopHandler', 'COP',
        'ResetHandler', 'RESET',
        'BreakHandler', 'BREAK',
        'EmptyHandler',
)

class LineReader:
    """
    A simple line reader that supports peeking at the next line.
    """
    def __init__(self, fp):
        self.fp = fp
        self.line = None
        self.lineno = 0
        self.peek = fp.readline()

    def __iter__(self):
        return self

    def __next__(self):
        line = self.peek

        if line:
            self.line = line
            self.lineno += 1
            self.peek = self.fp.readline()
            return line
        else:
            raise StopIteration



class MemoryMap:
    def __init__(self):
        self.addresses = dict()

    def load(self, filename):
        regex = re.compile(r'^([A-Za-z0-9_\-]+)\s+([A-Za-z0-9]{6})\s+[A-Z]{1,3}(?:\s+([A-Za-z0-9_\-]+)\s+([A-Za-z0-9]{6})\s)')

        with open(filename, 'r') as fp:
            for line in fp:
                m = regex.match(line)
                if m:
                    addr = int(m.group(2), 16) & 0x7FFFFF
                    self.addresses[addr] = m.group(1)

                    if m.group(3):
                        addr = int(m.group(4), 16) & 0x7FFFFF
                        self.addresses[addr] = m.group(3)

    def nameAddressIfUnlabled(self, addr, name):
        addr = addr & 0x7FFFFF
        if addr not in self.addresses:
            self.addresses[addr] = name

    def nameForAddress(self, addr):
        maddr = addr & 0x7FFFFF
        if maddr in self.addresses:
            return self.addresses[maddr]
        else:
            return None

    def addressForName(self, name):
        for a, n in self.addresses.items():
            if n == name:
                return a
        return -1

    def nameOrAddress(self, addr):
        maddr = addr & 0x7FFFFF
        if maddr in self.addresses:
            return self.addresses[maddr]
        else:
            return '%06x' % addr



class RoutineProfile:
    def __init__(self, addr, caller, callAddress):
        self.address = addr
        self.caller = caller
        self.callAddress = callAddress

        self.count = 0
        self.time = 0
        self.totaltime = 0
        self._calls = dict() # dict of tuple (callAddress, routineAddress) -> RoutineProfile

    def sorted_calls(self):
        return sorted(self._calls.values(), key=lambda x: x.totaltime, reverse=True)

    def calls(self):
        return self._calls.values()

    def get_or_make_call(self, callAddress, routineAddr):
        test = (callAddress, routineAddr)
        if test in self._calls:
            p = self._calls[test]
        else:
            p = RoutineProfile(routineAddr, self, callAddress)
            self._calls[test] = p

        return p

    def add_call(self, call):
        self._calls[(call.callAddress, call.address)] = call



class RoutineTotal:
    def __init__(self, addr):
        self.address = addr
        self.count = 0
        self.time = 0



class Profile:
    def __init__(self, memmap):
        self.memmap = memmap
        self.name = None
        self.root = None
        self.time = 0
        self.waiTime = 0
        self._interrupts = dict() # mapping (address) -> RoutineProfile
        self.profiles = set()


    def addInterrupts(self, *interrupts):
        for i in interrupts:
            try:
                a = int(i, 16)
            except:
                a = self.memmap.addressForName(i)

            if a >= 0:
                rt = RoutineProfile(a, None, -1)
                self._interrupts[a & 0x7FFFFF] = rt
                self._interrupts[a | 0x800000] = rt


    def readTrace(self, fp):
        regex = re.compile(r'^([A-Za-z0-9]{6})\s+(\w+?)\s.+?V:\s*(\d+)\sH:\s*(\d+)')
        addr_regex = re.compile(r'^([A-Za-z0-9]{6})\s')

        profiles = set() # mapping address -> RoutineProfile
        interruptStack = list()
        reader = LineReader(fp)

        m = regex.match(reader.peek)
        if not m:
            raise ValueError("Not in a trace format")

        prevVTime = int(m.group(3))
        prevHTime = int(m.group(4))


        def time_since_last_line(m):
            nonlocal prevVTime, prevHTime

            vTime = int(m.group(3))
            hTime = int(m.group(4))

            if vTime < prevVTime:
                vTime += SNES_SCANLINES

            if prevVTime != vTime:
                hTime += SNES_HTIME * (vTime - prevVTime)

            time = hTime - prevHTime

            prevVTime = int(m.group(3))
            prevHTime = int(m.group(4))

            return time


        # First line may be VBlank, check first.
        instAddr = int(m.group(1), 16)
        if instAddr not in self._interrupts:
            current = RoutineProfile(instAddr, None, -1)
            profiles.add(current)
        else:
            # First line Interrupt;
            current = None

        # Mark first instruction as entry.
        self.memmap.nameAddressIfUnlabled(instAddr, "__TRACE_START__")

        for line in reader:
            m = regex.match(line)
            if m:
                inst = m.group(2).lower()
                instAddr = int(m.group(1), 16)

                # Check to see if an interrupt occurred
                if instAddr in self._interrupts:
                    interruptStack.append(current)
                    current = self._interrupts[instAddr]
                    current.count += 1

                current.time += time_since_last_line(m)

                if inst == 'jsr' or inst == 'jsl':
                    # Determine the location of the routine by address of next line
                    nm = addr_regex.match(reader.peek)
                    routineAddr = int(nm.group(1), 16)

                    p = current.get_or_make_call(instAddr, routineAddr)
                    p.count += 1
                    current = p

                elif inst == 'rts' or inst == 'rtl':
                    # Return from previous instruction

                    if current.caller:
                        current = current.caller
                    else:
                        # Returning to a routine that hasn't been traced before.
                        # Determine the location of the routine by address of next line
                        nm = addr_regex.match(reader.peek)
                        routineAddr = int(nm.group(1), 16)

                        prev = current
                        current = RoutineProfile(routineAddr, None, -1)
                        prev.caller = current
                        current.add_call(prev)

                        profiles.discard(prev)
                        profiles.add(current)

                elif inst == 'wai':
                    # Add time waiting for interrupt to current function, not the interrupt itself
                    nm = regex.match(reader.peek)
                    if nm:
                        t = time_since_last_line(nm)
                        current.time += t
                        self.waiTime += t

                elif inst == 'rti':
                    # Return from interrupt
                    try:
                        current = interruptStack.pop()
                    except IndexError:
                        current = None

                    if current == None:
                        nm = addr_regex.match(reader.peek)
                        returnAddr = int(nm.group(1), 16)

                        current = RoutineProfile(returnAddr, None, -0xFF)
                        profiles.add(current)


        # Only add interrupts that were called
        for i in self._interrupts.values():
            if i.count > 0:
                self.profiles.add(i)

        self.profiles.update(profiles)
        self._calculate_total_time()
        self._generate_routines_list()


    def _calculate_total_time(self):
        def recurse(proutine):
            total = proutine.time
            for p in proutine.calls():
                total += recurse(p)

            proutine.totaltime = total

            return total

        self.totaltime = 0
        for p in self.profiles:
            self.totaltime += recurse(p)


    def _generate_routines_list(self):
        routines = dict()

        def recurse(proutine):
            addr = proutine.address
            if addr not in routines:
                routines[addr] = RoutineTotal(addr)

            routines[addr].count += proutine.count
            routines[addr].time += proutine.time

            for p in proutine.calls():
                recurse(p)

        for p in self.profiles:
            recurse(p)

        self.routines = sorted(routines.values(), key=lambda x: x.time, reverse=True)



def write_routine_times(fp, profile):
    totaltime = profile.totaltime
    memmap = profile.memmap

    fp.write('<h2>Routines:</h2>')
    fp.write('<table>')
    fp.write('<thead><th colspan="2">Routine</th><th>Count</th><th>Cycles</th><th>Percentage</th></thead>')
    fp.write('<tbody>')
    for r in profile.routines:
        fp.write("<tr>")

        aname = memmap.nameForAddress(r.address)
        if aname:
            fp.write("<td><tt>%06x</tt></td><td>%s</td>" % (
                r.address,
                aname
            ))
        elif r.address >= 0:
            fp.write("<td><tt>%06x</tt></td><td>&nbsp;</td>" % (
                r.address
            ))
        else:
            fp.write("<td><tt>NULL</tt></td><td>&nbsp;</td>")

        fp.write("<td>%d</td><td>%d</td><td>%f%%</td></tr>" % (
            r.count,
            r.time,
            r.time / totaltime * 100.0,
        ))
    fp.write('</tbody>')
    fp.write('</table>')



def write_profile_calls(fp, profile):
    memmap = profile.memmap

    def recurse(proutine, parent_totaltime):
        fp.write("<li>")

        aname = memmap.nameForAddress(proutine.address)
        if aname:
            fp.write("<tt>%06x</tt> %s" % (
                proutine.address,
                aname
            ))
        elif proutine.address >= 0:
            fp.write("<tt>%06x</tt>" % (
                proutine.address
            ))
        else:
            fp.write("<tt>NULL</tt>")

        fp.write(": Called at <tt>%06x</tt> %d times, %i cycles (%f%% parent, %f%% total)</li>" % (
                proutine.callAddress,
                proutine.count,
                proutine.totaltime,
                proutine.totaltime / parent_totaltime * 100.0,
                proutine.totaltime / profile.totaltime * 100.0,
        ))

        calls = proutine.sorted_calls()
        if calls:
            fp.write('<ul>')
            for p in calls:
                recurse(p, proutine.totaltime)
            fp.write('</ul>')


    for p in profile.profiles:
        fp.write('<h2>%s:</h2>' % memmap.nameOrAddress(p.address))
        fp.write('<ul>')

        recurse(p, p.totaltime)

        fp.write('</ul>')



def write_html(fp, profile):
    fp.write("<html><title>%s tracelog profile</title><body>" % profile.name)
    fp.write("<h1>%s</h1>" % profile.name)

    p =  profile.waiTime / profile.totaltime * 100

    fp.write("<div>%d Cycles Traced, %d WAI cycles (%f%% WAI, %f%% non WAI).</div>" % (
        profile.totaltime,
        profile.waiTime,
        p,
        100 - p,
    ))

    write_routine_times(fp, profile)
    write_profile_calls(fp, profile)

    fp.write('</body></html>')



def process_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('-m', '--memlog',
            type=str,
            help='Memlog file',
    )
    parser.add_argument('-i', '--interrupt', metavar='INTERRUPT',
            dest='interrupts', type=str,  action='append',
            help="""Addresses / Names of interrupt. Can be used multiple times.
If memlog is supplied and this argument is not used, then common names
will be guessed from the memlog file."""
    )
    parser.add_argument('logfile',
            type=str,
            help='The SNES assembly trace file (`-` is stdin)',
    )
    parser.add_argument('htmlfile',
            type=str,
            help='The HTML output file (`-` is stdout)',
    )

    return parser.parse_args()



def main():
    args = process_args()

    memmap = MemoryMap()
    profile = Profile(memmap)

    if args.memlog:
        memmap.load(args.memlog)

    if args.interrupts:
        profile.addInterrupts(*args.interrupts)
    else:
        profile.addInterrupts(*COMMON_INTERRUPT_NAMES)

    if args.logfile == '-':
        profile.name = 'SNES'
        profile.readTrace(sys.stdin)
    else:
        with open(args.logfile, 'r') as fp:
            profile.name = os.path.splitext(os.path.basename(args.logfile))[0]
            profile.readTrace(fp)

    if args.htmlfile == '-':
        write_html(sys.stdout, profile)
    else:
        with open(args.htmlfile, 'w') as fp:
            write_html(fp, profile)


if __name__ == '__main__':
    main()



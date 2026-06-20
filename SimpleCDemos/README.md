# SimpleCDemos

Small SNES homebrew demos written in C, each buildable with several 65816 C
toolchains through one shared build system (`shared/build/shared-config.mk`).

## Prerequisites

- **Windows** (the port wrappers use `.bat` / PowerShell).
- **GNU make** with a Unix `sh` on `PATH` — make runs recipes through `sh.exe`,
  so forward slashes are used throughout. Git Bash / MSYS2 both provide this.
- **Python 3** for the Calypsi hex converter and the jcc816 / tcc816 wrappers.
  The Calypsi path also needs the `intelhex` package: `pip install intelhex`.
- At least one of the C toolchains below.

## Toolchains

Set each toolchain's location in `shared/build/toolchains.mk` (see setup below).
That file is the single place the build looks for tool paths. Versions are what
these demos were last built against on this machine.

| Compiler   | `make COMPILER=` | Version here     | Variable          | Where to get it |
|------------|------------------|------------------|-------------------|-----------------|
| cc65       | `cc65`           | 2.19             | `CC65_HOME`       | https://cc65.github.io/ |
| Calypsi    | `calypsi`        | 65816 5.12       | `CALYPSI_HOME`    | https://www.calypsi.cc/ |
| LLVM-MOS   | `llvm-mos`       | rolling          | `LLVM_MOS_HOME`   | https://github.com/llvm-mos/llvm-mos-sdk |
| WDC816CC   | `wdc816cc`       | WDCTools         | `WDC_HOME`        | https://wdc65xx.com/ |
| vbcc       | `vbcc65816`      | 65816 backend    | `VBCC_HOME`       | http://www.compilers.de/vbcc.html |
| PVSnesLib  | `tcc816`         | -                | `PVSNESLIB_HOME`  | https://github.com/alekmaul/pvsneslib |
| JCC816     | `jcc816`         | -                | `JCC816_HOME`     | JCC816 project |

Each demo (except DemoLua548) declares all of these in `SUPPORTED_COMPILERS` and
`build-all.ps1` attempts every one, so which combinations actually succeed is
reported by build-all rather than assumed. Whether a given toolchain is present
depends on your machine / `toolchains.mk`.

## Setup

Copy the template and edit the paths for your machine:

    cp shared/build/toolchains.mk.example shared/build/toolchains.mk

`toolchains.mk` is git-ignored, so edit it freely. It is the one file to point
the build at your installed toolchains — only the entries for compilers you use
need to be correct. Use **forward slashes** and no trailing slash.

## Building a demo

From inside a demo directory:

    make COMPILER=calypsi      # or cc65, llvm-mos, ...
    make calypsi               # alias: clean + build with calypsi
    make clean

The output ROM is `build/mainBankZero_<compiler>.smc`. Each demo declares a
`SUPPORTED_COMPILERS` set; requesting one outside it errors unless you pass
`ALLOW_UNSUPPORTED=1`. See each demo's `README.md` for its purpose and status.

## Building everything

    .\build-all.ps1               # build every demo x its SUPPORTED_COMPILERS
    .\build-all.ps1 -FailOnError  # also exit non-zero if any build failed

It prints a per-demo pass/fail line, a summary, and an explicit list of every
failure (with a log path). Failures are never hidden.

## Demos

Builds that currently succeed here (`build-all.ps1`: 22/43 pass):

| Demo               | Builds with                                  |
|--------------------|----------------------------------------------|
| CGRamTest          | cc65, calypsi, llvm-mos, vbcc65816, tcc816   |
| BenchmarkDhrystone | cc65, calypsi, llvm-mos, vbcc65816, tcc816   |
| BenchmarkWhestone  | calypsi, vbcc65816                           |
| CEnvTests          | calypsi, llvm-mos, vbcc65816                 |
| Mode1Sprite        | calypsi, vbcc65816, tcc816                   |
| Mode1HDMA          | calypsi, vbcc65816, tcc816                   |
| DemoLua548         | calypsi                                      |

calypsi builds every demo; vbcc65816 builds every standard demo. The remaining
failures are left surfaced (not hidden) and are toolchain limits, not config
bugs:

- **cc65** — no floating point, single 32 KB ROM bank: fails Whetstone and the
  graphical demos (RODATA overflow).
- **llvm-mos** — graphical demos overflow the fixed bank.
- **tcc816** — missing soft-float runtime (Whetstone) and fails CEnvTests.
- **wdc816cc** — its C front end rejects the shared headers' declaration syntax.
- **jcc816** — its toolchain wrapper does not run on this machine.

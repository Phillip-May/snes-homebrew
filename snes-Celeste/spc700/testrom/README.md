# SPC700 Test ROM

This ROM validates SNES <-> SPC700 communication using `spc700/main.spc`.

## What it tests
- SPC IPL boot handshake (`AA/BB`)
- SPC RAM program transfer from `main.spc` active region (`$0200-$3796`)
- SPC execute jump to embedded start PC
- Runtime heartbeat (`APUIO3 == 0x99`)
- Command path:
  - Play music patterns (`CMD_PLAY_MUSIC`, 0x06)
  - Play SFX ids (`CMD_PLAY_SFX_ID`, 0x04)
  - Stop all (`CMD_STOP_ALL`, 0x03)

## Controls
- `Up/Down`: choose song from `{00,0A,14,1E,28}`
- `A`: play selected song
- `Left/Right`: choose SFX id (00-3F)
- `B`: play selected SFX
- `X` (or `Y` alias): stop all
- `Start`: run automated self-test sequence

## Debug status byte
- CPU `APUIO2` (`$2142`) is used as result status:
  - `0x00` idle
  - `0x55` pass
  - `0xEE` fail

## Build
1. Build SPC payload source image:
```powershell
cd ..\
bash build.sh
```
2. Generate payload header:
```powershell
cd testrom
C:\Python37\python.exe tools\gen_spc_payload.py
```
3. Build ROM:
```powershell
make COMPILER=llvm-mos
make COMPILER=vbcc65816
```

Output ROMs:
- `build/mainBankZero_llvm-mos.smc`
- `build/mainBankZero_vbcc65816.smc`

## Mesen automation
```powershell
powershell -ExecutionPolicy Bypass -File scripts/run_mesen_spc_test.ps1 -Compiler llvm-mos -MesenPath "C:\Program Files\Mesen2\Mesen.exe"
```

The Lua script presses `Start`, waits for self-test completion, and exits with pass/fail based on `$2142`.

local spcRam = emu.memType.spcRam
local spcIO = emu.memType.spcMemory
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/timer2.log", "w")
local frame = 0
emu.addEventCallback(function()
    frame = frame + 1
    -- Read timer output counters
    local t0out = emu.read(0xFD, spcIO)
    local t0div = emu.read(0xFA, spcRam)
    local control = emu.read(0xF1, spcRam)
    if frame <= 10 or frame % 30 == 0 then
        f:write(string.format("f=%d t0out=%d t0div=%d control=0x%02X\n", frame, t0out, t0div, control))
        f:flush()
    end
    if frame >= 120 then f:close(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

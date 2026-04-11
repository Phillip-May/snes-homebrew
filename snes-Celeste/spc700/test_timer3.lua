local spcRam = emu.memType.spcRam
local spcDsp = emu.memType.spcDspRegisters
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/timer3.log", "w")
local frame = 0
emu.addEventCallback(function()
    frame = frame + 1
    local ticks = emu.read(0xFDF0, spcRam) + emu.read(0xFDF1, spcRam) * 256
    local v0p = emu.read(0x02, spcDsp) + (emu.read(0x03, spcDsp) & 0x3F) * 256
    if frame <= 10 or frame % 30 == 0 then
        f:write(string.format("f=%d ticks=%d v0=%d\n", frame, ticks, v0p))
        f:flush()
    end
    if frame >= 180 then f:close(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

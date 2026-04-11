local spcRam = emu.memType.spcRam
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/ch.log", "w")
local frame = 0
emu.addEventCallback(function()
    frame = frame + 1
    local ticks = emu.read(0xFDF0, spcRam) + emu.read(0xFDF1, spcRam) * 256
    local active = emu.read(0x2E24, spcRam)
    local remain = emu.read(0x2E22, spcRam) + emu.read(0x2E23, spcRam) * 256
    local note = emu.read(0x2E21, spcRam)
    if frame <= 20 or frame % 15 == 0 then
        f:write(string.format("f=%d ticks=%d active=%d remain=%d note=%d\n", frame, ticks, active, remain, note))
        f:flush()
    end
    if frame >= 120 then f:close(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

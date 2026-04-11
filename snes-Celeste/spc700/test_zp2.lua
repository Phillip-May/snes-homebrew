local spcRam = emu.memType.spcRam
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/zp2.log", "w")
local frame = 0
emu.addEventCallback(function()
    frame = frame + 1
    local note = emu.read(0x21, spcRam)       -- offset 1
    local remain = emu.read(0x22, spcRam) + emu.read(0x23, spcRam) * 256  -- offset 2-3
    local notes_played = emu.read(0x2F, spcRam) -- offset 15 (0x20+15=0x2F)
    if frame <= 20 or frame == 30 or frame == 60 then
        f:write(string.format("f=%d note=%d remain=%d played=%d\n", frame, note, remain, notes_played))
        f:flush()
    end
    if frame >= 90 then f:close(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

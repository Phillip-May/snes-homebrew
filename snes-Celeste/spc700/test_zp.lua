local spcRam = emu.memType.spcRam
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/zp.log", "w")
local frame = 0
emu.addEventCallback(function()
    frame = frame + 1
    -- channels[0] at ZP 0x20, size 17 bytes
    -- current_note at offset 1 = 0x21
    -- ticks_remaining at offset 2-3 = 0x22-0x23
    local note = emu.read(0x21, spcRam)
    local remain = emu.read(0x22, spcRam) + emu.read(0x23, spcRam) * 256
    local active = emu.read(0x24, spcRam)
    local notes_played = emu.read(0x2E, spcRam)  -- offset 14
    if frame <= 20 or frame == 30 or frame == 60 then
        f:write(string.format("f=%d note=%d remain=%d active=%d played=%d\n", frame, note, remain, active, notes_played))
        f:flush()
    end
    if frame >= 90 then f:close(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

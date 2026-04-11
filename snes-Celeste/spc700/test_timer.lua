local spcRam = emu.memType.spcRam
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/timer_test.log", "w")
local frame = 0
emu.addEventCallback(function()
    frame = frame + 1
    local lo = emu.read(0xFDF0, spcRam)
    local hi = emu.read(0xFDF1, spcRam)
    local ticks = lo + hi * 256
    -- Also read channels[0].ticks_remaining at 0x42-0x43
    local tr_lo = emu.read(0x42, spcRam)
    local tr_hi = emu.read(0x43, spcRam)
    local tr = tr_lo + tr_hi * 256
    -- channels[0].active at 0x44
    local active = emu.read(0x44, spcRam)
    if frame <= 10 or frame % 30 == 0 then
        f:write(string.format("f=%d ticks=%d ch0_remaining=%d ch0_active=%d\n", frame, ticks, tr, active))
        f:flush()
    end
    if frame >= 180 then
        f:close()
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

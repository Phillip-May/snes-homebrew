local spcRam = emu.memType.spcRam
local spcDsp = emu.memType.spcDspRegisters
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/debug.log", "w")
local frame = 0
emu.addEventCallback(function()
    frame = frame + 1
    local v0_pl = emu.read(0x02, spcDsp)
    local v0_ph = emu.read(0x03, spcDsp)
    local v0_pitch = v0_pl + (v0_ph & 0x3F) * 256
    local ticks = emu.read(0xFDF0, spcRam) + emu.read(0xFDF1, spcRam) * 256
    local ch0_remain = emu.read(0xFDEA, spcRam) + emu.read(0xFDEB, spcRam) * 256
    local ch0_active = emu.read(0xFDEC, spcRam)
    local dbg_note = emu.read(0xFDE3, spcRam)
    local dbg_pitch = emu.read(0xFDE0, spcRam) + emu.read(0xFDE1, spcRam) * 256
    if frame <= 25 or frame % 30 == 0 then
        f:write(string.format("f=%d dsp=%d dbg=%d note=%d ticks=%d remain=%d active=%d\n",
            frame, v0_pitch, dbg_pitch, dbg_note, ticks, ch0_remain, ch0_active))
        f:flush()
    end
    if frame >= 180 then f:close(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

local spcRam = emu.memType.spcRam
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/dump.log", "w")
local frame = 0
emu.addEventCallback(function()
    frame = frame + 1
    if frame == 2 or frame == 16 or frame == 17 or frame == 18 or frame == 50 then
        local bytes = ""
        for i = 0, 20 do
            bytes = bytes .. string.format("%02X ", emu.read(0x20 + i, spcRam))
        end
        f:write(string.format("f=%d: %s\n", frame, bytes))
        f:flush()
    end
    if frame >= 60 then f:close(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

local spcDsp = emu.memType.spcDspRegisters
local frame = 0
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/direct.log", "w")

emu.addEventCallback(function()
    frame = frame + 1
    
    -- At frame 30, write a different pitch to voice 0
    if frame == 30 then
        emu.write(0x02, 0xCB, spcDsp)  -- PL for 715
        emu.write(0x03, 0x02, spcDsp)  -- PH for 715
        emu.log("Wrote pitch 715 to v0")
    end
    
    -- At frame 60, write another pitch
    if frame == 60 then
        emu.write(0x02, 0xFA, spcDsp)  -- PL for 1274
        emu.write(0x03, 0x04, spcDsp)  -- PH for 1274
        emu.log("Wrote pitch 1274 to v0")
    end
    
    local pl = emu.read(0x02, spcDsp)
    local ph = emu.read(0x03, spcDsp)
    local pitch = pl + (ph & 0x3F) * 256
    f:write(string.format("f=%d pitch=%d\n", frame, pitch))
    
    if frame >= 90 then f:close(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

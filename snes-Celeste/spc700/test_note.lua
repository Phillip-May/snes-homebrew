-- Test harness: capture voice 1 (SFX 10) ENVX across note transitions.
-- Music auto-plays pattern 0: voice 0=SFX21, voice 1=SFX10
-- SFX 10 alternates between phaser notes and rests — perfect for click detection.
local spcDsp = emu.memType.spcDspRegisters
local f = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/test_note.csv", "w")
f:write("frame,v0_pitch,v0_env,v0_vol,v1_pitch,v1_env,v1_vol\n")
local frame = 0
local MAX = 300

emu.addEventCallback(function()
    frame = frame + 1
    local row = tostring(frame)
    for v = 0, 1 do
        local base = v * 0x10
        local pl = emu.read(base + 0x02, spcDsp)
        local ph = emu.read(base + 0x03, spcDsp)
        local pitch = pl + (ph & 0x3F) * 256
        local envx = emu.read(base + 0x08, spcDsp)
        local voll = emu.read(base + 0x00, spcDsp)
        row = row .. string.format(",%d,%d,%d", pitch, envx, voll)
    end
    f:write(row .. "\n")
    if frame >= MAX then
        f:close()
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

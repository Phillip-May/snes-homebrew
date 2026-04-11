-- Capture each voice individually by muting the others.
-- Runs 4 passes: solo voice 1, solo voice 2, solo voice 3, solo voice 0 (from pattern 1+).
-- Outputs separate CSV files for each voice.

local spcDsp = emu.memType.spcDspRegisters
local spcRam = emu.memType.spcRam
local spcIO  = emu.memType.spcMemory

local FRAMES_PER_PASS = 600  -- 10s per voice
local frame = 0
local pass_num = 0  -- 0-3 for voices 1,2,3,0
local voice_order = {1, 2, 3}  -- voices to solo (voice 0 only active from pattern 1)
local outFile = nil
local pass_voice = 0

local function start_pass()
    if pass_num >= #voice_order then
        emu.log("[CAPTURE] All passes complete")
        if emu.stop then emu.stop(0) end
        return
    end
    pass_voice = voice_order[pass_num + 1]
    local path = string.format("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/voice%d_capture.csv", pass_voice)
    outFile = io.open(path, "w")
    outFile:write("frame,pitch,env,vol,srcn\n")
    frame = 0

    -- Mute all voices except the solo voice by setting volume to 0
    for v = 0, 4 do
        local base = v * 0x10
        if v == pass_voice then
            -- Restore volume (will be set by sequencer)
        else
            -- Mute by writing 0 volume
            emu.write(base + 0x00, 0, spcDsp)  -- VOLL = 0
            emu.write(base + 0x01, 0, spcDsp)  -- VOLR = 0
        end
    end
    emu.log(string.format("[CAPTURE] Pass %d: solo voice %d (%d frames)", pass_num + 1, pass_voice, FRAMES_PER_PASS))
end

start_pass()

emu.addEventCallback(function()
    if pass_num >= #voice_order then return end

    frame = frame + 1

    -- Keep other voices muted (sequencer may set their volume)
    for v = 0, 4 do
        if v ~= pass_voice then
            local base = v * 0x10
            emu.write(base + 0x00, 0, spcDsp)
            emu.write(base + 0x01, 0, spcDsp)
        end
    end

    -- Capture solo voice state
    local base = pass_voice * 0x10
    local pl = emu.read(base + 0x02, spcDsp)
    local ph = emu.read(base + 0x03, spcDsp)
    local pitch = pl + (ph & 0x3F) * 256
    local envx = emu.read(base + 0x08, spcDsp)
    local voll = emu.read(base + 0x00, spcDsp)
    local srcn = emu.read(base + 0x04, spcDsp)

    outFile:write(string.format("%d,%d,%d,%d,%d\n", frame, pitch, envx, voll, srcn))

    if frame >= FRAMES_PER_PASS then
        outFile:close()
        pass_num = pass_num + 1
        start_pass()
    end
end, emu.eventType.endFrame)

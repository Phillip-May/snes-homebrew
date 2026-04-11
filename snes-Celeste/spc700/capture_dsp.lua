-- Comprehensive DSP capture: music pattern 0 auto-plays on boot.
-- Capture 600 frames (10s) of DSP register state.

local spcDsp = emu.memType.spcDspRegisters
local spcRam = emu.memType.spcRam
local outFile = io.open("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/dsp_capture.csv", "w")
local frame = 0
local MAX_FRAMES = 600  -- 10s for quick comparison

outFile:write("frame,v0_pitch,v0_env,v0_vol,v0_srcn,v1_pitch,v1_env,v1_vol,v1_srcn,v2_pitch,v2_env,v2_vol,v2_srcn,v3_pitch,v3_env,v3_vol,v3_srcn\n")

-- Debug buffer reader
local DBG_BUF = 0xFE00
local DBG_SIZE = 0xFE
local DBG_WPTR = 0xFEFE
local lastRead = 0

local function readDebugBuffer()
    local wptr = emu.read(DBG_WPTR, spcRam) + emu.read(DBG_WPTR + 1, spcRam) * 256
    if wptr < lastRead then lastRead = 0 end
    if wptr == lastRead then return end
    local buf = ""
    while lastRead ~= wptr and lastRead < DBG_SIZE do
        local ch = emu.read(DBG_BUF + lastRead, spcRam)
        lastRead = lastRead + 1
        if ch == 0x0A or ch == 0x00 then
            if #buf > 0 then emu.log("[SPC] " .. buf) end
            buf = ""
        elseif ch >= 0x20 and ch <= 0x7E then
            buf = buf .. string.char(ch)
        end
    end
    if #buf > 0 then emu.log("[SPC] " .. buf) end
end

emu.addEventCallback(function()
    readDebugBuffer()
    frame = frame + 1
    if frame > MAX_FRAMES then
        outFile:close()
        emu.log("[CAPTURE] Done - " .. frame .. " frames")
        if emu.stop then emu.stop(0) end
        return
    end

    local row = tostring(frame)
    for v = 0, 3 do
        local base = v * 0x10
        local pl = emu.read(base + 0x02, spcDsp)
        local ph = emu.read(base + 0x03, spcDsp)
        local pitch = pl + (ph & 0x3F) * 256
        local envx = emu.read(base + 0x08, spcDsp)
        local voll = emu.read(base + 0x00, spcDsp)
        local srcn = emu.read(base + 0x04, spcDsp)
        row = row .. string.format(",%d,%d,%d,%d", pitch, envx, voll, srcn)
    end
    outFile:write(row .. "\n")
end, emu.eventType.endFrame)

emu.log("[CAPTURE] Starting " .. MAX_FRAMES .. " frame capture")

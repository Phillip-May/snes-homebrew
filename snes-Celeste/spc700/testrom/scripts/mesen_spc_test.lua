-- Mesen 2 Lua script: auto-drive SPC700 test ROM.
-- Expects ROM to expose pass/fail via CPU APUIO2 ($2142):
--   0x55 = pass, 0xEE = fail

local cpu = emu.memType.cpu
local maxFrames = 3600
local frame = 0
local started = false
local result = 0
local done = false
local autoClose = (os.getenv("SPC_TEST_AUTOCLOSE") == "1")

local function tap_start()
    if emu.setInput then
        emu.setInput(1, { Start = true })
    end
end

local function release_input()
    if emu.setInput then
        emu.setInput(1, {})
    end
end

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1

    if frame == 120 then
        tap_start()
    elseif frame == 122 then
        release_input()
        started = true
    end

    result = emu.read(0x2142, cpu)
    if result == 0x55 then
        emu.log("[SPC TEST] PASS at frame " .. frame)
        done = true
        if autoClose and emu.stop then emu.stop(0) end
        return
    elseif result == 0xEE then
        emu.log("[SPC TEST] FAIL at frame " .. frame)
        done = true
        if autoClose and emu.stop then emu.stop(1) end
        return
    end

    if frame >= maxFrames then
        emu.log("[SPC TEST] TIMEOUT")
        done = true
        if autoClose and emu.stop then emu.stop(2) end
    end
end, emu.eventType.endFrame)

emu.log("[SPC TEST] Waiting for ROM init; will press START at frame 120")

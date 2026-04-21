local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_direct_touch_stable_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_MONUMENT_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MONUMENT_TICK = sym.s_monumentTextTick
local ADDR_MONUMENT_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MONUMENT_CHAR = sym.GLOBAL_MonumentCurLineCharCount
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local frame = 0
local done = false
local room12Frame = nil
local touchStartFrame = nil
local lastFc = nil
local sameFc = 0
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a)
    local lo = rd8(a)
    local hi = rd8(a + 1)
    return lo + hi * 256
end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a, v)
    wr8(a, v)
    wr8(a + 1, math.floor(v / 256))
end
local function clearPad()
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
end
local function log(line)
    lines[#lines + 1] = line
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

log("START")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    clearPad()

    if frame >= 30 and frame < 90 then
        inputState.a = true
    elseif room12Frame == nil and frame >= 140 and frame <= 420 then
        if (frame % 20) == 0 then
            inputState.r = true
        end
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)
    local px = rd16(PLAYER_X)
    local py = rd16(PLAYER_Y)
    local mon = rd8(ADDR_MONUMENT_DISPLAYED)
    local tick = rd8(ADDR_MONUMENT_TICK)
    local line = rd8(ADDR_MONUMENT_LINE)
    local char = rd8(ADDR_MONUMENT_CHAR)

    if room == 12 and room12Frame == nil then
        room12Frame = frame
        log(string.format("ROOM12 frame=%d fc=%d px=%d py=%d", frame, fc, px, py))
    end

    if room12Frame ~= nil and (frame - room12Frame) >= 120 then
        if touchStartFrame == nil then
            touchStartFrame = frame
            log(string.format("TOUCH_START frame=%d fc=%d", frame, fc))
        end
        wr16(PLAYER_X, 60)
        wr16(PLAYER_Y, 40)
        px = rd16(PLAYER_X)
        py = rd16(PLAYER_Y)
    end

    if lastFc == fc then
        sameFc = sameFc + 1
    else
        sameFc = 0
    end
    lastFc = fc

    if frame == 1 or (frame % 30) == 0 or room12Frame ~= nil then
        log(string.format("f=%d fc=%d room=%d same=%d px=%d py=%d mon=%d tick=%d line=%d char=%d",
            frame, fc, room, sameFc, px, py, mon, tick, line, char))
    end

    if room12Frame ~= nil and room ~= 12 then
        log(string.format("LEFT_ROOM frame=%d fc=%d room=%d mon=%d tick=%d line=%d char=%d",
            frame, fc, room, mon, tick, line, char))
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if frame > 300 and sameFc >= 20 then
        log(string.format("HANG frame=%d fc=%d room=%d mon=%d tick=%d line=%d char=%d",
            frame, fc, room, mon, tick, line, char))
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if touchStartFrame ~= nil and (frame - touchStartFrame) >= 180 then
        log("DONE")
        done = true
        if emu.stop then emu.stop(0) end
    elseif frame >= 1800 then
        log("TIMEOUT")
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

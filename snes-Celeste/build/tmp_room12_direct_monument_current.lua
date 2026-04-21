local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_direct_monument_current_result.txt"
local outPng = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_direct_monument_current.png"
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
local lastFc = nil
local sameFc = 0
local shotTaken = false
local shotTakenRestart = false
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
    end

    local fc = rd16(ADDR_FRAME)
    local room = rd16(ADDR_ROOM)
    local px = rd16(PLAYER_X)
    local py = rd16(PLAYER_Y)
    local mon = rd8(ADDR_MONUMENT_DISPLAYED)
    local tick = rd8(ADDR_MONUMENT_TICK)
    local line = rd8(ADDR_MONUMENT_LINE)
    local char = rd8(ADDR_MONUMENT_CHAR)

    if room == 12 then
        local age = frame - 110
        if age < 90 then
            emu.write(PLAYER_X, 60, cpu)
            emu.write(PLAYER_X + 1, 0, cpu)
            emu.write(PLAYER_Y, 40, cpu)
            emu.write(PLAYER_Y + 1, 0, cpu)
        elseif age < 180 then
            emu.write(PLAYER_X, 100, cpu)
            emu.write(PLAYER_X + 1, 0, cpu)
            emu.write(PLAYER_Y, 80, cpu)
            emu.write(PLAYER_Y + 1, 0, cpu)
        elseif age < 240 then
            emu.write(PLAYER_X, 60, cpu)
            emu.write(PLAYER_X + 1, 0, cpu)
            emu.write(PLAYER_Y, 40, cpu)
            emu.write(PLAYER_Y + 1, 0, cpu)
        else
            emu.write(PLAYER_X, 100, cpu)
            emu.write(PLAYER_X + 1, 0, cpu)
            emu.write(PLAYER_Y, 80, cpu)
            emu.write(PLAYER_Y + 1, 0, cpu)
        end
        px = rd16(PLAYER_X)
        py = rd16(PLAYER_Y)
    end

    if lastFc == fc then
        sameFc = sameFc + 1
    else
        sameFc = 0
    end
    lastFc = fc

    if frame == 1 or frame % 5 == 0 or room == 12 then
        log(string.format("f=%d fc=%d room=%d same=%d px=%d py=%d mon=%d tick=%d line=%d char=%d",
            frame, fc, room, sameFc, px, py, mon, tick, line, char))
    end

    if room == 12 and not shotTaken and frame >= 250 then
        local png = emu.takeScreenshot()
        local f = io.open(outPng, "wb")
        if f then
            f:write(png)
            f:close()
        end
        shotTaken = true
        log("SHOT_ON")
    end

    if room == 12 and not shotTakenRestart and frame >= 380 then
        local png = emu.takeScreenshot()
        local f = io.open(outPng:gsub("%.png$", "_restart.png"), "wb")
        if f then
            f:write(png)
            f:close()
        end
        shotTakenRestart = true
        log("SHOT_RESTART")
    end

    if frame > 240 and sameFc >= 20 then
        log("HANG")
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if frame >= 450 then
        log("DONE")
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

local cpu = emu.memType.cpu
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_idle_gate_result.txt"
local outBox = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_idle_gate_box.png"
local outChar1 = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_idle_gate_char1.png"
local outChar5 = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_idle_gate_char5.png"

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_MONUMENT_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MONUMENT_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MONUMENT_CHAR = sym.GLOBAL_MonumentCurLineCharCount

local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local frame = 0
local done = false
local shots = {
    box = false,
    char1 = false,
    char5 = false
}
local logLines = {}
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

local function setPad(tbl)
    clearPad()
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
    end
end

local function flushLog()
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(logLines, "\n") .. "\n")
        f:close()
    end
end

local function log(msg)
    logLines[#logLines + 1] = msg
    flushLog()
end

local function savePng(path)
    local png = emu.takeScreenshot()
    local f = io.open(path, "wb")
    if f then
        f:write(png)
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

    if frame >= 120 and frame < 140 then
        setPad({ a = true })
    end

    local roomLo = rd8(ADDR_ROOM)
    local roomHi = rd8(ADDR_ROOM + 1)
    local fcLo = rd8(ADDR_FRAME)
    local fcHi = rd8(ADDR_FRAME + 1)
    local mon = rd8(ADDR_MONUMENT_DISPLAYED)
    local line = rd8(ADDR_MONUMENT_LINE)
    local char = rd8(ADDR_MONUMENT_CHAR)

    if frame >= 260 then
        wr16(PLAYER_X, 100)
        wr16(PLAYER_Y, 80)
    end

    if frame == 1 or (frame % 20) == 0 or mon ~= 0 then
        log(string.format("f=%d roomLo=%d roomHi=%d fcLo=%d fcHi=%d px=%d py=%d mon=%d line=%d char=%d",
            frame, roomLo, roomHi, fcLo, fcHi, rd16(PLAYER_X), rd16(PLAYER_Y), mon, line, char))
    end

    if mon ~= 0 and line == 0 and char == 0 and not shots.box then
        savePng(outBox)
        shots.box = true
        log(string.format("SHOT_BOX frame=%d", frame))
    end

    if mon ~= 0 and line == 0 and char == 1 and not shots.char1 then
        savePng(outChar1)
        shots.char1 = true
        log(string.format("SHOT_CHAR1 frame=%d", frame))
    end

    if mon ~= 0 and line == 0 and char == 5 and not shots.char5 then
        savePng(outChar5)
        shots.char5 = true
        log(string.format("SHOT_CHAR5 frame=%d", frame))
    end

    if shots.box and shots.char1 and shots.char5 then
        log("DONE")
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 1800 then
        log("TIMEOUT")
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

local cpu = emu.memType.cpu
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_direct_boot_capture_v2.txt"
local outBox = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_direct_boot_capture_v2_box.png"
local outChar1 = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_direct_boot_capture_v2_char1.png"
local outChar5 = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_direct_boot_capture_v2_char5.png"

local ADDR_PLAYER = sym.GLOBAL_PlayerData
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
local lines = {}

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
local function log(msg)
    lines[#lines + 1] = msg
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function savePng(path)
    local png = emu.takeScreenshot()
    local f = io.open(path, "wb")
    if f then
        f:write(png)
        f:close()
    end
end

log("START")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1

    if frame >= 30 then
        wr16(PLAYER_X, 60)
        wr16(PLAYER_Y, 40)
    end

    local mon = rd8(ADDR_MONUMENT_DISPLAYED)
    local line = rd8(ADDR_MONUMENT_LINE)
    local char = rd8(ADDR_MONUMENT_CHAR)

    if frame == 1 or (frame % 10) == 0 or mon ~= 0 then
        log(string.format("f=%d px=%d py=%d mon=%d line=%d char=%d",
            frame, rd16(PLAYER_X), rd16(PLAYER_Y), mon, line, char))
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

    if frame >= 1200 then
        log("TIMEOUT")
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

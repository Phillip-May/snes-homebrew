local cpu = emu.memType.cpu
local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_overlay_drive_result.txt"
local outPng1 = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_overlay_drive_on.png"
local outPng2 = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_overlay_drive_restart.png"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_MON = sym.GLOBAL_MonumentTextDisplayed
local ADDR_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_CHAR = sym.GLOBAL_MonumentCurLineCharCount

local frame = 0
local done = false
local shot1 = false
local shot2 = false
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
local function clearPad()
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
end
local function log(line)
    lines[#lines + 1] = line
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function writePng(path)
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

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            inputState.start = true
        elseif phase < 20 then
            inputState.a = true
        else
            inputState.b = true
        end
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)

    if room == 1 and fc >= 220 then
        local age = fc - 220
        if age < 12 then
            wr8(ADDR_MON, 1)
            wr8(ADDR_LINE, 0)
            wr8(ADDR_CHAR, age + 1)
        elseif age < 20 then
            wr8(ADDR_MON, 0)
            wr8(ADDR_LINE, 0)
            wr8(ADDR_CHAR, 0)
        elseif age < 32 then
            wr8(ADDR_MON, 1)
            wr8(ADDR_LINE, 0)
            wr8(ADDR_CHAR, age - 19)
        else
            wr8(ADDR_MON, 0)
            wr8(ADDR_LINE, 0)
            wr8(ADDR_CHAR, 0)
        end

        if not shot1 and age == 8 then
            writePng(outPng1)
            shot1 = true
            log(string.format("SHOT1 frame=%d fc=%d room=%d", frame, fc, room))
        end
        if not shot2 and age == 28 then
            writePng(outPng2)
            shot2 = true
            log(string.format("SHOT2 frame=%d fc=%d room=%d", frame, fc, room))
        end
    end

    if frame == 1 or (frame % 20) == 0 or shot1 or shot2 then
        log(string.format("f=%d fc=%d room=%d mon=%d line=%d char=%d",
            frame, fc, room, rd8(ADDR_MON), rd8(ADDR_LINE), rd8(ADDR_CHAR)))
    end

    if shot1 and shot2 and room == 1 and fc >= 260 then
        log("DONE")
        done = true
        if emu.stop then emu.stop(0) end
    end

    if frame >= 1200 then
        log("TIMEOUT")
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

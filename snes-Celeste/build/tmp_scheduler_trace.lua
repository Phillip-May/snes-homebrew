local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/scheduler_trace_result.txt"

local frame = 0
local lines = {}
local room1Frame = nil

local inputState = {
    up=false, down=false, left=false, right=false,
    select=false, start=false, a=false, b=false,
    x=false, y=false, l=false, r=false
}

local function rd8(a) return emu.read(a, cpu) or 0 end
local function rd16(a) return rd8(a) + rd8(a + 1) * 256 end
local function rs16(a)
    local v = rd16(a)
    if v >= 0x8000 then return v - 0x10000 end
    return v
end
local function log(s)
    lines[#lines + 1] = s
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

emu.addEventCallback(function()
    frame = frame + 1

    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
    if frame >= 240 and frame <= 360 then
        inputState.a = true
    end

    local fc = rd16(0x7E1489)
    local effects = rd16(0x7E148B)
    local playerX = rs16(0x7E148F)
    local playerY = rs16(0x7E1491)
    local room = rd16(0x7E0286)
    local inTitle = rd8(0x7E0228)
    local lastGameplay = rd16(0x7E0229)
    local stage = rd8(0x7E0215)
    local titleTimer = rd8(0x7E0230)
    local titleMode = rd8(0x7E11B3)
    local timerF = rd8(0x7E021F)
    local timerS = rd8(0x7E0220)
    local inputHi = rd8(0x7E0214)
    local inputLo = rd8(0x7E0284)
    local bg4x = rd16(0x7E0277)

    if room == 1 and not room1Frame then
        room1Frame = frame
        log("ROOM1")
    end

    if frame == 1 or frame % 30 == 0 or (room1Frame and frame <= room1Frame + 120) then
        log(string.format(
            "f=%d fc=%u room=%u stage=%02X inTitle=%u titleMode=%u titleTimer=%u lastGF=%u eff=%u timer=%u:%u inHi=%02X inLo=%02X px=%d py=%d bg4x=%u",
            frame, fc, room, stage, inTitle, titleMode, titleTimer, lastGameplay, effects,
            timerS, timerF, inputHi, inputLo, playerX, playerY, bg4x))
    end

    if frame >= 760 then
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_to_room12_monument_brief_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local frame = 0
local done = false
local roomHitFrame = nil
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
    table.insert(lines, line)
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

if emu.setInput then
    emu.addEventCallback(function()
        pcall(function() emu.setInput(inputState, 0) end)
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
    if room == 12 then
        if roomHitFrame == nil then
            roomHitFrame = frame
            log(string.format("ROOM12 frame=%d fc=%d", frame, fc))
        end
        if (frame - roomHitFrame) < 45 then
            wr16(PLAYER_X, 60)
            wr16(PLAYER_Y, 40)
        end
    end

    if lastFc == fc then
        sameFc = sameFc + 1
    else
        sameFc = 0
    end
    lastFc = fc

    if frame == 1 or (frame % 30) == 0 or room == 12 then
        log(string.format("f=%d fc=%d room=%d same=%d px=%d py=%d", frame, fc, room, sameFc, rd16(PLAYER_X), rd16(PLAYER_Y)))
    end

    if sameFc >= 20 then
        log("HANG")
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if roomHitFrame ~= nil and (frame - roomHitFrame) >= 600 then
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

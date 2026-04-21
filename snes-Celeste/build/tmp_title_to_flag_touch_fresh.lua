local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_to_flag_touch_fresh_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local TARGET_ROOM = 31
local TARGET_X = 56
local TARGET_Y = 48

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local frame = 0
local done = false
local lastFc = nil
local sameFc = 0
local roomHitFrame = nil
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
end
local function flush()
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
flush()

emu.addEventCallback(function()
    if done then return end

    frame = frame + 1
    clearPad()

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)

    if frame >= 120 and frame <= 420 then
        inputState.b = true
    elseif room >= 1 and room < TARGET_ROOM and (frame % 12) == 0 then
        inputState.r = true
    end

    if room == TARGET_ROOM then
        if roomHitFrame == nil then
            roomHitFrame = frame
            log(string.format("ROOM%d frame=%d fc=%d", TARGET_ROOM, frame, fc))
        end
        wr16(PLAYER_X, TARGET_X)
        wr16(PLAYER_Y, TARGET_Y)
    end

    if lastFc == fc then sameFc = sameFc + 1 else sameFc = 0 end
    lastFc = fc

    if frame == 1 or (frame % 30) == 0 or room == TARGET_ROOM then
        log(string.format("f=%d fc=%d room=%d same=%d px=%d py=%d", frame, fc, room, sameFc, rd16(PLAYER_X), rd16(PLAYER_Y)))
        flush()
    end

    if roomHitFrame ~= nil and room ~= TARGET_ROOM then
        log(string.format("ROOM_CORRUPT frame=%d room=%d fc=%d px=%d py=%d", frame, room, fc, rd16(PLAYER_X), rd16(PLAYER_Y)))
        flush()
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if roomHitFrame ~= nil and (room > 32 or rd16(PLAYER_X) > 255 or rd16(PLAYER_Y) > 255) then
        log(string.format("STATE_CORRUPT frame=%d room=%d fc=%d px=%d py=%d", frame, room, fc, rd16(PLAYER_X), rd16(PLAYER_Y)))
        flush()
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if roomHitFrame ~= nil and sameFc >= 20 then
        log("HANG")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if roomHitFrame ~= nil and (frame - roomHitFrame) >= 600 then
        log("DONE")
        flush()
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 2400 then
        log("TIMEOUT")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/normal_room12_monument_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_MONUMENT_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MONUMENT_TICK = sym.s_monumentTextTick
local ADDR_MONUMENT_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MONUMENT_CHAR = sym.GLOBAL_MonumentCurLineCharCount
local ADDR_OBJLIST = 0x7E11B1
local OBJ_SIZE = 25
local OBJ_LIST_SIZE = 29
local OBJ_TYPE_OFFSET = 9
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2
local TARGET_ROOM = 12

local frame = 0
local done = false
local lastFc = nil
local sameFc = 0
local roomHitFrame = nil
local monumentObjX = nil
local monumentObjY = nil
local monumentStarted = false
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
local function findMonument()
    local i
    for i = 1, (OBJ_LIST_SIZE - 1) do
        local base = ADDR_OBJLIST + (i * OBJ_SIZE)
        if rd8(base + OBJ_TYPE_OFFSET) == 70 then
            return rd16(base + 0), rd16(base + 2), i
        end
    end
    return nil, nil, nil
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
    if done then
        return
    end

    frame = frame + 1
    clearPad()

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)

    if frame >= 120 and frame <= 420 then
        inputState.b = true
    elseif room >= 1 and room < TARGET_ROOM then
        local phase = (frame - 420) % 20
        if frame >= 420 and phase < 6 then
            inputState.r = true
        end
    end

    if room == TARGET_ROOM then
        if roomHitFrame == nil then
            roomHitFrame = frame
            log(string.format("ROOM%d frame=%d fc=%d", TARGET_ROOM, frame, fc))
        end
        if monumentObjX == nil then
            local objX, objY, objIndex = findMonument()
            if objX ~= nil then
                monumentObjX = objX
                monumentObjY = objY
                log(string.format("MONUMENT_OBJ index=%d x=%d y=%d", objIndex, objX, objY))
            end
        end
        if monumentObjX ~= nil then
            wr16(PLAYER_X, monumentObjX + 4)
            wr16(PLAYER_Y, monumentObjY + 8)
        end
    end

    if lastFc == fc then
        sameFc = sameFc + 1
    else
        sameFc = 0
    end
    lastFc = fc

    local monumentDisplayed = rd8(ADDR_MONUMENT_DISPLAYED)
    local monumentTick = rd8(ADDR_MONUMENT_TICK)
    local monumentLine = rd8(ADDR_MONUMENT_LINE)
    local monumentChar = rd8(ADDR_MONUMENT_CHAR)

    if room == TARGET_ROOM and not monumentStarted and (monumentDisplayed ~= 0 or monumentLine ~= 0 or monumentChar ~= 0) then
        monumentStarted = true
        log(string.format("MONUMENT_START frame=%d fc=%d line=%d char=%d tick=%d", frame, fc, monumentLine, monumentChar, monumentTick))
    end

    if frame == 1 or (frame % 30) == 0 or room == TARGET_ROOM then
        log(string.format("f=%d fc=%d room=%d same=%d px=%d py=%d mon=%d tick=%d line=%d char=%d objx=%s objy=%s",
            frame, fc, room, sameFc, rd16(PLAYER_X), rd16(PLAYER_Y),
            monumentDisplayed, monumentTick, monumentLine, monumentChar,
            tostring(monumentObjX), tostring(monumentObjY)))
        flush()
    end

    if sameFc >= 20 then
        log("HANG")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if roomHitFrame ~= nil and room ~= TARGET_ROOM then
        log(string.format("ROOM_CORRUPT frame=%d room=%d fc=%d", frame, room, fc))
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

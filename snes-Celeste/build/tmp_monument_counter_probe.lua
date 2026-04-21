local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_counter_probe_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_DEBUG_LOGIC = 0x7E0222
local ADDR_DEBUG_MONUMENT_CALLS = 0x7E0224
local ADDR_MONUMENT_DISPLAYED = 0x7E0226
local ADDR_MONUMENT_TICK = 0x7E0227
local ADDR_MONUMENT_LINE = 0x7E0228
local ADDR_MONUMENT_CHAR = 0x7E0229
local ADDR_OBJLIST = 0x7E11B1
local OBJ_SIZE = 25
local OBJ_LIST_SIZE = 29
local OBJ_TYPE_OFFSET = 9
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2
local PLAYER_POSF = ADDR_PLAYER + 25
local PLAYER_SPD = ADDR_PLAYER + 34
local TARGET_ROOM = 12

local frame = 0
local room12Seen = false
local room12StartFrame = nil
local room12StartFc = nil
local lastFc = nil
local sameFc = 0
local monumentObjX = nil
local monumentObjY = nil
local dumpedLiveObjects = false
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function write_result(text)
    local f = io.open(out, "w")
    if f then
        f:write(text)
        f:close()
    end
end

local function log(line)
    lines[#lines + 1] = line
    write_result(table.concat(lines, "\n") .. "\n")
end

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a)
    local lo = rd8(a)
    local hi = rd8(a + 1)
    if lo == nil or hi == nil then return nil end
    return lo + hi * 256
end
local function rd_s16(a)
    local v = rd16(a)
    if v == nil then return nil end
    if v >= 0x8000 then return v - 0x10000 end
    return v
end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a, v)
    wr8(a, v)
    wr8(a + 1, math.floor(v / 256))
end
local function wr32(a, v)
    wr8(a + 0, v)
    wr8(a + 1, math.floor(v / 256))
    wr8(a + 2, math.floor(v / 65536))
    wr8(a + 3, math.floor(v / 16777216))
end

local function set_pad(tbl)
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
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

local function dumpObjects()
    local i
    for i = 0, (OBJ_LIST_SIZE - 1) do
        local base = ADDR_OBJLIST + (i * OBJ_SIZE)
        local x = rd16(base + 0)
        local y = rd16(base + 2)
        local flags = rd8(base + 4)
        local eType = rd8(base + OBJ_TYPE_OFFSET)
        log(string.format("OBJ i=%d base=%06X x=%d y=%d flags=%d type=%d",
            i, base, x or -1, y or -1, flags or -1, eType or -1))
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

write_result("START\n")

emu.addEventCallback(function()
    local ok, err = pcall(function()
        frame = frame + 1

        local room = rd16(ADDR_ROOM)
        local fc = rd16(ADDR_FRAME)
        local px = rd_s16(PLAYER_X)
        local py = rd_s16(PLAYER_Y)
        local logicCount = rd16(ADDR_DEBUG_LOGIC)
        local monumentCalls = rd16(ADDR_DEBUG_MONUMENT_CALLS)
        local mon = rd8(ADDR_MONUMENT_DISPLAYED)
        local tick = rd8(ADDR_MONUMENT_TICK)
        local line = rd8(ADDR_MONUMENT_LINE)
        local ch = rd8(ADDR_MONUMENT_CHAR)

        if room == nil or fc == nil then
            log("FAIL nil-state")
            if emu.stop then emu.stop(1) end
            return
        end

        if frame >= 120 and frame <= 360 then
            local phase = frame % 30
            if phase < 10 then
                set_pad({ Start = true })
            elseif phase < 20 then
                set_pad({ A = true })
            else
                set_pad({})
            end
        elseif frame >= 390 and frame <= 540 then
            if (frame % 20) < 10 then
                set_pad({ B = true })
            else
                set_pad({})
            end
        elseif room >= 1 and room < TARGET_ROOM and (frame % 24) == 0 then
            set_pad({ R = true })
        else
            set_pad({})
        end

        if room == TARGET_ROOM then
            if not room12Seen then
                room12Seen = true
                room12StartFrame = frame
                room12StartFc = fc
                log(string.format("ROOM12 frame=%d fc=%d logic=%d moncalls=%d px=%d py=%d", frame, fc, logicCount, monumentCalls, px, py))
            end

            if not dumpedLiveObjects and monumentCalls ~= nil and monumentCalls > 0 then
                dumpedLiveObjects = true
                dumpObjects()
            end

            if monumentObjX == nil then
                local objX, objY, objIndex = findMonument()
                if objX ~= nil then
                    monumentObjX = objX
                    monumentObjY = objY
                    log(string.format("MONUMENT index=%d x=%d y=%d", objIndex, objX, objY))
                end
            end

            if monumentObjX ~= nil then
                wr16(PLAYER_X, monumentObjX + 4)
                wr16(PLAYER_Y, monumentObjY + 8)
                wr32(PLAYER_POSF + 0, 0)
                wr32(PLAYER_POSF + 4, 0)
                wr32(PLAYER_SPD + 0, 0)
                wr32(PLAYER_SPD + 4, 0)
            end

            if lastFc == fc then
                sameFc = sameFc + 1
            else
                sameFc = 0
            end
            lastFc = fc

            log(string.format("f=%d fc=%d same=%d logic=%d moncalls=%d px=%d py=%d mon=%d tick=%d line=%d char=%d",
                frame, fc, sameFc, logicCount, monumentCalls, px, py, mon, tick, line, ch))

            if sameFc >= 20 then
                log("HANG")
                if emu.stop then emu.stop(1) end
                return
            end

            if room12StartFrame ~= nil and (frame - room12StartFrame) >= 240 then
                log("DONE")
                if emu.stop then emu.stop(0) end
                return
            end
        elseif room12Seen then
            log(string.format("ROOM_LEFT room=%d fc=%d logic=%d moncalls=%d", room, fc, logicCount, monumentCalls))
            if emu.stop then emu.stop(1) end
            return
        elseif frame % 60 == 0 then
            log(string.format("PRE f=%d room=%d fc=%d logic=%d moncalls=%d", frame, room, fc, logicCount, monumentCalls))
        end

        if frame >= 2400 then
            log("TIMEOUT")
            if emu.stop then emu.stop(1) end
            return
        end
    end)

    if not ok then
        write_result(table.concat(lines, "\n") .. "\nLUA_ERROR " .. tostring(err) .. "\n")
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

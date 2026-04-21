local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room_trace_current_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData

local frame = 0
local done = false
local lastRoom = nil
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
local function rd16s(a)
    local v = rd16(a)
    if v >= 0x8000 then return v - 0x10000 end
    return v
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

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            setPad({ Start = true })
        elseif phase < 20 then
            setPad({ A = true })
        end
    elseif frame >= 600 and frame < 750 then
        setPad({ Right = true })
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)
    local px = rd16s(ADDR_PLAYER)
    local py = rd16s(ADDR_PLAYER + 2)

    if lastRoom ~= room then
        log(string.format("ROOM frame=%d fc=%d room=%d x=%d y=%d", frame, fc, room, px, py))
        lastRoom = room
    elseif frame == 1 or (frame % 60) == 0 then
        log(string.format("f=%d fc=%d room=%d x=%d y=%d", frame, fc, room, px, py))
    end

    if frame >= 1000 then
        log("DONE")
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

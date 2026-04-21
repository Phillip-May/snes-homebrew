local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room_write_trace_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount

local frame = 0
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function rd8(a) return emu.read(a, cpu) or 0 end
local function rd16(a) return rd8(a) + rd8(a + 1) * 256 end
local function rs16(a)
    local v = rd16(a)
    if v >= 0x8000 then return v - 0x10000 end
    return v
end
local function flush()
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function log(s)
    lines[#lines + 1] = s
    flush()
end
local function snap(tag, addr, val)
    local state = emu.getState and emu.getState() or nil
    local cpuState = state and state.cpu or nil
    local pc = cpuState and cpuState.pc or -1
    local k = cpuState and cpuState.k or -1
    log(string.format("%s f=%d fc=%u room=%u px=%d py=%d addr=%06X val=%02X k=%02X pc=%04X",
        tag, frame, rd16(ADDR_FRAME), rd16(ADDR_ROOM), rs16(ADDR_PLAYER), rs16(ADDR_PLAYER + 2), addr, val, k, pc))
end

log("START")

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

if emu.addMemoryCallback then
    emu.addMemoryCallback(function(addr, val)
        snap("WRITE", addr, val)
    end, emu.callbackType.write, ADDR_ROOM)

    emu.addMemoryCallback(function(addr, val)
        snap("WRITE", addr, val)
    end, emu.callbackType.write, ADDR_ROOM + 1)
end

emu.addEventCallback(function()
    frame = frame + 1
    local room = rd16(ADDR_ROOM)
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
    if room == 0 and frame >= 120 and frame <= 360 then
        if (frame % 20) < 10 then
            inputState.start = true
        end
    end
    if frame == 1 or frame % 30 == 0 then
        snap("TRACE", 0, 0)
    end
    if frame >= 240 then
        log("DONE")
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

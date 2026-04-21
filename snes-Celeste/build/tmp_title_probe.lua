local cpu = emu.memType.cpu
local outPath = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_probe_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount

local frame = 0
local done = false
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}
local lines = {}

local function rd8(addr) return emu.read(addr, cpu) end
local function rd16(addr)
    local lo = rd8(addr)
    local hi = rd8(addr + 1)
    return lo + hi * 256
end
local function flush()
    local f = io.open(outPath, "w")
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

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end

    if frame >= 120 and frame <= 1200 then
        inputState.b = true
    end

    local fc = rd16(ADDR_FRAME)
    local room = rd16(ADDR_ROOM)
    if frame == 1 or (frame % 60) == 0 or room ~= 0 then
        table.insert(lines, string.format("frame=%d fc=%d room=%d b=%s", frame, fc, room, tostring(inputState.b)))
        flush()
    end

    if room == 1 then
        table.insert(lines, "PASS")
        flush()
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 1500 then
        table.insert(lines, "FAIL")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

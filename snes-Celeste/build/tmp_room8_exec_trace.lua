local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room8_exec_trace_result.txt"
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
local function set_pad(tbl)
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
    end
end
local function snap(tag)
    log(string.format(
        "%s f=%d fc=%u room=%u px=%d py=%d",
        tag, frame, rd16(ADDR_FRAME), rd16(ADDR_ROOM), rs16(ADDR_PLAYER), rs16(ADDR_PLAYER + 2)))
end

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

if emu.addMemoryCallback and emu.memCallbackType then
    emu.addMemoryCallback(function() snap("EXEC _start") end, emu.memCallbackType.cpuExec, 0xB000)
    emu.addMemoryCallback(function() snap("EXEC COP") end, emu.memCallbackType.cpuExec, 0x8000)
    emu.addMemoryCallback(function() snap("EXEC BRK") end, emu.memCallbackType.cpuExec, 0x8040)
    emu.addMemoryCallback(function() snap("EXEC ABORT") end, emu.memCallbackType.cpuExec, 0x8080)
end

emu.addEventCallback(function()
    frame = frame + 1

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            set_pad({ Start = true })
        elseif phase < 20 then
            set_pad({ A = true })
        else
            set_pad({})
        end
    else
        set_pad({})
    end

    if frame == 1 or frame % 30 == 0 then
        snap("TRACE")
    end

    if frame >= 360 then
        log("DONE")
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

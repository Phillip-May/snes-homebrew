local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room8_trace_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_DEBUG_STAGE = sym.GLOBAL_DebugStage
local ADDR_INPUT_HI = 0x7E0214
local ADDR_TITLE = 0x7E0227
local ADDR_TITLE_TIMER = 0x7E022F
local ADDR_INPUT_LO = 0x7E0284
local ADDR_BG4X = 0x7E0276

local frame = 0
local lines = {}
local lastRoom = -1
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
    elseif frame == 600 then
        set_pad({ Right = true })
    elseif frame == 750 then
        set_pad({})
    else
        set_pad({})
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)
    local inTitle = rd8(ADDR_TITLE)
    local titleTimer = rd8(ADDR_TITLE_TIMER)
    local playerX = rs16(ADDR_PLAYER)
    local playerY = rs16(ADDR_PLAYER + 2)
    local debugStage = ADDR_DEBUG_STAGE and rd8(ADDR_DEBUG_STAGE) or 0
    local inputHi = rd8(ADDR_INPUT_HI)
    local inputLo = rd8(ADDR_INPUT_LO)
    local bg4x = rd16(ADDR_BG4X)

    if room ~= lastRoom then
        log(string.format("ROOM f=%d fc=%u room=%u inTitle=%u titleTimer=%u px=%d py=%d bg4x=%u dbg=%02X",
            frame, fc, room, inTitle, titleTimer, playerX, playerY, bg4x, debugStage))
        lastRoom = room
    end

    if frame == 1 or frame % 30 == 0 or room == 8 then
        log(string.format("TRACE f=%d fc=%u room=%u inTitle=%u timer=%u inHi=%02X inLo=%02X px=%d py=%d bg4x=%u dbg=%02X",
            frame, fc, room, inTitle, titleTimer, inputHi, inputLo, playerX, playerY, bg4x, debugStage))
    end

    if frame >= 900 then
        log("DONE")
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

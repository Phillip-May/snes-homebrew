local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_text_dma_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local TARGET_ROOM = 12
local TOUCH_X = 60
local TOUCH_Y = 40
local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2
local WRAM = 0x7E0000
local ADDR_TEXT_MAP_DMA_COUNT = WRAM + 0x1DA8
local ADDR_TEXT_SLOT_DMA_COUNT = WRAM + 0x1DA9
local ADDR_TEXT_ANY_SLOT_DIRTY = WRAM + 0x1BD4
local ADDR_TEXT_MAP_DIRTY = WRAM + 0x1BD5

local frame = 0
local done = false
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
    for k, _ in pairs(inputState) do inputState[k] = false end
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
        if not ok then pcall(function() emu.setInput(inputState, 1) end) end
    end, emu.eventType.inputPolled)
end

log("START")
flush()

emu.addEventCallback(function()
    if done then return end

    frame = frame + 1
    clearPad()

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then inputState.start = true
        elseif phase < 20 then inputState.a = true end
    elseif frame >= 390 and frame <= 540 then
        if (frame % 20) < 10 then inputState.b = true end
    end

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)

    if room == TARGET_ROOM then
        if roomHitFrame == nil then
            roomHitFrame = frame
            log(string.format("ROOM%d frame=%d fc=%d", TARGET_ROOM, frame, fc))
        end
        wr16(PLAYER_X, TOUCH_X)
        wr16(PLAYER_Y, TOUCH_Y)
    end

    if roomHitFrame ~= nil and (frame - roomHitFrame) <= 180 and ((frame - roomHitFrame) % 10) == 0 then
        log(string.format("f=%d fc=%d mapDma=%d slotDma=%d anyDirty=%d mapDirty=%d",
            frame, fc,
            rd8(ADDR_TEXT_MAP_DMA_COUNT),
            rd8(ADDR_TEXT_SLOT_DMA_COUNT),
            rd8(ADDR_TEXT_ANY_SLOT_DIRTY),
            rd16(ADDR_TEXT_MAP_DIRTY)))
        flush()
    end

    if roomHitFrame ~= nil and (frame - roomHitFrame) >= 180 then
        log("DONE")
        flush()
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 3000 then
        log("TIMEOUT")
        flush()
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

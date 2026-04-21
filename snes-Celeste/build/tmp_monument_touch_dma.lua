local testlib = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_PlayerData",
    "GLOBAL_FrameCount",
    "GLOBAL_OBJList",
    "GLOBAL_MonumentTextDisplayed",
    "GLOBAL_MonumentCurLineNum",
    "GLOBAL_MonumentCurLineCharCount",
    "s_monumentTextTick",
    "s_bg1TextAnySlotDirty",
    "s_bg1TextMapDirtyRowBits",
    "s_textMapDmaCount",
    "s_textSlotDmaCount",
    "GLOBAL_DebugMonumentCalls",
    "GLOBAL_DebugMonumentTouches",
    "GLOBAL_DebugMonumentLastPlayerX",
    "GLOBAL_DebugMonumentLastPlayerY",
    "GLOBAL_DebugMonumentLastTouch",
    "GLOBAL_DebugMonumentLastTickBefore",
    "GLOBAL_DebugMonumentLastTickAfter"
})

local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_touch_dma_result.txt"
local outPng = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_touch_dma.png"

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_OBJLIST = sym.GLOBAL_OBJList
local ADDR_MON_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MON_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MON_CHAR = sym.GLOBAL_MonumentCurLineCharCount
local ADDR_MON_TICK = sym.s_monumentTextTick
local ADDR_TEXT_ANY_DIRTY = sym.s_bg1TextAnySlotDirty
local ADDR_TEXT_MAP_DIRTY = sym.s_bg1TextMapDirtyRowBits
local ADDR_TEXT_MAP_DMA_COUNT = sym.s_textMapDmaCount
local ADDR_TEXT_SLOT_DMA_COUNT = sym.s_textSlotDmaCount
local ADDR_DBG_CALLS = sym.GLOBAL_DebugMonumentCalls
local ADDR_DBG_TOUCHES = sym.GLOBAL_DebugMonumentTouches
local ADDR_DBG_LAST_PX = sym.GLOBAL_DebugMonumentLastPlayerX
local ADDR_DBG_LAST_PY = sym.GLOBAL_DebugMonumentLastPlayerY
local ADDR_DBG_LAST_TOUCH = sym.GLOBAL_DebugMonumentLastTouch
local ADDR_DBG_TICK_BEFORE = sym.GLOBAL_DebugMonumentLastTickBefore
local ADDR_DBG_TICK_AFTER = sym.GLOBAL_DebugMonumentLastTickAfter
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local OBJ_SIZE = 25
local OBJ_LIST_SIZE = 29
local OBJ_TYPE_OFFSET = 9
local OBJ_MONUMENT = 70
local TARGET_ROOM = 12

local frame = 0
local done = false
local roomHitFrame = nil
local monumentStartFrame = nil
local monumentX = nil
local monumentY = nil
local lines = {}
local inputState = testlib.new_input_state()

local function rd8(a) return testlib.read_u8(a, cpu) or 0 end
local function rd16(a) return testlib.read_u16(a, cpu) or 0 end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a, v)
    wr8(a, v)
    wr8(a + 1, math.floor(v / 256))
end
local function clearPad() testlib.set_pad(inputState, {}) end
local function log(line)
    lines[#lines + 1] = line
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function savePng(path)
    local png = emu.takeScreenshot()
    local f = io.open(path, "wb")
    if f then
        f:write(png)
        f:close()
    end
end
local function findMonument()
    for i = 1, (OBJ_LIST_SIZE - 1) do
        local base = ADDR_OBJLIST + (i * OBJ_SIZE)
        if rd8(base + OBJ_TYPE_OFFSET) == OBJ_MONUMENT then
            return rd16(base + 0), rd16(base + 2), i
        end
    end
    return nil, nil, nil
end

testlib.install_input_callback(inputState)
emu.addEventCallback(function()
    if monumentX ~= nil and rd16(ADDR_ROOM) == TARGET_ROOM then
        wr16(PLAYER_X, monumentX + 4)
        wr16(PLAYER_Y, monumentY + 8)
    end
end, emu.eventType.inputPolled)
log("START")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    clearPad()

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)
    local monDisplayed = rd8(ADDR_MON_DISPLAYED)
    local monLine = rd8(ADDR_MON_LINE)
    local monChar = rd8(ADDR_MON_CHAR)
    local monTick = rd8(ADDR_MON_TICK)

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            inputState.start = true
        elseif phase < 20 then
            inputState.a = true
        else
            inputState.b = true
        end
    elseif room >= 1 and room < TARGET_ROOM then
        local phase = (frame - 360) % 16
        if phase < 4 then
            inputState.r = true
        end
    end

    if room == TARGET_ROOM then
        if roomHitFrame == nil then
            roomHitFrame = frame
            log(string.format("ROOM%d frame=%d fc=%d", TARGET_ROOM, frame, fc))
        end
        if monumentX == nil then
            local x, y, index = findMonument()
            if x ~= nil then
                monumentX = x
                monumentY = y
                log(string.format("MONUMENT index=%d x=%d y=%d", index, x, y))
            end
        end
        if monumentStartFrame == nil and (monDisplayed ~= 0 or monLine ~= 0 or monChar ~= 0) then
            monumentStartFrame = frame
            log(string.format(
                "TEXT_START frame=%d fc=%d mon=%d line=%d char=%d tick=%d anyDirty=%d mapDirty=%04x mapDma=%d slotDma=%d calls=%d touches=%d lastTouch=%d lastPx=%d lastPy=%d tickB=%d tickA=%d",
                frame, fc, monDisplayed, monLine, monChar, monTick,
                rd8(ADDR_TEXT_ANY_DIRTY), rd16(ADDR_TEXT_MAP_DIRTY),
                rd8(ADDR_TEXT_MAP_DMA_COUNT), rd8(ADDR_TEXT_SLOT_DMA_COUNT),
                rd16(ADDR_DBG_CALLS), rd16(ADDR_DBG_TOUCHES), rd8(ADDR_DBG_LAST_TOUCH),
                rd16(ADDR_DBG_LAST_PX), rd16(ADDR_DBG_LAST_PY),
                rd8(ADDR_DBG_TICK_BEFORE), rd8(ADDR_DBG_TICK_AFTER)))
        end
    end

    if room == TARGET_ROOM and ((frame - roomHitFrame) % 10) == 0 then
        log(string.format(
            "f=%d fc=%d px=%d py=%d mon=%d line=%d char=%d tick=%d anyDirty=%d mapDirty=%04x mapDma=%d slotDma=%d calls=%d touches=%d lastTouch=%d lastPx=%d lastPy=%d tickB=%d tickA=%d",
            frame, fc, rd16(PLAYER_X), rd16(PLAYER_Y),
            monDisplayed, monLine, monChar, monTick,
            rd8(ADDR_TEXT_ANY_DIRTY), rd16(ADDR_TEXT_MAP_DIRTY),
            rd8(ADDR_TEXT_MAP_DMA_COUNT), rd8(ADDR_TEXT_SLOT_DMA_COUNT),
            rd16(ADDR_DBG_CALLS), rd16(ADDR_DBG_TOUCHES), rd8(ADDR_DBG_LAST_TOUCH),
            rd16(ADDR_DBG_LAST_PX), rd16(ADDR_DBG_LAST_PY),
            rd8(ADDR_DBG_TICK_BEFORE), rd8(ADDR_DBG_TICK_AFTER)))
    end

    if monumentStartFrame ~= nil and monLine >= 1 and monChar >= 10 then
        savePng(outPng)
        log("SHOT")
        done = true
        if emu.stop then
            emu.stop(0)
        end
        return
    end

    if frame >= 2400 then
        log("TIMEOUT")
        done = true
        if emu.stop then
            emu.stop(1)
        end
    end
end, emu.eventType.endFrame)

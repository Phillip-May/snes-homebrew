local projectRoot = os.getenv("SNES_CELESTE_ROOT") or "."
local testlib = dofile(projectRoot .. "/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_FrameCount",
    "GLOBAL_PlayerData",
    "GLOBAL_MonumentTextDisplayed",
    "GLOBAL_MonumentCurLineNum",
    "GLOBAL_MonumentCurLineCharCount",
    "s_monumentTextTick",
    "GLOBAL_OBJList",
})

local _, write_result = testlib.make_result_writer("mesen_monument_verify_result.txt")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_MON_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MON_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MON_CHAR = sym.GLOBAL_MonumentCurLineCharCount
local ADDR_MON_TICK = sym.s_monumentTextTick
local ADDR_OBJLIST = sym.GLOBAL_OBJList

local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local OBJ_SIZE = 25
local OBJ_LIST_SIZE = 29
local OBJ_TYPE_OFFSET = 9
local OBJ_MONUMENT = 70
local TARGET_ROOM = 12

local frame = 0
local roomHitFrame = nil
local monumentX = nil
local monumentY = nil
local lastFrameCount = nil
local sameFrameCount = 0
local inputState = testlib.new_input_state()

local function rd8(addr)
    return testlib.read_u8(addr, cpu) or 0
end

local function rd16(addr)
    return testlib.read_u16(addr, cpu) or 0
end

local function wr16(addr, value)
    emu.write(addr, value & 0xFF, cpu)
    emu.write(addr + 1, math.floor(value / 256) & 0xFF, cpu)
end

local function finish(ok, text)
    write_result(text)
    if emu.stop then
        emu.stop(ok and 0 or 1)
    end
end

local function findMonument()
    for i = 1, (OBJ_LIST_SIZE - 1) do
        local base = ADDR_OBJLIST + (i * OBJ_SIZE)
        if rd8(base + OBJ_TYPE_OFFSET) == OBJ_MONUMENT then
            return rd16(base + 0), rd16(base + 2)
        end
    end
    return nil, nil
end

testlib.install_input_callback(inputState)
write_result("RUNNING")

emu.addEventCallback(function()
    if monumentX ~= nil and rd16(ADDR_ROOM) == TARGET_ROOM then
        wr16(PLAYER_X, monumentX + 4)
        wr16(PLAYER_Y, monumentY + 8)
    end
end, emu.eventType.inputPolled)

emu.addEventCallback(function()
    frame = frame + 1
    testlib.set_pad(inputState, {})

    local room = rd16(ADDR_ROOM)
    local frameCount = rd16(ADDR_FRAME)
    local monDisplayed = rd8(ADDR_MON_DISPLAYED)
    local monLine = rd8(ADDR_MON_LINE)
    local monChar = rd8(ADDR_MON_CHAR)
    local monTick = rd8(ADDR_MON_TICK)

    if frame >= 120 and frame <= 360 then
        local bootPhase = frame % 30
        if bootPhase < 10 then
            inputState.start = true
        elseif bootPhase < 20 then
            inputState.a = true
        else
            inputState.b = true
        end
    elseif room >= 1 and room < TARGET_ROOM then
        local movePhase = (frame - 360) % 16
        if movePhase < 4 then
            inputState.r = true
        end
    end

    if lastFrameCount == frameCount then
        sameFrameCount = sameFrameCount + 1
    else
        sameFrameCount = 0
    end
    lastFrameCount = frameCount

    if room == TARGET_ROOM then
        if roomHitFrame == nil then
            roomHitFrame = frame
        end
        if monumentX == nil then
            monumentX, monumentY = findMonument()
        end
    end

    if room == TARGET_ROOM and monumentX ~= nil and monDisplayed ~= 0 and monChar >= 3 then
        finish(true, string.format(
            "PASS frame=%d fc=%d room=%d mon=%d x=%d y=%d displayed=%d line=%d char=%d tick=%d",
            frame, frameCount, room, OBJ_MONUMENT, monumentX, monumentY, monDisplayed, monLine, monChar, monTick
        ))
        return
    end

    if roomHitFrame ~= nil and room ~= TARGET_ROOM then
        finish(false, string.format("FAIL left-room frame=%d fc=%d room=%d", frame, frameCount, room))
        return
    end

    if roomHitFrame ~= nil and sameFrameCount >= 20 then
        finish(false, string.format(
            "FAIL hang frame=%d fc=%d room=%d displayed=%d line=%d char=%d tick=%d",
            frame, frameCount, room, monDisplayed, monLine, monChar, monTick
        ))
        return
    end

    if frame >= 2400 then
        finish(false, string.format(
            "FAIL timeout frame=%d fc=%d room=%d monX=%s monY=%s displayed=%d line=%d char=%d tick=%d",
            frame, frameCount, room, tostring(monumentX), tostring(monumentY), monDisplayed, monLine, monChar, monTick
        ))
    end
end, emu.eventType.endFrame)

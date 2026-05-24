local projectRoot = os.getenv("SNES_CELESTE_ROOT") or "."
local testlib = dofile(projectRoot .. "/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_FrameCount",
    "GLOBAL_PlayerData",
    "GLOBAL_OBJList",
    "GLOBAL_FlagOverlayShow",
    "GLOBAL_FlagOverlayDirty",
    "GLOBAL_FlagOverlayRevealDirty",
    "GLOBAL_FlagOverlayLine0Len",
    "GLOBAL_FlagOverlayLine1Len",
    "GLOBAL_FlagOverlayLine2Len",
    "GLOBAL_FlagOverlayLine0",
    "GLOBAL_FlagOverlayLine1",
    "GLOBAL_FlagOverlayLine2",
})

local _, write_result = testlib.make_result_writer("mesen_flag_verify_result.txt")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_OBJLIST = sym.GLOBAL_OBJList
local ADDR_FLAG_SHOW = sym.GLOBAL_FlagOverlayShow
local ADDR_FLAG_DIRTY = sym.GLOBAL_FlagOverlayDirty
local ADDR_FLAG_REVEAL_DIRTY = sym.GLOBAL_FlagOverlayRevealDirty
local ADDR_LINE0_LEN = sym.GLOBAL_FlagOverlayLine0Len
local ADDR_LINE1_LEN = sym.GLOBAL_FlagOverlayLine1Len
local ADDR_LINE2_LEN = sym.GLOBAL_FlagOverlayLine2Len
local ADDR_LINE0 = sym.GLOBAL_FlagOverlayLine0
local ADDR_LINE1 = sym.GLOBAL_FlagOverlayLine1
local ADDR_LINE2 = sym.GLOBAL_FlagOverlayLine2

local OBJ_SIZE = 25
local OBJ_TYPE_OFFSET = 9
local OBJ_DATA_OFFSET = 10
local OBJ_FLAG = 118
local OBJ_DIRTY = 0x80
local TEST_OBJ_INDEX = 6

local frame = 0
local injected = false
local touchStarted = false
local hiddenPreparedFrame = nil
local textOkFrame = nil
local revealCleanFrame = nil
local lastFrameCount = nil
local sameFrameCount = 0
local inputState = testlib.new_input_state()

local function rd8(addr)
    return testlib.read_u8(addr, cpu) or 0
end

local function rd16(addr)
    return testlib.read_u16(addr, cpu) or 0
end

local function wr8(addr, value)
    emu.write(addr, value & 0xFF, cpu)
end

local function wr16(addr, value)
    wr8(addr, value)
    wr8(addr + 1, math.floor(value / 256))
end

local function read_text(addr, len)
    local out = {}
    for i = 0, len - 1 do
        out[#out + 1] = string.char(rd8(addr + i))
    end
    return table.concat(out)
end

local function finish(ok, text)
    write_result(text)
    if emu.stop then
        emu.stop(ok and 0 or 1)
    end
end

local function inject_flag()
    local px = rd16(ADDR_PLAYER + 0)
    local py = rd16(ADDR_PLAYER + 2)
    local base = ADDR_OBJLIST + (TEST_OBJ_INDEX * OBJ_SIZE)
    wr16(base + 0, px + 80)
    wr16(base + 2, py)
    wr8(base + 4, OBJ_DIRTY)
    wr8(base + 5, 0)
    wr8(base + 6, 0x32)
    wr8(base + 7, 0xFF)
    wr8(base + 8, 0)
    wr8(base + OBJ_TYPE_OFFSET, OBJ_FLAG)
    wr8(base + OBJ_DATA_OFFSET + 0, 0)
    wr8(base + OBJ_DATA_OFFSET + 1, 0)
    wr8(base + OBJ_DATA_OFFSET + 2, 0)
    wr8(base + OBJ_DATA_OFFSET + 3, 0xFF)
    wr16(base + OBJ_DATA_OFFSET + 4, 0xFFFF)
    wr16(base + OBJ_DATA_OFFSET + 6, 0xFFFF)
end

local function move_flag_to_player()
    local px = rd16(ADDR_PLAYER + 0)
    local py = rd16(ADDR_PLAYER + 2)
    local base = ADDR_OBJLIST + (TEST_OBJ_INDEX * OBJ_SIZE)
    wr16(base + 0, px)
    wr16(base + 2, py)
end

local function flag_text_ok()
    local len0 = rd8(ADDR_LINE0_LEN)
    local len1 = rd8(ADDR_LINE1_LEN)
    local len2 = rd8(ADDR_LINE2_LEN)
    local line0 = read_text(ADDR_LINE0, len0)
    local line1 = read_text(ADDR_LINE1, len1)
    local line2 = read_text(ADDR_LINE2, len2)
    return line0 == "x0" and len1 == 8 and line1:sub(3, 3) == ":" and line1:sub(6, 6) == ":" and line2 == "deaths:0",
           line0, line1, line2, len1
end

testlib.install_input_callback(inputState)
write_result("RUNNING")

emu.addEventCallback(function()
    frame = frame + 1
    testlib.set_pad(inputState, {})

    local room = rd16(ADDR_ROOM)
    local frameCount = rd16(ADDR_FRAME)

    if frame >= 120 and frame <= 360 then
        local bootPhase = frame % 30
        if bootPhase < 10 then
            inputState.start = true
        elseif bootPhase < 20 then
            inputState.a = true
        else
            inputState.b = true
        end
    end

    if lastFrameCount == frameCount then
        sameFrameCount = sameFrameCount + 1
    else
        sameFrameCount = 0
    end
    lastFrameCount = frameCount

    if room == 1 and frame >= 700 and not injected then
        inject_flag()
        injected = true
    end

    if injected and hiddenPreparedFrame == nil and rd8(ADDR_FLAG_SHOW) == 0 then
        local okText = flag_text_ok()
        if okText and rd8(ADDR_FLAG_DIRTY) == 0 then
            hiddenPreparedFrame = frame
        end
    end

    if hiddenPreparedFrame ~= nil and not touchStarted and frame >= hiddenPreparedFrame + 120 then
        touchStarted = true
    end
    if touchStarted and rd8(ADDR_FLAG_SHOW) == 0 then
        move_flag_to_player()
    end

    if touchStarted and rd8(ADDR_FLAG_SHOW) ~= 0 then
        local okText, line0, line1, line2, len1 = flag_text_ok()
        if not okText then
            finish(false, string.format("FAIL bad-text frame=%d line0=%s len1=%d line1=%s line2=%s", frame, line0, len1, line1, line2))
            return
        end
        if textOkFrame == nil then
            textOkFrame = frame
        end
        if rd8(ADDR_FLAG_DIRTY) == 0 and rd8(ADDR_FLAG_REVEAL_DIRTY) == 0 and revealCleanFrame == nil then
            revealCleanFrame = frame
        end
        if revealCleanFrame ~= nil and frame >= revealCleanFrame + 90 then
            finish(true, string.format("PASS frame=%d fc=%d room=%d hiddenPrepared=%d revealClean=%d line0=%s line1=%s line2=%s", frame, frameCount, room, hiddenPreparedFrame, revealCleanFrame, line0, line1, line2))
        end
        return
    end

    if textOkFrame ~= nil and revealCleanFrame == nil and frame >= textOkFrame + 60 then
        finish(false, string.format("FAIL dirty-not-cleared frame=%d fc=%d room=%d", frame, frameCount, room))
        return
    end

    if sameFrameCount >= 20 and frame > 700 then
        finish(false, string.format("FAIL hang frame=%d fc=%d room=%d injected=%s touch=%s dirty=%d revealDirty=%d", frame, frameCount, room, tostring(injected), tostring(touchStarted), rd8(ADDR_FLAG_DIRTY), rd8(ADDR_FLAG_REVEAL_DIRTY)))
        return
    end

    if frame >= 1800 then
        finish(false, string.format("FAIL timeout frame=%d fc=%d room=%d show=%d dirty=%d revealDirty=%d injected=%s hiddenPrepared=%s touch=%s", frame, frameCount, room, rd8(ADDR_FLAG_SHOW), rd8(ADDR_FLAG_DIRTY), rd8(ADDR_FLAG_REVEAL_DIRTY), tostring(injected), tostring(hiddenPreparedFrame), tostring(touchStarted)))
    end
end, emu.eventType.endFrame)

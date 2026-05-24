local projectRoot = os.getenv("SNES_CELESTE_ROOT") or "."
local testlib = dofile(projectRoot .. "/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_PlayerData",
    "GLOBAL_FrameCount",
    "GLOBAL_OBJList"
})

local outPath, write_result = testlib.make_result_writer("mesen_breakable_verify_result.txt")
local shotPath = testlib.buildDir .. "/mesen_breakable_verify.png"

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_OBJLIST = sym.GLOBAL_OBJList
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local OBJ_SIZE = 25
local OBJ_LIST_SIZE = 29
local OBJ_TYPE_OFFSET = 9
local OBJ_BREAKABLE_WALL = 64
local TARGET_ROOM = 9

local frame = 0
local roomHitFrame = nil
local wallX = nil
local wallY = nil
local inputState = testlib.new_input_state()

local function rd8(a) return testlib.read_u8(a, cpu) or 0 end
local function rd16(a) return testlib.read_u16(a, cpu) or 0 end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a, v)
    wr8(a, v)
    wr8(a + 1, math.floor(v / 256))
end
local function savePng(path)
    local png = emu.takeScreenshot()
    local f = io.open(path, "wb")
    if f then
        f:write(png)
        f:close()
    end
end
local function finish(ok, text)
    write_result(text)
    if emu.stop then
        emu.stop(ok and 0 or 1)
    end
end
local function findBreakableWall()
    for i = 1, (OBJ_LIST_SIZE - 1) do
        local base = ADDR_OBJLIST + (i * OBJ_SIZE)
        if rd8(base + OBJ_TYPE_OFFSET) == OBJ_BREAKABLE_WALL then
            return rd16(base + 0), rd16(base + 2)
        end
    end
    return nil, nil
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
    elseif room >= 1 and room < TARGET_ROOM then
        local movePhase = (frame - 360) % 16
        if movePhase < 4 then
            inputState.r = true
        end
    end

    if room == TARGET_ROOM then
        if roomHitFrame == nil then
            roomHitFrame = frame
        end
        if wallX == nil then
            wallX, wallY = findBreakableWall()
        end
        if wallX ~= nil then
            wr16(PLAYER_X, wallX + 24)
            wr16(PLAYER_Y, wallY + 4)
        end
    end

    if roomHitFrame ~= nil and wallX ~= nil and (frame - roomHitFrame) >= 120 then
        savePng(shotPath)
        finish(true, string.format("PASS frame=%d fc=%d room=%d wallX=%d wallY=%d", frame, frameCount, room, wallX, wallY))
        return
    end

    if frame >= 1800 then
        finish(false, string.format("FAIL timeout room=%d frame=%d fc=%d wallX=%s wallY=%s",
            room, frame, frameCount, tostring(wallX), tostring(wallY)))
    end
end, emu.eventType.endFrame)

local testlib = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_PlayerData",
    "GLOBAL_FrameCount",
    "GLOBAL_OBJList"
})

local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room9_breakable_capture.txt"
local outPng = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room9_breakable_capture.png"

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
local done = false
local roomHitFrame = nil
local wallX = nil
local wallY = nil
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
local function findBreakableWall()
    for i = 1, (OBJ_LIST_SIZE - 1) do
        local base = ADDR_OBJLIST + (i * OBJ_SIZE)
        if rd8(base + OBJ_TYPE_OFFSET) == OBJ_BREAKABLE_WALL then
            return rd16(base + 0), rd16(base + 2), i
        end
    end
    return nil, nil, nil
end

testlib.install_input_callback(inputState)
log("START")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    clearPad()

    local room = rd16(ADDR_ROOM)
    local fc = rd16(ADDR_FRAME)

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
        if wallX == nil then
            local x, y, index = findBreakableWall()
            if x ~= nil then
                wallX = x
                wallY = y
                log(string.format("BREAKABLE index=%d x=%d y=%d", index, x, y))
            end
        end
        if wallX ~= nil then
            -- Keep the wall on-screen without colliding into it.
            wr16(PLAYER_X, wallX + 24)
            wr16(PLAYER_Y, wallY + 4)
        end
    end

    if frame == 1 or (frame % 60) == 0 or room == TARGET_ROOM then
        log(string.format("f=%d fc=%d room=%d px=%d py=%d wall=%s,%s",
            frame, fc, room, rd16(PLAYER_X), rd16(PLAYER_Y), tostring(wallX), tostring(wallY)))
    end

    if roomHitFrame ~= nil and wallX ~= nil and (frame - roomHitFrame) >= 120 then
        local png = emu.takeScreenshot()
        local f = io.open(outPng, "wb")
        if f then
            f:write(png)
            f:close()
        end
        log("SHOT")
        done = true
        if emu.stop then
            emu.stop(0)
        end
        return
    end

    if frame >= 1800 then
        log("TIMEOUT")
        done = true
        if emu.stop then
            emu.stop(1)
        end
    end
end, emu.eventType.endFrame)

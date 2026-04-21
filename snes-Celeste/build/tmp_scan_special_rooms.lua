local testlib = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_FrameCount",
    "GLOBAL_OBJList"
})

local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/special_room_scan.txt"
local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_OBJLIST = sym.GLOBAL_OBJList

local OBJ_SIZE = 25
local OBJ_LIST_SIZE = 29
local OBJ_TYPE_OFFSET = 9
local OBJ_BREAKABLE_WALL = 64
local OBJ_MONUMENT = 70
local MAX_ROOM = 30

local frame = 0
local done = false
local currentRoom = 0
local roomEntryFrame = nil
local lines = {}
local inputState = testlib.new_input_state()

local function rd8(a) return testlib.read_u8(a, cpu) or 0 end
local function rd16(a) return testlib.read_u16(a, cpu) or 0 end
local function log(line)
    lines[#lines + 1] = line
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end
local function clearPad() testlib.set_pad(inputState, {}) end
local function roomHasType(targetType)
    for i = 0, (OBJ_LIST_SIZE - 1) do
        local base = ADDR_OBJLIST + (i * OBJ_SIZE)
        if rd8(base + OBJ_TYPE_OFFSET) == targetType then
            return true
        end
    end
    return false
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
    elseif room >= 1 and room < MAX_ROOM then
        if roomEntryFrame ~= nil and (frame - roomEntryFrame) >= 20 then
            inputState.r = true
        end
    end

    if room ~= currentRoom then
        currentRoom = room
        roomEntryFrame = frame
        if room >= 1 and room <= MAX_ROOM then
            log(string.format("ROOM %d frame=%d fc=%d", room, frame, fc))
        end
    end

    if room >= 1 and room <= MAX_ROOM and roomEntryFrame ~= nil and (frame - roomEntryFrame) == 10 then
        local hasBreakable = roomHasType(OBJ_BREAKABLE_WALL)
        local hasMonument = roomHasType(OBJ_MONUMENT)
        log(string.format("SCAN room=%d breakable=%s monument=%s", room, tostring(hasBreakable), tostring(hasMonument)))
    end

    if room >= MAX_ROOM and roomEntryFrame ~= nil and (frame - roomEntryFrame) >= 40 then
        log("DONE")
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

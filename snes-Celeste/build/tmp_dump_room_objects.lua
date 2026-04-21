local testlib = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_PlayerData",
    "GLOBAL_FrameCount",
    "GLOBAL_OBJList"
})

local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room_object_dump.txt"
local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_OBJLIST = sym.GLOBAL_OBJList

local OBJ_SIZE = 25
local OBJ_LIST_SIZE = 29
local OBJ_TYPE_OFFSET = 9
local TARGET_ROOM = 8

local frame = 0
local done = false
local dumped = false
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

    if room == TARGET_ROOM and not dumped then
        dumped = true
        log(string.format("ROOM%d frame=%d fc=%d", TARGET_ROOM, frame, fc))
        for i = 0, (OBJ_LIST_SIZE - 1) do
            local base = ADDR_OBJLIST + (i * OBJ_SIZE)
            local eType = rd8(base + OBJ_TYPE_OFFSET)
            local x = rd16(base + 0)
            local y = rd16(base + 2)
            log(string.format("obj[%d] type=%d x=%d y=%d base=%06X", i, eType, x, y, base))
        end
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

local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_crash_write_trace.txt"

local ADDR_INPUT_STATE = 0x7E021C
local ADDR_IN_TITLE = 0x7E022F
local ADDR_TITLE_TIMER = 0x7E0237
local ADDR_INPUT_LO = 0x7E028C
local ADDR_ROOM = 0x7E028D
local ADDR_FRAME = 0x7E1490
local ADDR_PRG_ACTIVE = 0x7E1F5A
local ADDR_PRG_SP = 0x7E1F5B
local ADDR_PRG_STACK = 0x7E1F5C

local frame = 0
local done = false
local room1Seen = false
local lines = {}
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function rd8(addr)
    return emu.read(addr, cpu) or 0
end

local function rd16(addr)
    return rd8(addr) + rd8(addr + 1) * 256
end

local function flush()
    local f = io.open(out, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

local function log(msg)
    lines[#lines + 1] = msg
    flush()
end

local function clearPad()
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
end

local function cpuStateString()
    local state = emu.getState and emu.getState() or nil
    local c = state and state.cpu or nil
    if not c then
        return "k=?? pc=???? sp=???? a=???? x=???? y=???? p=??"
    end
    return string.format("k=%02X pc=%04X sp=%04X a=%04X x=%04X y=%04X p=%02X",
        c.k or 0, c.pc or 0, c.sp or 0, c.a or 0, c.x or 0, c.y or 0, c.ps or 0)
end

local function snapshot(tag)
    log(string.format(
        "%s f=%d fc=%d room=%d title=%d timer=%d state=%02X lo=%02X bank=%02X sp=%02X stk=%02X,%02X,%02X,%02X %s",
        tag,
        frame,
        rd16(ADDR_FRAME),
        rd16(ADDR_ROOM),
        rd8(ADDR_IN_TITLE),
        rd8(ADDR_TITLE_TIMER),
        rd8(ADDR_INPUT_STATE),
        rd8(ADDR_INPUT_LO),
        rd8(ADDR_PRG_ACTIVE),
        rd8(ADDR_PRG_SP),
        rd8(ADDR_PRG_STACK + 0),
        rd8(ADDR_PRG_STACK + 1),
        rd8(ADDR_PRG_STACK + 2),
        rd8(ADDR_PRG_STACK + 3),
        cpuStateString()))
end

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

if emu.addMemoryCallback then
    local function watch(addr, name)
        emu.addMemoryCallback(function(cbAddr, val)
            if frame > 260 then
                return
            end
            log(string.format(
                "WRITE[%s] f=%d fc=%d room=%d title=%d bank=%02X sp=%02X addr=%06X val=%02X %s",
                name,
                frame,
                rd16(ADDR_FRAME),
                rd16(ADDR_ROOM),
                rd8(ADDR_IN_TITLE),
                rd8(ADDR_PRG_ACTIVE),
                rd8(ADDR_PRG_SP),
                cbAddr,
                val,
                cpuStateString()))
        end, emu.callbackType.write, addr)
    end

    watch(ADDR_ROOM, "room_lo")
    watch(ADDR_ROOM + 1, "room_hi")
    watch(ADDR_FRAME, "frame_lo")
    watch(ADDR_FRAME + 1, "frame_hi")
    watch(ADDR_IN_TITLE, "title")
    watch(ADDR_PRG_ACTIVE, "bank")
    watch(ADDR_PRG_SP, "bank_sp")
end

log("START")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    clearPad()

    if rd16(ADDR_ROOM) == 1 then
        room1Seen = true
    end

    if room1Seen then
        -- Leave gameplay untouched.
    elseif frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            inputState.start = true
        elseif phase < 20 then
            inputState.a = true
        end
    elseif frame >= 390 and frame <= 540 then
        if (frame % 20) < 10 then
            inputState.b = true
        end
    end

    if frame == 1 or frame % 30 == 0 or (frame >= 170 and frame <= 220) then
        snapshot("TRACE")
    end

    if frame >= 260 then
        snapshot("DONE")
        done = true
        if emu.stop then
            emu.stop(0)
        end
    end
end, emu.eventType.endFrame)

local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_a_late_result.txt"
local ADDR_INPUT_STATE = 0x7E0214
local ADDR_INPUT_LO = 0x7E0284
local ADDR_ROOM = 0x7E0285
local ADDR_FRAME = 0x7E1488

local frame = 0
local done = false
local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}
local lines = {}

local function rd8(addr) return emu.read(addr, cpu) or 0 end
local function rd16(addr)
    local lo = rd8(addr)
    local hi = rd8(addr + 1)
    return lo + hi * 256
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

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

log("RUNNING")

emu.addEventCallback(function()
    if done then return end
    frame = frame + 1
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end

    if frame >= 240 and frame <= 320 then
        inputState.a = true
    end

    if frame == 1 or frame == 180 or frame == 240 or frame == 260 or frame == 300 or frame == 340 or frame == 420 then
        log(string.format("f=%d fc=%d room=%d state=%02X lo=%02X a=%s",
            frame, rd16(ADDR_FRAME), rd16(ADDR_ROOM), rd8(ADDR_INPUT_STATE), rd8(ADDR_INPUT_LO), tostring(inputState.a)))
    end

    if rd16(ADDR_ROOM) == 1 then
        log(string.format("PASS f=%d fc=%d state=%02X lo=%02X",
            frame, rd16(ADDR_FRAME), rd8(ADDR_INPUT_STATE), rd8(ADDR_INPUT_LO)))
        done = true
        if emu.stop then emu.stop(0) end
        return
    end

    if frame >= 900 then
        log(string.format("FAIL f=%d fc=%d room=%d state=%02X lo=%02X",
            frame, rd16(ADDR_FRAME), rd16(ADDR_ROOM), rd8(ADDR_INPUT_STATE), rd8(ADDR_INPUT_LO)))
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

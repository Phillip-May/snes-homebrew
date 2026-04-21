local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_input_probe_result.txt"
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

local function rd8(addr) return emu.read(addr, cpu) end
local function rd16(addr)
    local lo = rd8(addr)
    local hi = rd8(addr + 1)
    return (lo or 0) + (hi or 0) * 256
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

    if frame >= 180 and frame < 210 then
        inputState.a = true
    elseif frame >= 240 and frame < 270 then
        inputState.b = true
    elseif frame >= 300 and frame < 330 then
        inputState.start = true
    elseif frame >= 360 and frame < 390 then
        inputState.y = true
    elseif frame >= 420 and frame < 450 then
        inputState.x = true
    end

    if frame == 179 or frame == 181 or frame == 209 or frame == 241 or frame == 301 or frame == 361 or frame == 421 then
        log(string.format(
            "f=%d fc=%d room=%d set[a=%s b=%s start=%s y=%s x=%s] state=%02X lo=%02X",
            frame, rd16(ADDR_FRAME), rd16(ADDR_ROOM),
            tostring(inputState.a), tostring(inputState.b), tostring(inputState.start),
            tostring(inputState.y), tostring(inputState.x),
            rd8(ADDR_INPUT_STATE) or 0, rd8(ADDR_INPUT_LO) or 0))
    end

    if frame >= 520 then
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

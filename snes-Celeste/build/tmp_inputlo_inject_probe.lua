local testlib = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/scripts/mesen_testlib.lua")
local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_FrameCount",
    "GLOBAL_PlayerData",
    "GLOBAL_InputLo"
})

local outPath = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/inputlo_inject_probe_result.txt"
local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_INPUT_LO = sym.GLOBAL_InputLo

local frame = 0
local done = false
local lines = {}
local inputState = testlib.new_input_state()

local function rd8(a) return testlib.read_u8(a, cpu) or 0 end
local function rd16(a) return testlib.read_u16(a, cpu) or 0 end
local function wr8(a, v) emu.write(a, v & 0xFF, cpu) end
local function log(line)
    lines[#lines + 1] = line
    local f = io.open(outPath, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

testlib.install_input_callback(inputState)
log("START")

emu.addEventCallback(function()
    if frame >= 600 and frame < 720 and ((frame - 600) % 20) == 0 then
        wr8(ADDR_INPUT_LO, 0x10)
    end
end, emu.eventType.inputPolled)

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    testlib.set_pad(inputState, {})

    local room = rd8(ADDR_ROOM)
    if frame >= 120 and frame <= 360 then
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

    local fc = rd16(ADDR_FRAME)
    local px = rd16(ADDR_PLAYER + 0)
    local py = rd16(ADDR_PLAYER + 2)
    local inputLo = rd8(ADDR_INPUT_LO)

    if frame == 1 or (frame % 5) == 0 or room ~= 0 then
        log(string.format("f=%d fc=%d room=%d px=%d py=%d inputLo=%02X", frame, fc, room, px, py, inputLo))
    end

    if room >= 2 then
        done = true
        if emu.stop then
            emu.stop(0)
        end
        return
    end

    if frame >= 900 then
        done = true
        if emu.stop then
            emu.stop(1)
        end
    end
end, emu.eventType.endFrame)

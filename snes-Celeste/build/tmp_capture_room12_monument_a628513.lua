local testlib = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/scripts/mesen_testlib.lua")
testlib.projectRoot = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/_wt_a628513/snes-Celeste"
testlib.buildDir = testlib.projectRoot .. "/build"

local cpu = emu.memType.cpu
local sym = testlib.load_symbols({
    "GLOBAL_ActiveLevel",
    "GLOBAL_PlayerData",
    "GLOBAL_FrameCount",
    "GLOBAL_MonumentTextDisplayed",
    "GLOBAL_MonumentCurLineNum",
    "GLOBAL_MonumentCurLineCharCount",
    "s_monumentTextTick"
})

local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_monument_capture_a628513.txt"
local outIdlePng = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_monument_idle_a628513.png"
local outFullPng = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_monument_full_a628513.png"

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_MON_DISPLAYED = sym.GLOBAL_MonumentTextDisplayed
local ADDR_MON_LINE = sym.GLOBAL_MonumentCurLineNum
local ADDR_MON_CHAR = sym.GLOBAL_MonumentCurLineCharCount
local ADDR_MON_TICK = sym.s_monumentTextTick
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2

local TARGET_ROOM = 12
local KNOWN_MONUMENT_X = 56
local KNOWN_MONUMENT_Y = 32

local frame = 0
local done = false
local roomHitFrame = nil
local monumentStartFrame = nil
local monumentX = nil
local monumentY = nil
local idleShot = false
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
local function savePng(path)
    local png = emu.takeScreenshot()
    local f = io.open(path, "wb")
    if f then
        f:write(png)
        f:close()
    end
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
    local monDisplayed = rd8(ADDR_MON_DISPLAYED)
    local monLine = rd8(ADDR_MON_LINE)
    local monChar = rd8(ADDR_MON_CHAR)
    local monTick = rd8(ADDR_MON_TICK)

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
        if monumentX == nil then
            monumentX = KNOWN_MONUMENT_X
            monumentY = KNOWN_MONUMENT_Y
            log(string.format("MONUMENT x=%d y=%d", monumentX, monumentY))
        end

        if monumentX ~= nil then
            if not idleShot and (frame - roomHitFrame) >= 30 then
                wr16(PLAYER_X, monumentX - 40)
                wr16(PLAYER_Y, monumentY + 10)
                savePng(outIdlePng)
                idleShot = true
                log("SHOT_IDLE")
            else
                wr16(PLAYER_X, monumentX + 4)
                wr16(PLAYER_Y, monumentY + 8)
            end
        end

        if monumentStartFrame == nil and (monDisplayed ~= 0 or monLine ~= 0 or monChar ~= 0) then
            monumentStartFrame = frame
            log(string.format("TEXT_START frame=%d fc=%d line=%d char=%d tick=%d",
                frame, fc, monLine, monChar, monTick))
        end
    end

    if frame == 1 or (frame % 60) == 0 or room == TARGET_ROOM then
        log(string.format("f=%d fc=%d room=%d px=%d py=%d mon=%d line=%d char=%d tick=%d obj=%s,%s",
            frame, fc, room, rd16(PLAYER_X), rd16(PLAYER_Y),
            monDisplayed, monLine, monChar, monTick,
            tostring(monumentX), tostring(monumentY)))
    end

    if monumentStartFrame ~= nil and (frame - monumentStartFrame) >= 40 then
        savePng(outFullPng)
        log("SHOT_FULL")
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

local cpu = emu.memType.cpu
local outPath = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_to_room12_auto_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_REFRESH_FRAMES = 0x7E0222
local ADDR_GAMEPLAY_FRAMES = 0x7E0224
local ADDR_IN_TITLE = 0x7E022A
local ADDR_LAST_GAMEPLAY = 0x7E022B
local ADDR_MONUMENT_TICK = 0x7E0226
local ADDR_MONUMENT_LINE = 0x7E0227
local ADDR_MONUMENT_CHAR = 0x7E0228

local frame = 0
local lastFc = nil
local sameFc = 0
local done = false
local startFc = nil
local trace = {}

local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function write_result(text)
    local f = io.open(outPath, "w")
    if f then
        f:write(text)
        f:close()
    end
end

local function append_trace(line)
    trace[#trace + 1] = line
    write_result(table.concat(trace, "\n"))
end

local function rd8(addr)
    return emu.read(addr, cpu)
end

local function rd16(addr)
    local lo = rd8(addr)
    local hi = rd8(addr + 1)
    if lo == nil or hi == nil then
        return nil
    end
    return lo + hi * 256
end

local function clear_pad()
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
end

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

write_result("RUNNING")

emu.addEventCallback(function()
    if done then
        return
    end

    frame = frame + 1
    clear_pad()

    if frame >= 120 and frame <= 360 then
        local phase = frame % 30
        if phase < 10 then
            inputState.start = true
        elseif phase < 20 then
            inputState.a = true
        else
            inputState.b = true
        end
    end

    local fc = rd16(ADDR_FRAME)
    local room = rd16(ADDR_ROOM)
    local px = rd16(ADDR_PLAYER + 0)
    local py = rd16(ADDR_PLAYER + 2)
    local refreshFrames = rd16(ADDR_REFRESH_FRAMES)
    local gameplayFrames = rd16(ADDR_GAMEPLAY_FRAMES)
    local inTitle = rd8(ADDR_IN_TITLE)
    local lastGameplayFrame = rd16(ADDR_LAST_GAMEPLAY)
    local monumentTick = rd8(ADDR_MONUMENT_TICK)
    local monumentLine = rd8(ADDR_MONUMENT_LINE)
    local monumentChar = rd8(ADDR_MONUMENT_CHAR)

    if fc == nil or room == nil or px == nil or py == nil or refreshFrames == nil or gameplayFrames == nil or inTitle == nil or lastGameplayFrame == nil or monumentTick == nil or monumentLine == nil or monumentChar == nil then
        write_result("FAIL nil-read")
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if frame == 1 then
        startFc = fc
    end

    if frame == 1 or (frame % 60) == 0 then
        append_trace("TRACE frame=" .. frame .. " fc=" .. fc .. " room=" .. room ..
            " px=" .. px .. " py=" .. py .. " gpf=" .. gameplayFrames ..
            " ref=" .. refreshFrames .. " title=" .. inTitle ..
            " last=" .. lastGameplayFrame .. " tick=" .. monumentTick ..
            " line=" .. monumentLine .. " char=" .. monumentChar)
    end

    if lastFc == fc then
        sameFc = sameFc + 1
    else
        sameFc = 0
    end
    lastFc = fc

    if frame > 600 and sameFc >= 120 then
        write_result("FAIL hang frame=" .. frame .. " fc=" .. fc .. " room=" .. room .. " px=" .. px .. " py=" .. py)
        done = true
        if emu.stop then emu.stop(1) end
        return
    end

    if frame >= 900 then
        local advanced = false
        if startFc ~= nil then
            advanced = (((fc - startFc) % 65536) > 120)
        end
        if not advanced then
            append_trace("FAIL framecount-stalled start=" .. tostring(startFc) .. " cur=" .. tostring(fc) .. " room=" .. room)
            done = true
            if emu.stop then emu.stop(1) end
            return
        end
        append_trace("PASS frame=" .. frame .. " fc=" .. fc .. " room=" .. room .. " px=" .. px .. " py=" .. py ..
            " gpf=" .. gameplayFrames .. " ref=" .. refreshFrames ..
            " title=" .. inTitle .. " last=" .. lastGameplayFrame ..
            " tick=" .. monumentTick .. " line=" .. monumentLine .. " char=" .. monumentChar)
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

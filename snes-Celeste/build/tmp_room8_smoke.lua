local cpu = emu.memType.cpu
local outPath = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room8_smoke_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local ADDR_FRAME = sym.GLOBAL_FrameCount

local frame = 0
local done = false
local startFrameCount = nil

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

local function set_pad(tbl)
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
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

local function rd8(addr)
    return emu.read(addr, cpu)
end

local function rd16(addr)
    local lo = rd8(addr)
    local hi = rd8(addr + 1)
    if lo == nil or hi == nil then return nil end
    return lo + hi * 256
end

local function rs16(addr)
    local v = rd16(addr)
    if v == nil then return nil end
    if v >= 0x8000 then return v - 0x10000 end
    return v
end

write_result("RUNNING")

emu.addEventCallback(function()
    local ok, err = pcall(function()
        if done then return end

        frame = frame + 1

        if frame >= 120 and frame <= 360 then
            local phase = frame % 30
            if phase < 10 then
                set_pad({ start = true })
            elseif phase < 20 then
                set_pad({ a = true })
            else
                set_pad({})
            end
        else
            set_pad({})
        end

        local fc = rd16(ADDR_FRAME)
        local room = rd16(ADDR_ROOM)
        local px = rs16(ADDR_PLAYER)
        local py = rs16(ADDR_PLAYER + 2)
        if fc == nil or room == nil or px == nil or py == nil then
            write_result("FAIL nil-state frame=" .. tostring(frame))
            done = true
            if emu.stop then emu.stop(1) end
            return
        end

        if frame == 1 then
            startFrameCount = fc
        end

        if frame >= 420 then
            local delta = (fc - startFrameCount) % 65536
            if room ~= 8 then
                write_result("FAIL room=" .. tostring(room) .. " fc=" .. tostring(fc) .. " x=" .. tostring(px) .. " y=" .. tostring(py))
                done = true
                if emu.stop then emu.stop(1) end
                return
            end
            if delta < 120 then
                write_result("FAIL framecount-stalled start=" .. tostring(startFrameCount) .. " cur=" .. tostring(fc))
                done = true
                if emu.stop then emu.stop(1) end
                return
            end
            write_result("PASS frame=" .. tostring(frame) .. " room=" .. tostring(room) .. " fc=" .. tostring(fc) .. " x=" .. tostring(px) .. " y=" .. tostring(py))
            done = true
            if emu.stop then emu.stop(0) end
            return
        end

        if frame >= 900 then
            write_result("FAIL timeout room=" .. tostring(room) .. " fc=" .. tostring(fc))
            done = true
            if emu.stop then emu.stop(1) end
        end
    end)

    if not ok then
        write_result("FAIL lua-error " .. tostring(err))
        done = true
        if emu.stop then emu.stop(1) end
    end
end, emu.eventType.endFrame)

local outPng = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_capture.png"
local outTxt = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_capture_result.txt"
local frame = 0
local done = false
local lines = {}

local inputState = {
    up = false, down = false, left = false, right = false,
    select = false, start = false, a = false, b = false,
    x = false, y = false, l = false, r = false
}

local function flush()
    local f = io.open(outTxt, "w")
    if f then
        f:write(table.concat(lines, "\n") .. "\n")
        f:close()
    end
end

if emu.setInput then
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then pcall(function() emu.setInput(inputState, 1) end) end
    end, emu.eventType.inputPolled)
end

table.insert(lines, "START")
flush()

emu.addEventCallback(function()
    if done then return end
    frame = frame + 1

    if frame == 180 then
        local png = emu.takeScreenshot()
        local f = io.open(outPng, "wb")
        if f then
            f:write(png)
            f:close()
        end
        table.insert(lines, "SHOT")
        flush()
        done = true
        if emu.stop then emu.stop(0) end
    end
end, emu.eventType.endFrame)

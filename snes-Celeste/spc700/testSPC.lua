-- Mesen-S Lua script: Write test values to SPC700 I/O registers once per second

local SPC_IO_BASE = 0x2140
local counter = 0
local frameRate = 60   -- assume NTSC (use 50 if PAL)

-- Values will just increment each time
local value = 0

local function oncePerSecond()
    -- Write 4 incrementing values to $2140–$2143
    for i=0,3 do
        local addr = SPC_IO_BASE + i
        emu.write(addr, (value + i) & 0xFF, emu.memType.cpu)
    end

    print(string.format("Second tick: wrote %02X %02X %02X %02X",
        value & 0xFF, (value+1) & 0xFF, (value+2) & 0xFF, (value+3) & 0xFF))

    value = (value + 1) & 0xFF
end

local function onFrame()
    counter = counter + 1
    if counter >= frameRate then
        counter = 0
        oncePerSecond()
    end
end

emu.addEventCallback(onFrame, emu.eventType.endFrame)
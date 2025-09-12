-- Simple Debug Register Monitor for Mesen 2
-- Monitors debug registers and prints to console (jcc816 printhook style)

-- Debug register addresses (very end of WRAM)
local DEBUG_CHAR_ADDR = 0x7FFF00
local DEBUG_LINE_ADDR = 0x7FFF01  
local DEBUG_READY_ADDR = 0x7FFF02

-- State tracking
local last_ready = 0
local frame_count = 0
local text = ""  -- Accumulate characters like jcc816 printhook

-- Function to read a byte
local function read_byte(address)
    return emu.read(address, emu.memType.cpuMemory)
end

-- Function to write a byte
local function write_byte(address, value)
    emu.write(address, value, emu.memType.cpuMemory)
end

-- Function to read a character from debug register
local function read_char()
    local ready = read_byte(DEBUG_READY_ADDR)
    
    if ready == 1 then
        -- New data available
        local char_code = read_byte(DEBUG_CHAR_ADDR)
        local line_num = read_byte(DEBUG_LINE_ADDR)
        
        -- Convert to character
        local c = string.char(char_code & 0xFF)
        
        if c == '\0' then
            -- Null terminator - log accumulated text and reset
            if text ~= "" then
                emu.log(text)
            end
            text = ""
        else
            -- Accumulate character
            text = text .. c
        end
        
        -- Clear ready flag
        write_byte(DEBUG_READY_ADDR, 0)
    end
    
    last_ready = ready
end

-- Main update function
local function on_frame()
    frame_count = frame_count + 1
    
    -- Check for new data every frame for better responsiveness
    read_char()
end

-- Register callback
emu.addEventCallback(on_frame, emu.eventType.endFrame)

emu.log("Simple Debug Register Monitor Loaded")
emu.log("Monitoring addresses 0x7FFF00-0x7FFF02")

local M = {}

M.projectRoot = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste"
M.buildDir = M.projectRoot .. "/build"
M.cpuWramMin = 0x7E0000
M.cpuWramMax = 0x7FFFFF

function M.make_result_writer(filename)
    local outPath = M.buildDir .. "/" .. filename
    local function write_result(text)
        local f = io.open(outPath, "w")
        if f then
            f:write(text)
            f:close()
        end
    end
    return outPath, write_result
end

function M.load_symbols(requiredNames)
    local sym = dofile(M.buildDir .. "/mesen_symbols.lua")
    if type(sym) ~= "table" then
        error("mesen_symbols.lua did not return a table")
    end
    if sym.WRAM_BASE ~= M.cpuWramMin then
        error(string.format("Unexpected WRAM base: got 0x%06X", sym.WRAM_BASE or 0))
    end
    if requiredNames ~= nil then
        for _, name in ipairs(requiredNames) do
            local addr = sym[name]
            if type(addr) ~= "number" then
                error("Missing required symbol: " .. tostring(name))
            end
            if addr < M.cpuWramMin or addr > M.cpuWramMax then
                error(string.format("Symbol %s is not in CPU WRAM: 0x%06X", name, addr))
            end
        end
    end
    return sym
end

function M.new_input_state()
    return {
        up = false, down = false, left = false, right = false,
        select = false, start = false, a = false, b = false,
        x = false, y = false, l = false, r = false
    }
end

function M.set_pad(inputState, tbl)
    for k, _ in pairs(inputState) do
        inputState[k] = false
    end
    for k, v in pairs(tbl) do
        inputState[string.lower(k)] = v and true or false
    end
end

function M.install_input_callback(inputState)
    if not emu.setInput then
        return
    end
    emu.addEventCallback(function()
        local ok = pcall(function() emu.setInput(inputState, 0) end)
        if not ok then
            pcall(function() emu.setInput(inputState, 1) end)
        end
    end, emu.eventType.inputPolled)
end

function M.read_u8(addr, memType)
    return emu.read(addr, memType)
end

function M.read_u16(addr, memType)
    local lo = M.read_u8(addr, memType)
    local hi = M.read_u8(addr + 1, memType)
    if lo == nil or hi == nil then
        return nil
    end
    return lo + hi * 256
end

function M.read_s16(addr, memType)
    local value = M.read_u16(addr, memType)
    if value == nil then
        return nil
    end
    if value >= 0x8000 then
        return value - 0x10000
    end
    return value
end

return M

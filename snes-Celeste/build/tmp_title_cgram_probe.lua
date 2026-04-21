local out = 'C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/title_cgram_probe_result.txt'
local cgram = emu.memType.snesCgRam
local lines = {}
local function r(a) return emu.read(a,cgram) or 0 end
local function flush() local f=io.open(out,'w'); if f then f:write(table.concat(lines,'\n')..'\n'); f:close(); end end
emu.addEventCallback(function()
  local vals={}
  for i=0,15 do table.insert(vals, string.format('%02X%02X', r(0x40+i*2+1), r(0x40+i*2))) end
  table.insert(lines, table.concat(vals,' '))
  flush()
  if emu.stop then emu.stop(0) end
end, emu.eventType.endFrame)

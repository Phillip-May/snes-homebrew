local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_memtypes.txt'
local f=io.open(out,'w')
if f then
  if emu.memType then
    for k,v in pairs(emu.memType) do
      f:write(tostring(k)..'='..tostring(v)..'\n')
    end
  else
    f:write('no emu.memType\n')
  end
  f:close()
end
if emu.stop then emu.stop(0) end

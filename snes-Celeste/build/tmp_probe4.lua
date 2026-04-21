local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/tmp_probe4_result.txt'
local sm=emu.memType.snesMemory
local frame=0
local function r8(a) return emu.read(a,sm) or 0 end
local function append(t) local f=io.open(out,'a'); if f then f:write(t..'\n'); f:close(); end end
if io.open(out,'r') then os.remove(out) end
emu.addEventCallback(function()
 frame=frame+1
 if frame==120 then
  append('m='..r8(0x7E025F)..','..r8(0x7E157D)..','..r8(0x7E157E))
  if emu.stop then emu.stop(0) end
 end
end, emu.eventType.endFrame)
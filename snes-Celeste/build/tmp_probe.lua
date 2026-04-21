local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/tmp_probe_result.txt'
local sym=dofile('C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua')
local cpu=emu.memType.cpu
local frame=0
local function r16(a) local lo=emu.read(a,cpu); local hi=emu.read(a+1,cpu); return (lo or 0)+(hi or 0)*256 end
local function append(t) local f=io.open(out,'a'); if f then f:write(t..'\n'); f:close(); end end
if io.open(out,'r') then os.remove(out) end
emu.addEventCallback(function()
 frame=frame+1
 if frame==1 or frame==120 or frame==240 or frame==900 then
  append('f='..frame..' fc='..r16(sym.GLOBAL_FrameCount)..' room='..r16(sym.GLOBAL_ActiveLevel))
 end
 if frame==900 then if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)
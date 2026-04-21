local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_smoke_probe.txt'
local frame=0
local done=false
local function wr(t)
 local f=io.open(out,'w'); if f then f:write(t); f:close(); end
end
wr('RUNNING')
emu.addEventCallback(function()
 if done then return end
 frame=frame+1
 if frame>=120 then
   wr('PASS frame='..frame)
   done=true
   if emu.stop then emu.stop(0) end
 end
end, emu.eventType.endFrame)

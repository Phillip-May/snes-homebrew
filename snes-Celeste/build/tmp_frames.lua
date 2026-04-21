local out='C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/tmp_frames_result.txt'
local frame=0
local function wr(t) local f=io.open(out,'w'); if f then f:write(t); f:close(); end end
wr('RUNNING')
emu.addEventCallback(function()
 frame=frame+1
 if frame==900 then wr('PASS frame='..frame); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)
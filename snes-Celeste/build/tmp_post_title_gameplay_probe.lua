local cpu = emu.memType.cpu
local out = 'C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/post_title_gameplay_probe.txt'
local frame = 0
local lines = {}
local inputState = {up=false,down=false,left=false,right=false,select=false,start=false,a=false,b=false,x=false,y=false,l=false,r=false}
local function rd8(a) return emu.read(a,cpu) or 0 end
local function rd16(a) local lo=rd8(a); local hi=rd8(a+1); return lo+hi*256 end
local function rs16(a) local v=rd16(a); if v>=0x8000 then return v-0x10000 else return v end end
local function flush() local f=io.open(out,'w'); if f then f:write(table.concat(lines,'\n')..'\n'); f:close() end end
local function log(s) lines[#lines+1]=s; flush() end
if emu.setInput then emu.addEventCallback(function() local ok=pcall(function() emu.setInput(inputState,0) end); if not ok then pcall(function() emu.setInput(inputState,1) end) end end, emu.eventType.inputPolled) end
emu.addEventCallback(function()
  frame = frame + 1
  for k,_ in pairs(inputState) do inputState[k]=false end
  if frame >= 240 and frame <= 360 then inputState.a = true end
  if frame >= 430 and frame <= 520 then inputState.right = true end
  if frame >= 560 and frame <= 620 then inputState.b = true end
  local room = rd16(0x7E0285)
  if frame == 1 or frame % 30 == 0 or room == 1 then
    log(string.format('f=%d fc=%d room=%d timerF=%d timerS=%d freeze=%d input=%02X px=%d py=%d bg4x=%d bg4y=%d', frame, rd16(0x7E1488), room, rd8(0x7E021E), rd8(0x7E021F), rd8(0x7E0215), rd8(0x7E0214), rs16(0x7E148E), rs16(0x7E1490), rd16(0x7E0277), rd16(0x7E0279)))
  end
  if frame >= 760 then if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

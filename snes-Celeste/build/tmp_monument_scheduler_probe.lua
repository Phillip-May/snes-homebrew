local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/monument_scheduler_probe.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")
local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2
local ADDR_MON = 0x7E0222
local ADDR_TICK = 0x7E0223
local ADDR_LINE = 0x7E0224
local ADDR_CHAR = 0x7E0225
local ADDR_LAST_GAMEPLAY = 0x7E0228
local frame=0
local lines={}
local inputState={ up=false,down=false,left=false,right=false,select=false,start=false,a=false,b=false,x=false,y=false,l=false,r=false }
local function rd8(a) return emu.read(a,cpu) end
local function rd16(a) local lo=rd8(a) local hi=rd8(a+1) return lo+hi*256 end
local function wr8(a,v) emu.write(a, v & 0xFF, cpu) end
local function wr16(a,v) wr8(a,v) wr8(a+1, math.floor(v/256)) end
local function clearPad() for k,_ in pairs(inputState) do inputState[k]=false end end
local function flush() local f=io.open(out,'w') if f then f:write(table.concat(lines,'\n')..'\n') f:close() end end
if emu.setInput then emu.addEventCallback(function() local ok=pcall(function() emu.setInput(inputState,0) end) if not ok then pcall(function() emu.setInput(inputState,1) end) end end, emu.eventType.inputPolled) end
emu.addEventCallback(function()
  frame=frame+1
  clearPad()
  if frame >= 120 and frame <= 360 then local phase=frame%30 if phase<10 then inputState.start=true elseif phase<20 then inputState.a=true end elseif frame >= 390 and frame <= 540 then if (frame%20)<10 then inputState.b=true end end
  local room=rd16(ADDR_ROOM)
  if room==12 then wr16(PLAYER_X,60) wr16(PLAYER_Y,40) end
  if room==12 and frame >= 560 and frame <= 640 then
    lines[#lines+1]=string.format('f=%d fc=%d last=%d mon=%d tick=%d line=%d char=%d', frame, rd16(ADDR_FRAME), rd16(ADDR_LAST_GAMEPLAY), rd8(ADDR_MON), rd8(ADDR_TICK), rd8(ADDR_LINE), rd8(ADDR_CHAR))
    if (frame % 10)==0 then flush() end
  end
  if frame >= 640 then flush(); if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

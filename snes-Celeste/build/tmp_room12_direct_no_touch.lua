local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_direct_no_touch_result.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")
local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local ADDR_PLAYER = sym.GLOBAL_PlayerData
local PLAYER_X = ADDR_PLAYER + 0
local PLAYER_Y = ADDR_PLAYER + 2
local frame = 0
local done = false
local lastFc = nil
local sameFc = 0
local lines = {}
local inputState = { up=false,down=false,left=false,right=false,select=false,start=false,a=false,b=false,x=false,y=false,l=false,r=false }
local function rd8(a) return emu.read(a,cpu) end
local function rd16(a) local lo=rd8(a) local hi=rd8(a+1) return lo + hi*256 end
local function clearPad() for k,_ in pairs(inputState) do inputState[k]=false end end
local function log(line) table.insert(lines,line) end
local function flush() local f=io.open(out,"w") if f then f:write(table.concat(lines,"\n").."\n") f:close() end end
if emu.setInput then emu.addEventCallback(function() local ok=pcall(function() emu.setInput(inputState,0) end) if not ok then pcall(function() emu.setInput(inputState,1) end) end end, emu.eventType.inputPolled) end
log("START") flush()
emu.addEventCallback(function()
 if done then return end
 frame=frame+1
 clearPad()
 if frame >= 120 and frame <= 360 then local phase=frame%30 if phase<10 then inputState.start=true elseif phase<20 then inputState.a=true end elseif frame >= 390 and frame <= 540 then if (frame%20)<10 then inputState.b=true end end
 local room=rd16(ADDR_ROOM)
 local fc=rd16(ADDR_FRAME)
 if lastFc == fc then sameFc=sameFc+1 else sameFc=0 end
 lastFc=fc
 if frame==1 or (frame%30)==0 or room==12 then log(string.format("f=%d fc=%d room=%d same=%d px=%d py=%d", frame, fc, room, sameFc, rd16(PLAYER_X), rd16(PLAYER_Y))) flush() end
 if frame >= 1800 then log("DONE") flush() done=true if emu.stop then emu.stop(0) end end
end, emu.eventType.endFrame)

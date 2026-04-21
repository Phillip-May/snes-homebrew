local cpu = emu.memType.cpu
local out = "C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/room12_probe_fresh.txt"
local sym = dofile("C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/build/mesen_symbols.lua")

local ADDR_ROOM = sym.GLOBAL_ActiveLevel
local ADDR_FRAME = sym.GLOBAL_FrameCount
local frame = 0
local lines = {}
local inputState = {
    up=false,down=false,left=false,right=false,
    select=false,start=false,a=false,b=false,x=false,y=false,l=false,r=false
}

local function rd8(a) return emu.read(a, cpu) end
local function rd16(a) return rd8(a) + rd8(a + 1) * 256 end
local function log(s)
  lines[#lines+1]=s
  local f=io.open(out,"w")
  if f then f:write(table.concat(lines,"\n").."\n"); f:close() end
end

if emu.setInput then
  emu.addEventCallback(function()
    local ok = pcall(function() emu.setInput(inputState, 0) end)
    if not ok then pcall(function() emu.setInput(inputState, 1) end) end
  end, emu.eventType.inputPolled)
end

emu.addEventCallback(function()
  frame = frame + 1
  for k,_ in pairs(inputState) do inputState[k]=false end
  if frame >= 120 and frame <= 360 then
    local phase = frame % 30
    if phase < 10 then inputState.start = true
    elseif phase < 20 then inputState.a = true end
  end
  local room = rd16(ADDR_ROOM)
  local fc = rd16(ADDR_FRAME)
  if frame == 1 or frame % 30 == 0 or room ~= 0 then
    log(string.format("f=%d fc=%d room=%d", frame, fc, room))
  end
  if room == 12 then
    log("DONE")
    if emu.stop then emu.stop(0) end
  elseif frame > 600 then
    log("TIMEOUT")
    if emu.stop then emu.stop(1) end
  end
end, emu.eventType.endFrame)

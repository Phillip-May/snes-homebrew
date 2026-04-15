local spcDsp = emu.memType.spcDspRegisters
local frame = 0
local sawPlaying = false
local outPath = 'C:/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700/testrom/stop_verify_result.txt'

local function setPad(tbl)
  if emu.setInput then emu.setInput(1, tbl) end
end

local function writeResult(s)
  local f = io.open(outPath, 'w')
  if f then f:write(s); f:close() end
end

local function allVoicesMuted()
  for v=0,4 do
    local base = v * 0x10
    local l = emu.read(base + 0x00, spcDsp)
    local r = emu.read(base + 0x01, spcDsp)
    if l ~= 0 or r ~= 0 then
      return false
    end
  end
  return true
end

writeResult('RUNNING')

emu.addEventCallback(function()
  frame = frame + 1

  if frame == 120 then
    setPad({ A = true })
  elseif frame == 122 then
    setPad({})
  elseif frame == 180 then
    for v=0,3 do
      local base = v * 0x10
      local l = emu.read(base + 0x00, spcDsp)
      local r = emu.read(base + 0x01, spcDsp)
      if l ~= 0 or r ~= 0 then sawPlaying = true end
    end
    setPad({ X = true, Y = true })
  elseif frame == 182 then
    setPad({})
  elseif frame == 230 then
    local muted = allVoicesMuted()
    if sawPlaying and muted then
      writeResult('PASS sawPlaying=true muted=true')
      if emu.stop then emu.stop(0) end
    else
      writeResult('FAIL sawPlaying=' .. tostring(sawPlaying) .. ' muted=' .. tostring(muted))
      if emu.stop then emu.stop(1) end
    end
  elseif frame > 600 then
    writeResult('TIMEOUT')
    if emu.stop then emu.stop(2) end
  end
end, emu.eventType.endFrame)

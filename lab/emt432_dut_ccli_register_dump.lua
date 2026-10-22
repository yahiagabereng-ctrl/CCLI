#!/usr/bin/env lua
-- OpenWrt TG544: Modbus TCP via nc(1). One SSH: emt432_dump_registers_one_ssh.ps1

local host = os.getenv("EMT432_HOST") or "192.168.178.10"
local port = os.getenv("EMT432_PORT") or "502"
local unit = tonumber(os.getenv("EMT432_UNIT") or "1") or 1
local word_swap = (os.getenv("EMT432_WORD_SWAP") or "1") ~= "0"
local pause_ms = tonumber(os.getenv("EMT432_PAUSE_MS") or "80") or 80

local function hex2(n)
  return string.format("\\x%02x", n % 256)
end

local tid = 0
local function modbus_pause()
  if pause_ms > 0 then
    os.execute(string.format("sleep %.3f 2>/dev/null", pause_ms / 1000.0))
  end
end

local function read_holding_once(offset, count)
  tid = (tid + 1) % 65536
  local pdu = string.char(
    unit, 0x03,
    math.floor(offset / 256) % 256, offset % 256,
    math.floor(count / 256) % 256, count % 256
  )
  local mbap = string.char(
    math.floor(tid / 256) % 256, tid % 256,
    0, 0,
    math.floor(#pdu / 256) % 256, #pdu % 256
  )
  local bin = mbap .. pdu
  local hex = ""
  for i = 1, #bin do
    hex = hex .. hex2(bin:byte(i))
  end
  local tmp = "/tmp/emt432_mb_rsp"
  local cmd = string.format(
    "sh -c \"printf '%s' | nc %s %s 2>/dev/null | hexdump -C > %s\"",
    hex, host, port, tmp
  )
  os.execute(cmd)
  local f = io.open(tmp, "r")
  if not f then
    return nil
  end
  local bytes = {}
  for line in f:lines() do
    if line:find("|") then
      local left = line:sub(1, line:find("|") - 1)
      local hexpart = left:gsub("^[%x]+%s+", "")
      for hx in hexpart:gmatch("%x%x") do
        bytes[#bytes + 1] = tonumber(hx, 16)
      end
    end
  end
  f:close()
  if #bytes < 9 then
    return nil
  end
  local uid = bytes[7]
  local fc = bytes[8]
  local bc = bytes[9]
  if not uid or not fc or not bc then
    return nil
  end
  if uid ~= unit or fc >= 0x80 or fc ~= 0x03 then
    return nil
  end
  local regs = {}
  local nreg = math.floor(bc / 2)
  if nreg ~= count then
    return nil
  end
  for i = 1, nreg do
    local p = 9 + (i - 1) * 2
    local hi, lo = bytes[p + 1], bytes[p + 2]
    if not hi or not lo then
      return nil
    end
    regs[i] = hi * 256 + lo
  end
  return regs
end

local function read_holding(offset, count, tries)
  tries = tries or 4
  for _ = 1, tries do
    modbus_pause()
    local regs = read_holding_once(offset, count)
    if regs then
      return regs
    end
  end
  return nil
end

local function merge_regs(a, b)
  if not a then
    return b
  end
  if not b then
    return a
  end
  local out = {}
  for i = 1, #a do
    out[i] = a[i]
  end
  local base = #a
  for i = 1, #b do
    out[base + i] = b[i]
  end
  return out
end

local function read_holding_chunked(offset, total, chunk)
  chunk = chunk or 20
  local all = {}
  local pos = 0
  while pos < total do
    local n = math.min(chunk, total - pos)
    local regs = read_holding(offset + pos, n)
    if not regs then
      return nil
    end
    for i = 1, #regs do
      all[pos + i] = regs[i]
    end
    pos = pos + n
  end
  return all
end

local function f32_from_u16(r0, r1)
  if word_swap then
    r0, r1 = r1, r0
  end
  local b1 = math.floor(r0 / 256) % 256
  local b2 = r0 % 256
  local b3 = math.floor(r1 / 256) % 256
  local b4 = r1 % 256
  local sign = (b1 >= 128) and -1 or 1
  b1 = b1 % 128
  local exp = b1 * 2 + math.floor(b2 / 128)
  local mant = ((b2 % 128) * 256 + b3) * 256 + b4
  if exp == 0 and mant == 0 then
    return 0.0
  end
  if exp == 255 then
    return mant == 0 and (sign * (1 / 0)) or (0 / 0)
  end
  return sign * (1 + mant / 8388608) * (2 ^ (exp - 127))
end

local function f32_at(regs, idx)
  local r0, r1 = regs[idx], regs[idx + 1]
  if not r0 or not r1 then
    return nil
  end
  return f32_from_u16(r0, r1)
end

local function read_f32_at(offset)
  local regs = read_holding(offset, 2)
  if not regs then
    return nil
  end
  return f32_at(regs, 1), regs[1], regs[2]
end

local function i64_at(regs, idx)
  local v = 0
  for i = 0, 3 do
    v = v * 65536 + (regs[idx + i] % 65536)
  end
  if v >= 2 ^ 63 then
    v = v - 2 ^ 64
  end
  return v
end

local function line(fmt, ...)
  io.write(string.format(fmt .. "\n", ...))
end

io.write(string.format(
  "# EMT432 live dump (DUT -> %s:%s unit=%d word_swap=%s pause_ms=%d)\n\n",
  host, port, unit, tostring(word_swap), pause_ms
))

if not read_holding(10, 1) then
  io.stderr:write("FAIL: Modbus read 40011 — check LAN3 to " .. host .. "\n")
  os.exit(1)
end

line("## Config / commissioning")
line("%-6s %-6s %-28s %s", "PLC", "off", "name", "value")

local config = {
  {40011, 10, "mounted_version", "u16"},
  {40232, 231, "Measurement_Config", "u16"},
  {40234, 233, "TA_Chronos_register", "u16"},
  {40235, 234, "TA_Transducer_ratio", "f32"},
  {40237, 236, "TA_Transducer_delay", "f32"},
  {40239, 238, "TV_Transducer_ratio", "f32"},
  {40241, 240, "TV_Transducer_delay", "f32"},
  {40243, 242, "minimum_voltage_ripple", "f32"},
  {40245, 244, "minimum_current_ripple", "f32"},
  {40247, 246, "minimum_power_ripple", "f32"},
  {40256, 255, "Energy_unit_factor", "u16"},
}

for _, row in ipairs(config) do
  local plc, off, name, typ = row[1], row[2], row[3], row[4]
  local cnt = (typ == "f32") and 2 or 1
  local regs = read_holding(off, cnt)
  if not regs then
    line("%-6d %-6d %-28s READ_FAIL", plc, off, name)
  elseif typ == "u16" then
    line("%-6d %-6d %-28s %d (0x%x)", plc, off, name, regs[1], regs[1])
  else
    local val = f32_at(regs, 1)
    line(
      "%-6d %-6d %-28s %.9g  [raw u16: %d, %d]",
      plc, off, name, val, regs[1], regs[2]
    )
  end
end

io.write("\n## Energy int64 [Wh/10]\n")
local energy = {
  {40716, 715, "KWh1_rev2", "CCLI pin; may N/A on ITA Rev0"},
  {40721, 720, "KWh1_rev0", "ITA map primary"},
  {40729, 728, "KWh_SUM", ""},
  {40765, 764, "KVARh1", ""},
  {40845, 844, "KVAh1", ""},
}
for _, row in ipairs(energy) do
  local plc, off, name, note = row[1], row[2], row[3], row[4]
  local regs = read_holding(off, 4)
  if not regs then
    line("%-6d %-6d %-20s READ_FAIL  %s", plc, off, name, note)
  else
    local raw = i64_at(regs, 1)
    line(
      "%-6d %-6d %-20s raw=%d  kWh=%.6f  %s",
      plc, off, name, raw, raw / 10000.0, note
    )
  end
end

io.write("\n## CCLI poll FC3 block (offset 906 count 36)\n")
local poll = read_holding_chunked(906, 36, 18)
local poll_signals = {
  {906, "V_L1_N"},
  {922, "I_L1"},
  {932, "P1"},
  {940, "Q1"},
}
if not poll then
  io.write("READ_FAIL entire block\n")
else
  for _, sig in ipairs(poll_signals) do
    local off, nm = sig[1], sig[2]
    local idx = off - 906 + 1
    local val = f32_at(poll, idx)
    if val == nil then
      line("%-6d %-6d %-16s READ_FAIL", 40001 + off, off, nm)
    else
      line("%-6d %-6d %-16s %.6g", 40001 + off, off, nm, val)
    end
  end
end

io.write("\n## Realtime RMS 40907-40985 (hr:906 # 80)\n")
local rms_names = {
  {906, "V_L1_N"}, {908, "V_L2_N"}, {910, "V_L3_N"}, {912, "V_STAR_AVG"},
  {914, "V_L1_L2"}, {916, "V_L2_L3"}, {918, "V_L3_L1"}, {920, "V_LINE_AVG"},
  {922, "I_L1"}, {924, "I_L2"}, {926, "I_L3"}, {928, "I_N"}, {930, "I_AVG"},
  {932, "P1"}, {934, "P2"}, {936, "P3"}, {938, "P_SUM"},
  {940, "Q1"}, {942, "Q2"}, {944, "Q3"}, {946, "Q_SUM"},
  {948, "S1"}, {950, "S2"}, {952, "S3"}, {954, "S_SUM"},
  {956, "PF1"}, {958, "PF2"}, {960, "PF3"}, {962, "PF_3PH"},
  {964, "CF1"}, {966, "CF2"}, {968, "CF3"}, {970, "CF_N"},
  {972, "Frequency"},
  {974, "V_L1_N_peak"}, {976, "V_L2_N_peak"}, {978, "V_L3_N_peak"},
  {980, "V_L1_L2_peak"}, {982, "V_L2_L3_peak"}, {984, "V_L3_L1_peak"},
}

local block = read_holding_chunked(906, 80, 20)
local used_fallback = false
if not block then
  used_fallback = true
  io.write("# note: chunked 80-reg read failed; per-signal FC3 x2\n")
end

for _, sig in ipairs(rms_names) do
  local off, nm = sig[1], sig[2]
  local val
  if block then
    local idx = off - 906 + 1
    val = f32_at(block, idx)
  end
  if val == nil then
    used_fallback = true
    val = read_f32_at(off)
  end
  if val == nil then
    line("%-6d %-6d %-16s READ_FAIL", 40001 + off, off, nm)
  else
    line("%-6d %-6d %-16s %.6g", 40001 + off, off, nm, val)
  end
end

if used_fallback then
  io.write("# RMS: some values read via per-register FC3 (slower, same as CCLI floats)\n")
end

local cmd = read_holding(905, 1)
io.write("\n## Reference\n")
if cmd then
  line("40906 Command (off 905): %d", cmd[1])
else
  io.write("40906 Command: READ_FAIL\n")
end
io.write("\n# DONE\n")

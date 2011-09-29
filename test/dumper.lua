local function keys(t)
  local res = {}
  local oktypes = { stringstring = true, numbernumber = true }
  local function cmpfct(a,b)
    if oktypes[type(a)..type(b)] then
      return a < b
    else
      return type(a) < type(b)
    end
  end
  for k in pairs(t) do
    res[#res+1] = k
  end
  table.sort(res, cmpfct)
  return res
end

function DataDumper(value)
  local dumplua
  local keycache, out = {}, {}
  function quote(value)
    value = value:gsub("([%z\1-\31])([0-9])", function(a,b) return ("\\%03d%s"):format(a:byte(),b) end)
	value = value:gsub("([%z\1-\31])", function(a) return ("\\%d"):format(a:byte()) end)
    return '"'..value..'"'
  end
  local fcts = {
    string = function(value) return quote(value) end,
    number = function(value) return value end,
    boolean = function(value) return tostring(value) end,
    ['nil'] = function(value) return 'nil' end,
    ['function'] = function() return "[function]" end,
    userdata = function() return "[userdata]" end,
    thread = function() return "[thread]" end,
  }
  local function make_key(t, key)
    local s
    if type(key) == 'string' and key:match('^[_%a][_%w]*$') then
      s = key .. "="
    else
      s = "[" .. dumplua(key) .. "]="
    end
    t[key] = s
    return s
  end
  fcts.table = function (value)
    -- Table value
    local numidx = 1
    out[#out+1] = "{"
    for _,key in pairs(keys(value)) do
	  local val = value[key]
      if key == numidx then
        numidx = numidx + 1
      else
        out[#out+1] = keycache[key]
      end
      local str = dumplua(val)
      out[#out+1] = str..","
    end
    if string.sub(out[#out], -1) == "," then
      out[#out] = string.sub(out[#out], 1, -2);
    end
    out[#out+1] = "}"
    return "" 
  end
  function dumplua(value)
    return fcts[type(value)](value)
  end
    setmetatable(keycache, {__index = make_key })
    table.insert(out,dumplua(value))
    return table.concat(out)
end

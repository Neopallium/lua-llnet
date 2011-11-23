local llnet = require"llnet"

local tcp = llnet.Protocols.tcp
print("llnet.Protocols.tcp = ", tcp)
print("llnet.Protocols[" .. tcp .. "] = ", llnet.Protocols[tcp])


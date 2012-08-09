
local llnet = require"llnet"
local P = llnet.Protocols

local host, port, family, socktype, protocol = arg[1], arg[2], arg[3], arg[4], arg[5]

if #arg < 1 then
	print(arg[0], ": <host> [<port> [<family> [<socktype> [<protocol>]]]]")
	return
end

if family then
	family = family:upper()
	family = llnet[family] or llnet['AF_' .. family] or 0
end

if socktype then
	socktype = socktype:upper()
	socktype = llnet[socktype] or llnet['SOCK_' .. socktype] or 0
end

if protocol then
	protocol = P[protocol]
end

local addr = llnet.LSockAddr()
local stat, err = addr:lookup_full(host, port, family, socktype, protocol)

if stat then
	print("addr:", addr)
else
	print("getaddrinfo returned an error:", err, llnet.EAI_Errors:description(err))
end


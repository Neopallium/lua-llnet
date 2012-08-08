
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

local info, err = llnet.LAddrInfo.full(host, port, family, socktype, protocol)
local addr = llnet.LSockAddr()

if info and info:first() then
	repeat
		local canon = info:get_canonname()
		if canon then
			print("canon name:", canon)
		end
		info:get_addr(addr)
		print("addr:", addr)
		print("info:", info:get_family(), info:get_socktype(), P[info:get_protocol()])
	until not info:next()
elseif info then
	print("Zero results.")
else
	print("getaddrinfo returned an error:", err, llnet.EAI_Errors:description(err))
end


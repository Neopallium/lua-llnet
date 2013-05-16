local llnet = require"llnet"

local sock_type = llnet["SOCK_" .. (arg[1] or "stream"):upper()]

local sock1, sock2, err = llnet.socketpair(sock_type, llnet.SOCK_CLOEXEC)
print("pair: ", sock1, sock2, err)

local addr = llnet.LSockAddr()
print(sock1:get_sockname(addr))
print("sock1 sockname: ", addr)
print(sock2:get_sockname(addr))
print("sock2 sockname: ", addr)

print("send sock1: ", sock1:send("Hello"))
print("recv sock2: ", sock2:recv(10))

--print("close: ", sock1:close(), sock2:close())


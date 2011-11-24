local llnet = require"llnet"

local sock = llnet.LSocketFD(llnet.AF_INET, llnet.SOCK_STREAM, 0, 0)

print("recv on non-connected socket:", sock:recv(10))
--print("send on non-connected socket:", sock:send("Hello"))


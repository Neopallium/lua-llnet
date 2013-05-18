local llnet = require"llnet"

local sock = llnet.LSocket(llnet.AF_INET, llnet.SOCK_STREAM, 0, 0)

print("recv on non-connected socket:", sock:recv(10))
local addr = llnet.LSockAddr.unix("/tmp/.non_existent_file")
print("connect error:", sock:connect(addr))
--print("send on non-connected socket:", sock:send("Hello"))


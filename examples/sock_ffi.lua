-- Copyright (c) 2010-2011 by Robert G. Jakabosky <bobby@neoawareness.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

local setmetatable = setmetatable
local print = print
local assert = assert

local ffi = require"ffi"

ffi.cdef[[
/* LSockAddr */

typedef int socklen_t;
typedef int sa_family_t;

typedef struct LSockAddr LSockAddr;

int l_sockaddr_init_ip_port(LSockAddr *addr, const char *ip, int port);

int l_sockaddr_init_unix(LSockAddr *addr, const char *path);

int l_sockaddr_init_family(LSockAddr *addr, sa_family_t family);

int l_sockaddr_init_len(LSockAddr *addr, socklen_t addrlen);

void l_sockaddr_clear(LSockAddr *addr);

void l_sockaddr_resize(LSockAddr *addr, socklen_t addrlen);

void l_sockaddr_fill(LSockAddr *addr, const struct sockaddr *saddr, socklen_t addrlen);

sa_family_t l_sockaddr_get_family(LSockAddr *addr);

struct sockaddr *l_sockaddr_get_addr(LSockAddr *addr);

socklen_t l_sockaddr_get_addrlen(LSockAddr *addr);

/* LSocketFD */
typedef int LSocketFD;

int l_socket_set_nonblock(LSocketFD sock, bool val);

int l_socket_set_close_on_exec(LSocketFD sock, bool val);

int l_socket_get_option(LSocketFD sock, int level, int opt, void *val, socklen_t *len);

int l_socket_set_option(LSocketFD sock, int level, int opt, const void *val, socklen_t len);

int l_socket_get_int_option(LSocketFD sock, int level, int opt, int *val);

int l_socket_set_int_option(LSocketFD sock, int level, int opt, int val);

LSocketFD l_socket_open(int domain, int type, int protocol, int flags);

LSocketFD l_socket_close_internal(LSocketFD sock);

int l_socket_shutdown(LSocketFD sock, int how);

int l_socket_connect(LSocketFD sock, LSockAddr *addr);

int l_socket_bind(LSocketFD sock, LSockAddr *addr);

int l_socket_listen(LSocketFD sock, int backlog);

LSocketFD l_socket_accept(LSocketFD sock, LSockAddr *peer, int flags);

int l_socket_send(LSocketFD sock, const void *buf, size_t len, int flags);

int l_socket_recv(LSocketFD sock, void *buf, size_t len, int flags);

LSocketFD l_socket_open(int domain, int type, int protocol, int flags);

]]
local C = ffi.load("./llnet.so")

local llnet = require"llnet"
local new_socket = llnet.LSocketFD
local Errors = llnet.Errors
local AF_UNIX = llnet.AF_UNIX
local AF_INET = llnet.AF_INET
local AF_INET6 = llnet.AF_INET6

local Options = llnet.Options
local SOL_SOCKET = Options.SOL_SOCKET

local Families = setmetatable({}, {
__index = function(tab, family)
	local name = "AF_" .. family:upper()
	local id = llnet[name]
	rawset(tab, family, id)
	return id
end,
})

local Sock_types = setmetatable({}, {
__index = function(tab, sock_type)
	local name = "SOCK_" .. sock_type:upper()
	local id = llnet[name]
	rawset(tab, sock_type, id)
	return id
end,
})

local level_opts_mt = {
__index = function(tab, opt)
	local name = tab._prefix .. opt:upper()
	local id = Options[name]
	rawset(tab, opt, id)
	return id
end,
}
local function new_level_opts(level, prefix)
	return setmetatable({
		_level = level,
		_prefix = prefix,
	}, level_opts_mt)
end

local Levels = setmetatable({}, {
__index = function(tab, name)
	local level
	local prefix
	if name == 'socket' then
		level = Options.SOL_SOCKET
		prefix = "SO_"
	else
		-- check for protocol
		level = llnet.Protocols[name]
		prefix = name:upper() .. '_'
	end
	local opts = new_level_opts(level, prefix)
	rawset(tab, name, opts)
	return opts
end,
})

local sock_mt = {}
sock_mt.__index = sock_mt

local function push_perror()
	return nil, Errors[ffi.errno()]
end

local function wrap_llnet_sock(fd, family, stype)
	return setmetatable({
		fd = fd,
		family = family,
		stype = stype,
	}, sock_mt)
end

local tmp_addr = ffi.new("LSockAddr[1]")
C.l_sockaddr_init(tmp_addr)

function sock_mt:accept(addr, flags)
	local fd = C.l_socket_accept(self.fd, addr, flags or 0)
	if fd == -1 then return push_perror() end
	return wrap_llnet_sock(fd, self.family, self.stype)
end

local function make_addr(host, port)
	if host == '*' or host == nil then host = '0.0.0.0' end
	C.l_sockaddr_set_ip_port(tmp_addr, host, tonumber(port))
	return tmp_addr
end

function sock_mt:bind(host, port)
	local rc = C.l_socket_bind(self.fd, make_addr(host, port))
	if rc == -1 then return push_perror() end
	return true
end

function sock_mt:connect(host, port)
	local rc = C.l_socket_connect(self.fd, make_addr(host, port))
	if rc == -1 then return push_perror() end
	return true
end

function sock_mt:listen(backlog)
	local rc = C.l_socket_listen(self.fd,backlog)
	if rc == -1 then return push_perror() end
	return true
end

local tmp_val = ffi.new("int[1]")
function sock_mt:getopt(level, option)
	local opts = Levels[level]
	local rc = C.l_socket_get_int_option(self.fd,opts._level, opts[option], val)
	if rc == -1 then return push_perror() end
	return val[0]
end

function sock_mt:setopt(level, option, value)
	local opts = Levels[level]
	local rc = C.l_socket_set_int_option(self.fd,opts._level, opts[option], value)
	if rc == -1 then return push_perror() end
	return true
end

function sock_mt:setblocking(blocking)
	if blocking then
		local rc = C.l_socket_set_nonblock(self.fd,0)
	else
		local rc = C.l_socket_set_nonblock(self.fd,1)
	end
	if rc == -1 then return push_perror() end
	return true
end

function sock_mt:shutdown(how)
	if self.fd then
		local rc = C.l_socket_shutdown(self.fd,how)
		if rc == -1 then return push_perror() end
	end
	return true
end

function sock_mt:fileno()
	return self.fd
end

function sock_mt:close()
	local fd = self.fd
	if fd == nil then return true end
	self.fd = nil
	local rc = C.l_socket_close_internal(fd)
	if rc == -1 then return push_perror() end
	return true
end

function sock_mt:send(buf)
	local rc = C.l_socket_send(self.fd,buf, #buf, 0)
	if rc == -1 then return push_perror() end
	return rc
end

local tmp_buf_len = 8192
local tmp_buf = ffi.new("char[?]", tmp_buf_len)
function sock_mt:recv(len)
	local buf_len = (tmp_buf_len < len) and tmp_buf_len or len
	local rc = C.l_socket_recv(self.fd, tmp_buf, buf_len, 0)
	if rc <= 0 then
		-- rc == 0, then socket is closed.
		if rc == 0 then return nil, "CLOSED" end
		return push_perror()
	end
	return ffi.string(tmp_buf, rc)
end

function sock_mt:send_buf(data, len, flags)
	local rc = C.l_socket_send(self.fd, data, len, flags or 0)
	if rc == -1 then return push_perror() end
	return rc
end

function sock_mt:recv_buf(data, len, flags)
	local rc = C.l_socket_recv(self.fd, data, len, flags or 0)
	if rc <= 0 then
		-- rc == 0, then socket is closed.
		if rc == 0 then return nil, "CLOSED" end
		return push_perror()
	end
	return rc
end

module(...)

NONBLOCK = llnet.SOCK_NONBLOCK
CLOEXEC = llnet.SOCK_CLOEXEC

function new(family, stype, proto, flags)
	family = Families[family or 'inet']
	stype = Sock_types[stype or 'stream']
	local fd = C.l_socket_open(family, stype, proto or 0, flags or 0)
	if fd == -1 then return push_perror() end
	return wrap_llnet_sock(fd, family, stype)
end


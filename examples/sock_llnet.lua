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

local llnet = require"llnet"
local get_opt = llnet.GetSocketOption
local set_opt = llnet.SetSocketOption
local new_socket = llnet.LSocketFD
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
	rawset(tab, opt, name)
	return name
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

local function wrap_llnet_sock(fd, family, stype)
	return setmetatable({
		fd = fd,
		family = family,
		stype = stype,
	}, sock_mt)
end

function sock_mt:accept(addr, flags)
	local sock, err = self.fd:accept(addr, flags)
	if sock then
		return wrap_llnet_sock(sock, self.family, self.stype)
	end
	return nil, err
end

local tmp_addr = llnet.LSockAddr()
local function make_addr(host, port)
	if host == '*' or host == nil then host = '0.0.0.0' end
	tmp_addr:lookup_full(host, port)
	return tmp_addr
end

function sock_mt:bind(host, port)
	return self.fd:bind(make_addr(host, port))
end

function sock_mt:connect(host, port)
	return self.fd:connect(make_addr(host, port))
end

function sock_mt:listen(backlog)
	return self.fd:listen(backlog)
end

function sock_mt:getopt(level, option)
	local opts = Levels[level]
	local name = opts[option]
	return get_opt[name](self.fd)
end

function sock_mt:setopt(level, option, value)
	local opts = Levels[level]
	local name = opts[option]
	return set_opt[name](self.fd, value)
end

function sock_mt:setblocking(blocking)
	return self.fd:set_nonblock(not blocking)
end

function sock_mt:shutdown(how)
	if self.fd then
		return self.fd:shutdown(how)
	end
end

function sock_mt:fileno()
	return self.fd:fileno()
end

function sock_mt:close()
	local fd = self.fd
	if not fd then return true end
	self.fd = nil
	return fd:close()
end

function sock_mt:send(buf)
	return self.fd:send(buf, 0)
end

function sock_mt:recv(len)
	return self.fd:recv(len, 0)
end

function sock_mt:send_buf(buf, off, len, flags)
	return self.fd:send_buf(buf, off, len, flags or 0)
end

function sock_mt:recv_buf(buf, off, len, flags)
	return self.fd:recv_buf(buf, off, len, flags or 0)
end

function sock_mt:send_buffer(buf, off, len, flags)
	return self.fd:send_buffer(buf, off, len, flags or 0)
end

function sock_mt:recv_buffer(buf, off, len, flags)
	return self.fd:recv_buffer(buf, off, len, flags or 0)
end

module(...)

NONBLOCK = llnet.SOCK_NONBLOCK
CLOEXEC = llnet.SOCK_CLOEXEC

function new(family, stype, proto, flags)
	family = Families[family or 'inet']
	stype = Sock_types[stype or 'stream']
	return wrap_llnet_sock(new_socket(family, stype, proto or 0, flags or 0), family, stype)
end


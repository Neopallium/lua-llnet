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

local nixio = require"nixio"
local new_socket = nixio.socket

local SOCK_NONBLOCK = 1
local SOCK_CLOEXEC = 0

local sock_mt = {}
sock_mt.__index = sock_mt

local function wrap_nixio_sock(sock, flags)
	if flags and flags > 0 then
		if flags == SOCK_NONBLOCK then
			sock:setblocking(false)
		end
	end
	return setmetatable({
		sock = sock,
	}, sock_mt)
end

function sock_mt:accept(addr, flags)
	local sock, errno, errmsg = self.sock:accept()
	if sock then
		return wrap_nixio_sock(sock, flags)
	end
	if sock == false then return nil, 'EAGAIN' end
	return nil, errmsg
end

function sock_mt:bind(host, port)
	return self.sock:bind(host, port)
end

function sock_mt:connect(host, port)
	local rc, errno, errmsg = self.sock:connect(host, port)
	if rc == false then return nil, 'EAGAIN' end
	if rc == nil then
		if errmsg == 'Operation now in progress' then
			return nil, 'EINPROGRESS'
		end
	end
	return rc, errmsg
end

function sock_mt:listen(backlog)
	return self.sock:listen(backlog)
end

local unsupported_options = {
defer_accept = true,
}

function sock_mt:getopt(level, option)
	if not unsupported_options[option] then
		return self.sock:getsockopt(level, option)
	end
	return true
end

function sock_mt:setopt(level, option, value)
	if not unsupported_options[option] then
		return self.sock:setsockopt(level, option, value)
	end
	return true
end

function sock_mt:setblocking(blocking)
	return self.sock:setblocking(blocking)
end

function sock_mt:shutdown(how)
	return self.sock:shutdown(how)
end

function sock_mt:fileno()
	return self.sock:fileno()
end

function sock_mt:close()
	local sock = self.sock
	if sock == nil then return true end
	self.sock = nil
	return sock:close()
end

function sock_mt:send(buf)
	local rc, errno, errmsg = self.sock:send(buf)
	if rc == false then return nil, 'EAGAIN' end
	return rc, errmsg
end

function sock_mt:recv(len)
	local rc, errno, errmsg = self.sock:recv(len)
	if rc == false then return nil, 'EAGAIN' end
	if rc == "" then return nil, 'CLOSED' end
	return rc, errmsg
end

module(...)

NONBLOCK = SOCK_NONBLOCK
CLOEXEC = SOCK_CLOEXEC

function new(family, stype, proto, flags)
	return wrap_nixio_sock(new_socket(family or 'inet', stype or 'stream'), flags)
end


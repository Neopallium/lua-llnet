
local epoll = require"epoll"

local bit = require"bit32"
local band = bit.band

local setmetatable = setmetatable
local pairs = pairs

local EPOLLIN = epoll.EPOLLIN
local EPOLLOUT = epoll.EPOLLOUT
local EPOLLERR = epoll.EPOLLERR

local poll_mt = {}
poll_mt.__index = poll_mt

function poll_mt:add(sock, events)
	local fd = sock:fileno()
	self.socks[fd] = sock
	return self.epoll:add(fd, events, fd)
end

function poll_mt:mod(sock, events)
	local fd = sock:fileno()
	self.socks[fd] = sock
	return self.epoll:mod(fd, events, fd)
end

function poll_mt:del(sock)
	local fd = sock:fileno()
	-- check if the socket is registered.
	sock = self.socks[fd]
	if sock then
		-- clear registered socket.
		self.socks[fd] = nil
		-- remove events for socket.
		return self.epoll:del(fd)
	end
end

local function make_event_callback(self, socks)
	return function (fd, ev)
		-- call registered callback.
		local sock = socks[fd]
		if not sock then return end
		local event_cb = sock.on_io_event
		if event_cb then
			event_cb(sock, ev)
		else
			self:del(sock)
		end
	end
end

local function _loop_step_cb(epoll, event_cb, timeout)
end

function poll_mt:step(timeout)
	local rc, err = self.epoll:wait_callback(self.event_cb, timeout or -1)
	if rc then return rc end
	if err ~= 'EINTR' then
		error(err)
	end
	return 0
end

function poll_mt:start()
	local epoll = self.epoll
	local socks = self.socks
	local event_cb = self.event_cb
	self.is_running = true
	while self.is_running do
		epoll:wait_callback(event_cb, -1)
	end
end

function poll_mt:stop()
	self.is_running = false
end

function poll_mt:close()
	return self.epoll:close()
end

local _M={}

for k,v in pairs(epoll) do
	if k:sub(1,5) == 'EPOLL' then
		_M[k] = v
	end
end

function _M.new()
	local self
	local socks = {}
	self = setmetatable({
		epoll = epoll.new(),
		event_cb = make_event_callback(self, socks),
		socks = socks,
	}, poll_mt)
	return self
end

return _M

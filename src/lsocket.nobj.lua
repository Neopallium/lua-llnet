-- Copyright (c) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>
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

export_definitions {
-- address families
"AF_UNSPEC",
"AF_UNIX",
"AF_INET",
"AF_INET6",
"AF_IPX",
"AF_NETLINK",
"AF_PACKET",

-- socket types
"SOCK_STREAM",
"SOCK_DGRAM",
"SOCK_SEQPACKET",
"SOCK_RAW",
"SOCK_RDM",
-- flags
"SOCK_NONBLOCK",
"SOCK_CLOEXEC",

-- Shutdown how types.
"SHUT_RD",
"SHUT_WR",
"SHUT_RDWR",

-- IP_MTU_DISCOVER arguments.
"IP_PMTUDISC_DONT",   -- Never send DF frames.
"IP_PMTUDISC_WANT",   -- Use per route hints.
"IP_PMTUDISC_DO",     -- Always DF.
"IP_PMTUDISC_PROBE",  -- Ignore dst pmtu.

-- IPV6_MTU_DISCOVER arguments.
"IPV6_PMTUDISC_DONT",   -- Never send DF frames.
"IPV6_PMTUDISC_WANT",   -- Use per route hints.
"IPV6_PMTUDISC_DO",     -- Always DF.
"IPV6_PMTUDISC_PROBE",  -- Ignore dst pmtu.

-- Routing header options for IPv6.
"IPV6_RTHDR_LOOSE",  -- Hop doesn't need to be neighbour.
"IPV6_RTHDR_STRICT", -- Hop must be a neighbour.

"IPV6_RTHDR_TYPE_0", -- IPv6 Routing header type 0.

}

object "LSocket" {
	userdata_type = 'embed',
	include "lsocket.h",
	ffi_typedef[[
typedef int LSocketFD;
struct LSocket {
	LSocketFD fd;
};
]],
	ffi_cdef[[
int l_socket_send(LSocket *sock, const void *buf, size_t len, int flags);
int l_socket_recv(LSocket *sock, void *buf, size_t len, int flags);
]],
	implements "FD" {
		implement_method "get_fd" {
			get_field = 'fd'
		},
		implement_method "get_type" {
			constant = "1", -- 1 == socket
		},
	},
	constructor {
		c_method_call "errno_rc" "l_socket_open"
			{ "int", "domain", "int", "type", "int", "protocol?", "int", "flags?" },
	},
	constructor "fd" {
		var_in{"int", "fd"},
		c_source"  ${this}->fd = ${fd};",
		ffi_source"  ${this}.fd = ${fd}",
	},
	destructor "close" {
		c_method_call "void" "l_socket_close" {},
	},
	method "__tostring" {
		var_out{ "const char *", "str", },
		c_source "pre" [[
	char tmp[64];
]],
		c_source[[
	${str} = tmp;
	snprintf(tmp, 64, "LSocket: fd=%d", ${this}->fd);
]],
		ffi_source[[
	${str} = string.format("LSocket: fd=%i", ${this}.fd)
]],
	},
	method "shutdown" {
		c_method_call "errno_rc" "l_socket_shutdown" { "int", "how" },
	},
	method "fileno" {
		var_out{"int", "fd"},
		c_source"  ${fd} = ${this}->fd;",
		ffi_source"  ${fd} = ${this}.fd",
	},
	method "set_nonblock" {
		c_method_call "errno_rc" "l_socket_set_nonblock" { "bool", "nonblock" },
	},
	method "connect" {
		c_method_call "errno_rc" "l_socket_connect" { "LSockAddr *", "addr" },
	},
	method "bind" {
		c_method_call "errno_rc" "l_socket_bind" { "LSockAddr *", "addr" },
	},
	method "listen" {
		c_method_call "errno_rc" "l_socket_listen" { "int", "backlog" },
	},
	method "get_sockname" {
		c_method_call "errno_rc" "l_socket_get_sockname" { "LSockAddr *", "addr" },
	},
	method "get_peername" {
		c_method_call "errno_rc" "l_socket_get_peername" { "LSockAddr *", "addr" },
	},
	method "accept" {
		c_method_call { "errno_rc", "rc"} "l_socket_accept"
			{ "LSocket *", "client>1", "LSockAddr *", "peer?", "int", "flags?" },
	},
	method "send" {
		var_in{"const char *", "data"},
		var_in{"int", "flags?"},
		var_out{"errno_rc", "rc"},
		c_source[[
	${rc} = l_socket_send(${this}, ${data}, ${data_len}, ${flags});
	/* ${rc} >= 0, then return number of bytes sent. */
	if(${rc} >= 0) {
		lua_pushinteger(L, ${rc});
		return 1;
	}
]],
		ffi_source[[
	${rc} = C.l_socket_send(${this}, ${data}, ${data_len}, ${flags})
	-- ${rc} >= 0, then return number of bytes sent.
	if ${rc} >= 0 then return ${rc} end
]],
	},
	ffi_source[[
local tmp_buf_len = 8192
local tmp_buf = ffi.new("char[?]", tmp_buf_len)
]],
	method "recv" {
		var_in{"size_t", "len"},
		var_in{"int", "flags?"},
		var_out{"char *", "data", has_length = true},
		var_out{"errno_rc", "rc"},
		c_source "pre" [[
#define BUF_LEN 8192
	char buf[BUF_LEN];
	size_t buf_len = BUF_LEN;
]],
		c_source[[
	if(buf_len > ${len}) { buf_len = ${len}; }
	${rc} = l_socket_recv(${this}, buf, buf_len, ${flags});
	/* ${rc} == 0, then socket is closed. */
	if(${rc} == 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "CLOSED");
		return 2;
	}
	${data} = buf;
	${data_len} = ${rc};
]],
		ffi_source[[
	local buf_len = (tmp_buf_len < ${len}) and tmp_buf_len or ${len}
	${rc} = C.l_socket_recv(${this}, tmp_buf, buf_len, ${flags})
	-- ${rc} == 0, then socket is closed.
	if ${rc} == 0 then return nil, "CLOSED" end
	${data} = tmp_buf;
	${data_len} = ${rc};
]],
	},

	method "send_buffer" {
		var_in{"Buffer", "buf"},
		var_in{"size_t", "off?", default = 0 },
		var_in{"size_t", "len?", default = 0 },
		var_in{"int", "flags?"},
		var_out{"errno_rc", "rc"},
		-- temp. vars
		c_source "pre" [[
	size_t data_len;
	const uint8_t *data;
]],
		c_source[[
	data_len = ${buf}_if->get_size(${buf});
	data = ${buf}_if->const_data(${buf});
	/* apply offset. */
	if(${off} > 0) {
		if(${off} >= data_len) {
			luaL_argerror(L, ${off::idx}, "Offset out-of-bounds.");
		}
		data += ${off};
		data_len -= ${off};
	}
	/* apply length. */
	if(${len} > 0) {
		if(${len} > data_len) {
			luaL_argerror(L, ${len::idx}, "Length out-of-bounds.");
		}
		data_len = ${len};
	}
	${rc} = l_socket_send(${this}, data, data_len, ${flags});
	/* ${rc} >= 0, then return number of bytes sent. */
	if(${rc} >= 0) {
		lua_pushinteger(L, ${rc});
		return 1;
	}
]],
		ffi_source[[
	local data_len = ${buf}_if.get_size(${buf})
	local data = ${buf}_if.const_data(${buf})
	-- apply offset.
	if(${off} > 0) then
		if(${off} >= data_len) then
			error("Offset out-of-bounds.");
		end
		data = data + ${off};
		data_len = data_len - ${off};
	end
	-- apply length.
	if(${len} > 0) then
		if(${len} > data_len) then
			error("Length out-of-bounds.");
		end
		data_len = ${len};
	end
	${rc} = C.l_socket_send(${this}, data, data_len, ${flags})
	-- ${rc} >= 0, then return number of bytes sent.
	if ${rc} >= 0 then return ${rc} end
]],
	},
	method "recv_buffer" {
		var_in{"MutableBuffer", "buf"},
		var_in{"size_t", "off?", default = 0 },
		var_in{"size_t", "len?", default = 4 * 1024 },
		var_in{"int", "flags?"},
		var_out{"int", "read_len"},
		var_out{"errno_rc", "rc"},
		-- temp. vars
		c_source "pre" [[
	size_t cap_len;
	uint8_t *data;
]],
		c_source[[
	cap_len = ${buf}_if->get_size(${buf});
	data = ${buf}_if->data(${buf});
	/* apply offset. */
	if(${off} > 0) {
		if(${off} >= cap_len) {
			luaL_argerror(L, ${off::idx}, "Offset out-of-bounds.");
		}
		data += ${off};
		cap_len -= ${off};
	}
	/* calculate read length. */
	if(${len} < cap_len) {
		cap_len = ${len};
	}
	if(0 == cap_len) {
		lua_pushnil(L);
		lua_pushliteral(L, "ENOBUFS");
		return 2;
	}
	${rc} = l_socket_recv(${this}, data, cap_len, ${flags});
	/* ${rc} == 0, then socket is closed. */
	if(${rc} == 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "CLOSED");
		return 2;
	}
	${read_len} = ${rc};
]],
		ffi_source[[
	local cap_len = ${buf}_if.get_size(${buf})
	local data = ${buf}_if.data(${buf})
	-- apply offset.
	if(${off} > 0) then
		if(${off} >= cap_len) then
			error("Offset out-of-bounds.");
		end
		data = data + ${off};
		cap_len = cap_len - ${off};
	end
	-- calculate read length.
	if(${len} < cap_len) then
		cap_len = ${len};
	end
	if(0 == cap_len) then
		return nil, "ENOBUFS"
	end
	${rc} = C.l_socket_recv(${this}, data, cap_len, ${flags})
	-- ${rc} == 0, then socket is closed.
	if ${rc} == 0 then return nil, "CLOSED" end
	${read_len} = ${rc}
]],
	},
}


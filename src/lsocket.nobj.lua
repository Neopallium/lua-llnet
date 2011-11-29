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
}

export_definitions "Options" {
-- set/get sockopt levels.
"SOL_SOCKET",
"SOL_IP",
"SOL_IPV6",
"SOL_ICMPV6",
"SOL_TCP",
"SOL_UDP",
"SOL_UNIX",

-- Socket options.
"SO_DEBUG",
"SO_REUSEADDR",
"SO_TYPE",
"SO_ERROR",
"SO_DONTROUTE",
"SO_BROADCAST",
"SO_SNDBUF",
"SO_RCVBUF",
"SO_SNDBUFFORCE",
"SO_RCVBUFFORCE",
"SO_KEEPALIVE",
"SO_OOBINLINE",
"SO_NO",
"SO_PRIORITY",
"SO_LINGER",
"SO_BSDCOMPAT",

"SO_PASSCRED",
"SO_PEERCRED",
"SO_RCVLOWAT",
"SO_SNDLOWAT",
"SO_RCVTIMEO",
"SO_SNDTIMEO",

-- Security levels - as per NRL IPv6 - don't actually do anything
"SO_SECURITY_AUTHENTICATION",
"SO_SECURITY_ENCRYPTION_TRANSPORT",
"SO_SECURITY_ENCRYPTION_NETWORK",

"SO_BINDTODEVICE",

-- Socket filtering
"SO_ATTACH",
"SO_DETACH",

"SO_PEERNAME",
"SO_TIMESTAMP",

"SO_ACCEPTCONN",

"SO_PEERSEC",
"SO_PASSSEC",
"SO_TIMESTAMPNS",

"SO_MARK",

"SO_TIMESTAMPING",

"SO_PROTOCOL",
"SO_DOMAIN",

"SO_RXQ",

--
-- IP options
--
"IP_OPTIONS",	-- ip_opts; IP per-packet options.  
"IP_HDRINCL",	-- int; Header is included with data.  
"IP_TOS",	-- int; IP type of service and precedence.  
"IP_TTL",	-- int; IP time to live.  
"IP_RECVOPTS",	-- bool; Receive all IP options w/datagram.  
-- For BSD compatibility.  
"IP_RECVRETOPTS",	-- bool; Receive IP options for response.  
"IP_RETOPTS",	-- ip_opts; Set/get IP per-packet options.  
"IP_MULTICAST_IF",	-- in_addr; set/get IP multicast i/f 
"IP_MULTICAST_TTL",	-- u_char; set/get IP multicast ttl 
"IP_MULTICAST_LOOP",	-- i_char; set/get IP multicast loopback 
"IP_ADD_MEMBERSHIP",	-- ip_mreq; add an IP group membership 
"IP_DROP_MEMBERSHIP",	-- ip_mreq; drop an IP group membership 
"IP_UNBLOCK_SOURCE",	-- ip_mreq_source: unblock data from source 
"IP_BLOCK_SOURCE",	-- ip_mreq_source: block data from source 
"IP_ADD_SOURCE_MEMBERSHIP",	-- ip_mreq_source: join source group 
"IP_DROP_SOURCE_MEMBERSHIP",	-- ip_mreq_source: leave source group 
"IP_MSFILTER",

"IP_ROUTER_ALERT",	-- bool 
"IP_PKTINFO",	-- bool 
"IP_PKTOPTIONS",
"IP_PMTUDISC",	-- obsolete name? 
"IP_MTU_DISCOVER",	-- int; see below 
"IP_RECVERR",	-- bool 
"IP_RECVTTL",	-- bool 
"IP_RECVTOS",	-- bool 
"IP_MTU",	-- int 
"IP_FREEBIND",
"IP_IPSEC_POLICY",
"IP_XFRM_POLICY",
"IP_PASSSEC",
"IP_TRANSPARENT",

-- TProxy original addresses 
"IP_ORIGDSTADDR",
"IP_RECVORIGDSTADDR",

"IP_MINTTL",


-- IP_MTU_DISCOVER arguments.  
"IP_PMTUDISC_DONT",	-- Never send DF frames.  
"IP_PMTUDISC_WANT",	-- Use per route hints.  
"IP_PMTUDISC_DO",	-- Always DF.  
"IP_PMTUDISC_PROBE",	-- Ignore dst pmtu.  

"IP_DEFAULT_MULTICAST_TTL",
"IP_DEFAULT_MULTICAST_LOOP",
"IP_MAX_MEMBERSHIPS",


--
-- IP options
--
"IPV6_ADDRFORM",
"IPV6_2292PKTINFO",
"IPV6_2292HOPOPTS",
"IPV6_2292DSTOPTS",
"IPV6_2292RTHDR",
"IPV6_2292PKTOPTIONS",
"IPV6_CHECKSUM",
"IPV6_2292HOPLIMIT",

"IPV6_NEXTHOP",
"IPV6_AUTHHDR",
"IPV6_UNICAST_HOPS",
"IPV6_MULTICAST_IF",
"IPV6_MULTICAST_HOPS",
"IPV6_MULTICAST_LOOP",
"IPV6_JOIN_GROUP",
"IPV6_LEAVE_GROUP",
"IPV6_ROUTER_ALERT",
"IPV6_MTU_DISCOVER",
"IPV6_MTU",
"IPV6_RECVERR",
"IPV6_V6ONLY",
"IPV6_JOIN_ANYCAST",
"IPV6_LEAVE_ANYCAST",
"IPV6_IPSEC_POLICY",
"IPV6_XFRM_POLICY",

"IPV6_RECVPKTINFO",
"IPV6_PKTINFO",
"IPV6_RECVHOPLIMIT",
"IPV6_HOPLIMIT",
"IPV6_RECVHOPOPTS",
"IPV6_HOPOPTS",
"IPV6_RTHDRDSTOPTS",
"IPV6_RECVRTHDR",
"IPV6_RTHDR",
"IPV6_RECVDSTOPTS",
"IPV6_DSTOPTS",

"IPV6_RECVTCLASS",
"IPV6_TCLASS",

-- Obsolete synonyms for the above.  
"IPV6_ADD_MEMBERSHIP",
"IPV6_DROP_MEMBERSHIP",
"IPV6_RXHOPOPTS",
"IPV6_RXDSTOPTS",

-- IPV6_MTU_DISCOVER values.  
"IPV6_PMTUDISC_DONT",	-- Never send DF frames.  
"IPV6_PMTUDISC_WANT",	-- Use per route hints.  
"IPV6_PMTUDISC_DO",	-- Always DF.  
"IPV6_PMTUDISC_PROBE",	-- Ignore dst pmtu.  

-- Routing header options for IPv6.  
"IPV6_RTHDR_LOOSE",	-- Hop doesn't need to be neighbour. 
"IPV6_RTHDR_STRICT",	-- Hop must be a neighbour.  

"IPV6_RTHDR_TYPE_0",	-- IPv6 Routing header type 0.  

--
-- TCP options.
--

"TCP_NODELAY",	-- Don't delay send to coalesce packets  
"TCP_MAXSEG",	-- Set maximum segment size  
"TCP_CORK",	-- Control sending of partial frames  
"TCP_KEEPIDLE",	-- Start keeplives after this period 
"TCP_KEEPINTVL",	-- Interval between keepalives 
"TCP_KEEPCNT",	-- Number of keepalives before death 
"TCP_SYNCNT",	-- Number of SYN retransmits 
"TCP_LINGER2",	-- Life time of orphaned FIN-WAIT-2 state 
"TCP_DEFER_ACCEPT",	-- Wake up listener only when data arrive 
"TCP_WINDOW_CLAMP",	-- Bound advertised window 
"TCP_INFO",	-- Information about this connection. 
"TCP_QUICKACK",	-- Bock/reenable quick ACKs.  
"TCP_CONGESTION",	-- Congestion control algorithm.  
"TCP_MD5SIG",	-- TCP MD5 Signature (RFC2385) 

}
object "LSocketFD" {
	userdata_type = 'simple',
	sys_include "sys/socket.h",
	sys_include "netinet/in.h",
	sys_include "netinet/ip.h",
	sys_include "netinet/tcp.h",
	sys_include "netinet/udp.h",
	include "lsocket.h",
	ffi_type = "int",
	ffi_cdef[[
int l_socket_recv(LSocketFD sock, void *buf, size_t len, int flags);
]],
	constructor {
		c_call "LSocketFD" "l_socket_open"
			{ "int", "domain", "int", "type", "int", "protocol", "int", "flags" },
	},
	constructor "fd" {
		var_in{"int", "fd"},
		c_source[[
	${this} = ${fd};
]],
		ffi_source[[
	${this} = ${fd}
]],
	},
	destructor "close" {
		c_method_call "void" "l_socket_close_internal" {},
	},
	method "__tostring" {
		var_out{ "const char *", "str", },
		c_source "pre" [[
	char tmp[1024];
]],
		c_source[[
	${str} = tmp;
	snprintf(tmp, 1024, "LSocketFD: fd=%d\n", ${this});
]],
		ffi_source[[
	${str} = string.format("LSocketFD: fd=%i\n", ${this})
]],
	},
	method "shutdown" {
		c_method_call "int" "l_socket_shutdown" { "int", "how" },
	},
	method "fileno" {
		var_out{"int", "fd"},
		c_source[[
	${fd} = ${this};
]],
		ffi_source[[
	${fd} = ${this}
]],
	},
	method "set_nonblock" {
		c_method_call "errno_rc" "l_socket_set_nonblock" { "bool", "nonblock" },
	},
	method "set_option" {
		var_in{"int", "level"},
		var_in{"int", "opt"},
		var_in{"<any>", "value"},
		var_out{"errno_rc", "rc"},
		c_source "pre" [[
	int val_type;
]],
		c_source[[
	val_type = lua_type(L, ${value::idx});
	if(val_type == LUA_TSTRING) {
		size_t len;
		const char *val = lua_tolstring(L, ${value::idx}, &len);
		${rc} = l_socket_set_option(${this}, ${level}, ${opt}, val, len);
	} else if(val_type == LUA_TNUMBER) {
		int val = lua_tointeger(L, ${value::idx});
		${rc} = l_socket_set_option(${this}, ${level}, ${opt}, &val, sizeof(val));
	} else {
		return luaL_argerror(L, ${value::idx}, "expected string/number");
	}
]],
	},
	method "set_int_option" {
		c_method_call "errno_rc" "l_socket_set_int_option" { "int", "level", "int", "opt", "int", "val" },
	},
	method "get_int_option" {
		c_method_call "errno_rc" "l_socket_get_int_option" { "int", "level", "int", "opt", "int", "&val>1" },
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
	method "accept" {
		var_out{"LSocketFD", "client"},
		c_method_call { "errno_rc", "rc"} "l_socket_accept" { "LSockAddr *", "peer?", "int", "flags?" },
		c_source[[
	${client} = ${rc};
]],
		ffi_source[[
	${client} = ${rc};
]],
	},
	method "send" {
		c_method_call "errno_rc" "l_socket_send"
			{ "const char *", "data", "size_t", "#data", "int", "flags?" },
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
	int buf_len = BUF_LEN;
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
}


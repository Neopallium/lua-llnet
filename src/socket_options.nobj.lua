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

local Options = {
-- set/get sockopt levels.
"SOL_SOCKET",
"SOL_IP",
"SOL_IPV6",
"SOL_ICMPV6",
"SOL_TCP",
"SOL_UDP",
"SOL_UNIX",
}

local supported_otypes = {
bool   = { ctype="int",
	set_opt = [[
errno_rc lsocket_opt_set_${opt_name}(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, ${level}, ${opt_name}, value);
}
]],
	get_opt = [[
errno_rc lsocket_opt_get_${opt_name}(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, ${level}, ${opt_name}, value);
}
]],
},
int    = { ctype="int",
	set_opt = [[
errno_rc lsocket_opt_set_${opt_name}(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, ${level}, ${opt_name}, value);
}
]],
	get_opt = [[
errno_rc lsocket_opt_get_${opt_name}(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, ${level}, ${opt_name}, value);
}
]],
},
string = { ctype="const char *",
	set_opt = [[
errno_rc lsocket_opt_set_${opt_name}(LSocketFD sock, const char *value, size_t len) {
	return l_socket_set_option(sock, ${level}, ${opt_name}, value, len);
}
]],
	get_opt = [[
errno_rc lsocket_opt_get_${opt_name}(LSocketFD sock, char *value, size_t *len) {
	socklen_t slen = *len;
	int rc = l_socket_get_option(sock, ${level}, ${opt_name}, value, &slen);
	*len = slen;
	return rc;
}
]],
},
sockaddr = { ctype="LSockAddr *", is_object = true,
	set_opt = [[
errno_rc lsocket_opt_set_${opt_name}(LSocketFD sock, LSockAddr *addr) {
	return l_socket_set_option(sock, ${level}, ${opt_name}, L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr));
}
]],
	get_opt = [[
errno_rc lsocket_opt_get_${opt_name}(LSocketFD sock, LSockAddr *addr) {
	struct sockaddr_storage saddr;
	socklen_t slen = sizeof(saddr);
	int rc = l_socket_get_option(sock, ${level}, ${opt_name}, &addr, &slen);
	l_sockaddr_init(addr);
	l_sockaddr_fill(addr, (struct sockaddr *)&saddr, slen);
	return rc;
}
]],
},
-- not supported types.
  -- socket
linger    = false,
timeval   = false,
ucred     = false,
  -- IP
ip_mreqn  = false,
ip_mreq_source = false,
ip_msfilter= false,
in_pktinfo = false,
  -- IPv6
ipv6_mreq  = false,
in6_pktinfo= false,
ip6_hbh    = false,
ip6_dest   = false,
ip6_rthdr  = false,
}

local option_info = {
SOL_SOCKET = { prefix="SO_",
	-- Socket options.
	SO_DEBUG            = { otype="bool",   mode="rw" },
	SO_REUSEADDR        = { otype="bool",   mode="rw" },
	SO_TYPE             = { otype="int",    mode="r" },
	SO_ERROR            = { otype="int",    mode="r" },
	SO_DONTROUTE        = { otype="bool",   mode="rw" },
	SO_BROADCAST        = { otype="bool",   mode="rw" },
	SO_SNDBUF           = { otype="int",    mode="rw" },
	SO_RCVBUF           = { otype="int",    mode="rw" },
	SO_SNDBUFFORCE      = { otype="int",    mode="rw" },
	SO_RCVBUFFORCE      = { otype="int",    mode="rw" },
	SO_KEEPALIVE        = { otype="bool",   mode="rw" },
	SO_OOBINLINE        = { otype="bool",   mode="rw" },
	SO_NO_CHECK         = { otype="bool",   mode="rw" },
	SO_PRIORITY         = { otype="int",    mode="rw" },
	SO_LINGER           = { otype="linger", mode="rw" },
	SO_BSDCOMPAT        = { otype="bool",   mode="rw" },

	SO_PASSCRED         = { otype="bool",   mode="rw" },
	SO_PEERCRED         = { otype="ucred",  mode="r" },
	SO_RCVLOWAT         = { otype="int",    mode="rw" },
	SO_SNDLOWAT         = { otype="int",    mode="rw" },
	SO_RCVTIMEO         = { otype="timeval",mode="rw" },
	SO_SNDTIMEO         = { otype="timeval",mode="rw" },

	SO_BINDTODEVICE     = { otype="string", mode="rw" },

	SO_ACCEPTCONN       = { otype="bool",   mode="r" },

	SO_TIMESTAMP        = { otype="bool",   mode="rw" },
	SO_TIMESTAMPNS      = { otype="bool",   mode="rw" },
	SO_TIMESTAMPING     = { otype="int",    mode="rw" },

	SO_MARK             = { otype="int",    mode="rw" },

	SO_PROTOCOL         = { otype="int",    mode="r" },
	SO_DOMAIN           = { otype="int",    mode="r" },

},
SOL_IP = { prefix="IP_",
	--
	-- IP options
	--
	IP_OPTIONS        = { otype="string", mode="rw" },  -- ip_opts; IP per-packet options.  
	IP_HDRINCL        = { otype="bool",   mode="rw" },  -- int; Header is included with data.  
	IP_TOS            = { otype="int",    mode="rw" },  -- int; IP type of service and precedence.  
	IP_TTL            = { otype="int",    mode="rw" },  -- int; IP time to live.  
	IP_RECVOPTS       = { otype="bool",   mode="rw" },  -- bool; Receive all IP options w/datagram.  

	-- For BSD compatibility.
	IP_RECVRETOPTS    = { otype="bool",    mode="rw" },  -- bool; Receive IP options for response.  
	IP_RETOPTS        = { otype="string",  mode="rw" },  -- ip_opts; Set/get IP per-packet options.  
	IP_MULTICAST_IF   = { otype="ip_mreqn",mode="rw" },  -- in_addr; set/get IP multicast i/f 
	IP_MULTICAST_TTL  = { otype="int",     mode="rw" },  -- u_char; set/get IP multicast ttl 
	IP_MULTICAST_LOOP = { otype="bool",    mode="rw" },  -- i_char; set/get IP multicast loopback 
	IP_ADD_MEMBERSHIP = { otype="ip_mreqn",mode="w" },  -- ip_mreq; add an IP group membership 
	IP_DROP_MEMBERSHIP= { otype="ip_mreqn",mode="w" },  -- ip_mreq; drop an IP group membership 
	-- ip_mreq_source: unblock data from source 
	IP_UNBLOCK_SOURCE = { otype="ip_mreq_source",mode="w" },
	-- ip_mreq_source: block data from source 
	IP_BLOCK_SOURCE   = { otype="ip_mreq_source",mode="w" },
	-- ip_mreq_source: join source group 
	IP_ADD_SOURCE_MEMBERSHIP = { otype="ip_mreq_source",mode="rw" },
	-- ip_mreq_source: leave source group 
	IP_DROP_SOURCE_MEMBERSHIP= { otype="ip_mreq_source",mode="rw" },
	IP_MSFILTER       = { otype="ip_msfilter",    mode="rw" },

	IP_ROUTER_ALERT   = { otype="bool",   mode="rw" },  -- bool 
	IP_PKTINFO        = { otype="bool",   mode="rw" },  -- bool 
	IP_PKTOPTIONS     = { otype="in_pktinfo",mode="rw" },
	IP_MTU_DISCOVER   = { otype="int",    mode="rw" },  -- int; see below 
	IP_RECVERR        = { otype="bool",   mode="rw" },  -- bool 
	IP_RECVTTL        = { otype="bool",   mode="rw" },  -- bool 
	IP_RECVTOS        = { otype="bool",   mode="rw" },  -- bool 
	IP_MTU            = { otype="int",    mode="rw" },  -- int 
	IP_FREEBIND       = { otype="bool",   mode="rw" },

	-- TProxy original addresses 
	IP_RECVORIGDSTADDR= { otype="bool",   mode="rw" },

	IP_MINTTL         = { otype="int",    mode="rw" },

},
SOL_IPV6 = { prefix="IPV6_",

	--
	-- IP options
	--
	IPV6_ADDRFORM        = { otype="int",    mode="w" },
	IPV6_CHECKSUM        = { otype="int",    mode="rw" },

	IPV6_NEXTHOP         = { otype="sockaddr",mode="rw" },
	IPV6_UNICAST_HOPS    = { otype="int",    mode="rw" },
	IPV6_MULTICAST_IF    = { otype="int",    mode="rw" },
	IPV6_MULTICAST_HOPS  = { otype="int",    mode="rw" },
	IPV6_MULTICAST_LOOP  = { otype="bool",   mode="rw" },
	IPV6_ROUTER_ALERT    = { otype="int",    mode="rw" },
	IPV6_MTU_DISCOVER    = { otype="int",    mode="rw" },
	IPV6_MTU             = { otype="int",    mode="rw" },
	IPV6_RECVERR         = { otype="bool",   mode="rw" },
	IPV6_V6ONLY          = { otype="bool",   mode="rw" },
	IPV6_JOIN_GROUP      = { otype="ipv6_mreq",mode="w" },
	IPV6_LEAVE_GROUP     = { otype="ipv6_mreq",mode="w" },
	IPV6_JOIN_ANYCAST    = { otype="ipv6_mreq",mode="w" },
	IPV6_LEAVE_ANYCAST   = { otype="ipv6_mreq",mode="w" },

	IPV6_RECVPKTINFO     = { otype="bool",   mode="rw" },
	IPV6_PKTINFO         = { otype="in6_pktinfo",mode="rw" },
	IPV6_RECVHOPLIMIT    = { otype="bool",   mode="rw" },
	IPV6_HOPLIMIT        = { otype="int",    mode="rw" },
	IPV6_RECVHOPOPTS     = { otype="bool",   mode="rw" },
	IPV6_HOPOPTS         = { otype="ip6_hbh",mode="rw" },
	IPV6_RTHDRDSTOPTS    = { otype="ip6_dest",mode="rw" },
	IPV6_RECVRTHDR       = { otype="bool",   mode="rw" },
	IPV6_RTHDR           = { otype="ip6_rthdr",mode="rw" },
	IPV6_RECVDSTOPTS     = { otype="bool",   mode="rw" },
	IPV6_DSTOPTS         = { otype="ip6_dest",mode="rw" },

	IPV6_RECVTCLASS      = { otype="bool",   mode="rw" },
	IPV6_TCLASS          = { otype="int",    mode="rw" },
	-- Obsolete synonyms for the above.  
	IPV6_ADD_MEMBERSHIP  = "IPV6_JOIN_GROUP",
	IPV6_DROP_MEMBERSHIP = "IPV6_LEAVE_GROUP",
	IPV6_RXHOPOPTS       = "IPV6_HOPOPTS",
	IPV6_RXDSTOPTS       = "IPV6_DSTOPTS",
},
SOL_TCP = { prefix="TCP_",

	--
	-- TCP options.
	--
	TCP_NODELAY       = { otype="bool",   mode="rw" },  -- bool; Don't delay send to coalesce packets  
	TCP_MAXSEG        = { otype="int",    mode="rw" },  -- Set maximum segment size  
	TCP_CORK          = { otype="bool",   mode="rw" },  -- bool; Control sending of partial frames  
	TCP_KEEPIDLE      = { otype="int",    mode="rw" },  -- Start keeplives after this period 
	TCP_KEEPINTVL     = { otype="int",    mode="rw" },  -- Interval between keepalives 
	TCP_KEEPCNT       = { otype="int",    mode="rw" },  -- Number of keepalives before death 
	TCP_SYNCNT        = { otype="int",    mode="rw" },  -- Number of SYN retransmits 
	TCP_LINGER2       = { otype="int",    mode="rw" },  -- Life time of orphaned FIN-WAIT-2 state 
	TCP_DEFER_ACCEPT  = { otype="int",    mode="rw" },  -- int; Wake up listener only when data arrive 
	TCP_WINDOW_CLAMP  = { otype="int",    mode="rw" },  -- Bound advertised window 
	TCP_INFO          = { otype="tcp_info",mode="r" },  -- Information about this connection. 
	TCP_QUICKACK      = { otype="bool",   mode="rw" },  -- Bock/reenable quick ACKs.  
	TCP_CONGESTION    = { otype="string", mode="rw" },  -- Congestion control algorithm.  

},

}

-- Utility functions.
local add = function(t,v) return table.insert(t, v) end
local function template(data, templ)
	return templ:gsub("%${(.-)}", data)
end

local options_get_list = {}
local options_set_list = {}

local options_c_code = {}

-- add all options to Options table.
for lvl,opts in pairs(option_info) do
	local prefix = opts.prefix
	for opt,info in pairs(opts) do
		if opt:find(prefix) == 1 then
			-- check if this is an alias
			if type(info) == 'string' then
				info = opts[info]
			end
			info.level = lvl
			info.opt_name = opt
			-- check if option type is supported.
			local otype = info.otype
			local ctype = supported_otypes[otype]
			if ctype then
				-- copy ctype info
				for k,v in pairs(ctype) do info[k] = v end
				add(Options, opt)
				-- generate C code for option.
				local m = info.mode
				local code = "#ifdef ${opt_name}\n"
				if m == 'rw' or m == 'w' then
					local args = { "LSocketFD", "sock", ctype.ctype, "value" }
					if otype == 'string' then
						args[5] = "socklen_t"
						args[6] = "#value"
					end
					add(options_set_list, c_function(opt) { if_defs = opt,
						c_call "errno_rc" ("lsocket_opt_set_" .. opt) (args),
					})
					code = code .. ctype.set_opt
				end
				local get = ''
				if m == 'rw' or m == 'r' then
					local args = { "LSocketFD", "sock", ctype.ctype, "&value" }
					local val_out = { ctype.ctype, "&value" }
					if otype == 'string' then
						args = { "LSocketFD", "sock", "char *", "value", "socklen_t", "&#value" }
						val_out = { "char *", "value", has_length = true }
					elseif ctype.is_object then
						args = { "LSocketFD", "sock", ctype.ctype, "value" }
						val_out = { ctype.ctype, "value" }
					end
					add(options_get_list, c_function(opt) { if_defs = opt,
						var_out(val_out),
						c_call "errno_rc" ("lsocket_opt_get_" .. opt) (args),
					})
					code = code .. ctype.get_opt
				end
				code = code .. "\n#endif\n"
				add(options_c_code, template(info, code))
			end
		end
	end
end

options_c_code = table.concat(options_c_code)
c_source "src" (options_c_code)

export_definitions "Options" (Options)

package "SetSocketOption" (options_set_list)

package "GetSocketOption" (options_get_list)


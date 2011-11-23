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

meta_object "Protocols" {
	define "_GNU_SOURCE" "1",
	sys_include "netdb.h",
	define "L_PROTO_MAX_BUF" "4096",
	method "__index" {
		var_in{ "<any>", "key"},
		var_out{ "<any>", "proto" },
		c_source[[
	struct protoent *ent;
#if defined(HAVE_getprotobyname_r)
	struct protoent ent_buf;
	char buf[L_PROTO_MAX_BUF];
	int rc;
#endif
	const char *name;
	int proto;
	int key_type;

	luaL_checktype(L, ${this::idx}, LUA_TTABLE);

	key_type = lua_type(L, ${key::idx});
	if(key_type == LUA_TSTRING) {
		name = lua_tostring(L, ${key::idx});
#if defined(HAVE_getprotobyname_r)
		rc = getprotobyname_r(name, &ent_buf, buf, L_PROTO_MAX_BUF, &ent);
		if(rc != 0) {
			return luaL_error(L, "getprotobyname_r(): buffer to small.");
		}
#else
		ent = getprotobyname(name);
#endif
		if(ent == NULL) {
			return luaL_error(L, "Unknown protocol: name=%s", name);
		}
		lua_pushinteger(L, ent->p_proto);
	} else if(key_type == LUA_TNUMBER) {
		proto = lua_tointeger(L, ${key::idx});
#if defined(HAVE_getprotobynumber_r)
		rc = getprotobynumber_r(proto, &ent_buf, buf, L_PROTO_MAX_BUF, &ent);
		if(rc != 0) {
			return luaL_error(L, "getprotobynumber_r(): buffer to small.");
		}
#else
		ent = getprotobynumber(proto);
#endif
		if(ent == NULL) {
			return luaL_error(L, "Unknown protocol: number=%s", proto);
		}
		lua_pushstring(L, ent->p_name);
	} else {
		return luaL_argerror(L, ${key::idx}, "expected string/number");
	}

	/* cache results. */
	lua_pushvalue(L, ${key::idx});
	lua_pushvalue(L, -2);
	lua_rawset(L, ${this::idx});

	return 1;
]],
	},
}


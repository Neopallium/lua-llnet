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

meta_object "Services" {
	define "L_SERV_MAX_BUF" "4096",
	c_function "byname" {
		var_in{ "const char *", "name"},
		var_in{ "const char *", "proto?", default = "tcp"},
		c_source[[
	struct servent *ent;
#if defined(HAVE_getservbyname_r)
	struct servent ent_buf;
	char buf[L_SERV_MAX_BUF];
	int rc;

	rc = getservbyname_r(${name}, ${proto}, &ent_buf, buf, L_SERV_MAX_BUF, &ent);
	if(rc != 0) {
		return luaL_error(L, "getservbyname_r(): buffer to small.");
	}
#else
	ent = getservbyname(${name}, ${proto});
#endif
	if(ent == NULL) {
		return luaL_error(L, "Unknown service: name=%s, proto=%s", ${name}, ${proto});
	}

	lua_pushstring(L, ent->s_name);
	lua_pushinteger(L, ntohs(ent->s_port));
	lua_pushstring(L, ent->s_proto);

	return 3;
]],
	},
	c_function "byport" {
		var_in{ "int", "port"},
		var_in{ "const char *", "proto?", default = "tcp"},
		c_source[[
	struct servent *ent;
#if defined(HAVE_getservbyport_r)
	struct servent ent_buf;
	char buf[L_SERV_MAX_BUF];
	int rc;

	rc = getservbyport_r(htons(${port}), ${proto}, &ent_buf, buf, L_SERV_MAX_BUF, &ent);
	if(rc != 0) {
		return luaL_error(L, "getservbyport_r(): buffer to small.");
	}
#else
	ent = getservbyport(htons(${port}), ${proto});
#endif

	if(ent == NULL) {
		return luaL_error(L, "Unknown service: port=%d, proto=%s", ${port}, ${proto});
	}

	lua_pushstring(L, ent->s_name);
	lua_pushinteger(L, ntohs(ent->s_port));
	lua_pushstring(L, ent->s_proto);

	return 3;
]],
	},
	method "__index" {
		var_in{ "<any>", "key"},
		var_out{ "<any>", "value" },
		c_source[[
	struct servent *ent;
#if defined(HAVE_getservbyname_r)
	struct servent ent_buf;
	char buf[L_SERV_MAX_BUF];
	int rc;
#endif
	const char *name;
	int port;
	int key_type;

	luaL_checktype(L, ${this::idx}, LUA_TTABLE);

	key_type = lua_type(L, ${key::idx});
	if(key_type == LUA_TSTRING) {
		name = lua_tostring(L, ${key::idx});
#if defined(HAVE_getservbyname_r)
		rc = getservbyname_r(name, NULL, &ent_buf, buf, L_SERV_MAX_BUF, &ent);
		if(rc != 0) {
			return luaL_error(L, "getservbyname_r(): buffer to small.");
		}
#else
		ent = getservbyname(name, NULL);
#endif
		if(ent == NULL) {
			return luaL_error(L, "Unknown service: name=%s", name);
		}
		lua_pushinteger(L, ntohs(ent->s_port));
	} else if(key_type == LUA_TNUMBER) {
		port = lua_tointeger(L, ${key::idx});
#if defined(HAVE_getservbyname_r)
		rc = getservbyport_r(htons(port), NULL, &ent_buf, buf, L_SERV_MAX_BUF, &ent);
		if(rc != 0) {
			return luaL_error(L, "getservbyport_r(): buffer to small.");
		}
#else
		ent = getservbyport(htons(port), NULL);
#endif
		if(ent == NULL) {
			return luaL_error(L, "Unknown service: port=%s", port);
		}
		lua_pushstring(L, ent->s_name);
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


/***********************************************************************************************
************************************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!! Warning this file was generated from a set of *.nobj.lua definition files !!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
************************************************************************************************
***********************************************************************************************/

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* some Lua 5.0 compatibility support. */
#if !defined(lua_pushliteral)
#define lua_pushliteral(L, s) lua_pushstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#endif

#if !defined(LUA_VERSION_NUM)
#define lua_pushinteger(L, n) lua_pushnumber(L, (lua_Number)n)
#define luaL_Reg luaL_reg
#endif

/* some Lua 5.1 compatibility support. */
#if !defined(LUA_VERSION_NUM) || (LUA_VERSION_NUM == 501)
/*
** Adapted from Lua 5.2.0
*/
static void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    lua_pushstring(L, l->name);
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source)

#define lua_rawlen(L, idx) lua_objlen(L, idx)

#endif

#if LUA_VERSION_NUM == 502

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source, NULL)

static int luaL_typerror (lua_State *L, int narg, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                    tname, luaL_typename(L, narg));
  return luaL_argerror(L, narg, msg);
}

#endif

#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_MODULES_AS_GLOBALS 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 0
#define USE_FIELD_GET_SET_METHODS 0
#define LUAJIT_FFI 1
#define L_PROTO_MAX_BUF 4096
#define L_SERV_MAX_BUF 4096


#include <string.h>
#include "lsockaddr.h"
#include "laddrinfo.h"
#include "lsocket.h"
#include "liobuffer.h"



#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifdef _MSC_VER
#define __WINDOWS__
#else
#if defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__

/* for MinGW32 compiler need to include <stdint.h> */
#ifdef __GNUC__
#include <stdint.h>
#include <stdbool.h>
#else

/* define some standard types missing on Windows. */
#ifndef __INT32_MAX__
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#endif
#ifndef __INT64_MAX__
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
typedef int bool;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#endif

/* wrap strerror_s(). */
#ifdef __GNUC__
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) do { \
	strncpy((buf), strerror(errno), (buflen)-1); \
	(buf)[(buflen)-1] = '\0'; \
} while(0)
#endif
#else
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) strerror_s((buf), (buflen), (errno))
#endif
#endif

#define FUNC_UNUSED

#define LUA_NOBJ_API __declspec(dllexport)

#else

#define LUA_NOBJ_API LUALIB_API

#include <stdint.h>
#include <stdbool.h>

#define FUNC_UNUSED __attribute__((unused))

#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define assert_obj_type(type, obj) \
	assert(__builtin_types_compatible_p(typeof(obj), type *))
#else
#define assert_obj_type(type, obj)
#endif

void *nobj_realloc(void *ptr, size_t osize, size_t nsize);

void *nobj_realloc(void *ptr, size_t osize, size_t nsize) {
	(void)osize;
	if(0 == nsize) {
		free(ptr);
		return NULL;
	}
	return realloc(ptr, nsize);
}

#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	nobj_realloc((obj), sizeof(type), 0); \
} while(0)

#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = nobj_realloc(NULL, 0, sizeof(type)); \
} while(0)

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
#define OBJ_TYPE_IMPORT (1<<2)
struct obj_type {
	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */
	int32_t         id;       /**< type's id. */
	uint32_t        flags;    /**< type's flags (weak refs) */
	const char      *name;    /**< type's object name. */
};

typedef struct obj_base {
	int32_t        id;
	base_caster_t  bcaster;
} obj_base;

typedef enum obj_const_type {
	CONST_UNKOWN    = 0,
	CONST_BOOLEAN   = 1,
	CONST_NUMBER    = 2,
	CONST_STRING    = 3
} obj_const_type;

typedef struct obj_const {
	const char      *name;    /**< constant's name. */
	const char      *str;
	double          num;
	obj_const_type  type;
} obj_const;

typedef enum obj_field_type {
	TYPE_UNKOWN    = 0,
	TYPE_UINT8     = 1,
	TYPE_UINT16    = 2,
	TYPE_UINT32    = 3,
	TYPE_UINT64    = 4,
	TYPE_INT8      = 5,
	TYPE_INT16     = 6,
	TYPE_INT32     = 7,
	TYPE_INT64     = 8,
	TYPE_DOUBLE    = 9,
	TYPE_FLOAT     = 10,
	TYPE_STRING    = 11
} obj_field_type;

typedef struct obj_field {
	const char      *name;  /**< field's name. */
	uint32_t        offset; /**< offset to field's data. */
	obj_field_type  type;   /**< field's data type. */
	uint32_t        flags;  /**< is_writable:1bit */
} obj_field;

typedef enum {
	REG_OBJECT,
	REG_PACKAGE,
	REG_META,
} module_reg_type;

typedef struct reg_impl {
	const char *if_name;
	const void *impl;
} reg_impl;

typedef struct reg_sub_module {
	obj_type        *type;
	module_reg_type req_type;
	const luaL_Reg  *pub_funcs;
	const luaL_Reg  *methods;
	const luaL_Reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
	const reg_impl  *implements;
	int             bidirectional_consts;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char obj_udata_weak_ref_key[] = "obj_udata_weak_ref_key";

/* use static pointer as key to module's private table. */
static char obj_udata_private_key[] = "obj_udata_private_key";

#if LUAJIT_FFI
typedef int (*ffi_export_func_t)(void);
typedef struct ffi_export_symbol {
	const char *name;
	union {
	void               *data;
	ffi_export_func_t  func;
	} sym;
} ffi_export_symbol;
#endif



typedef int errno_rc;

static void error_code__errno_rc__push(lua_State *L, errno_rc err);
typedef int eai_rc;

static void error_code__eai_rc__push(lua_State *L, eai_rc err);


static obj_type obj_types[] = {
#define obj_type_id_Errors 0
#define obj_type_Errors (obj_types[obj_type_id_Errors])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "Errors" },
#define obj_type_id_EAI_Errors 1
#define obj_type_EAI_Errors (obj_types[obj_type_id_EAI_Errors])
  { NULL, 1, OBJ_TYPE_FLAG_WEAK_REF, "EAI_Errors" },
#define obj_type_id_Protocols 2
#define obj_type_Protocols (obj_types[obj_type_id_Protocols])
  { NULL, 2, OBJ_TYPE_FLAG_WEAK_REF, "Protocols" },
#define obj_type_id_Services 3
#define obj_type_Services (obj_types[obj_type_id_Services])
  { NULL, 3, OBJ_TYPE_FLAG_WEAK_REF, "Services" },
#define obj_type_id_LSockAddr 4
#define obj_type_LSockAddr (obj_types[obj_type_id_LSockAddr])
  { NULL, 4, OBJ_TYPE_SIMPLE, "LSockAddr" },
#define obj_type_id_LAddrInfo 5
#define obj_type_LAddrInfo (obj_types[obj_type_id_LAddrInfo])
  { NULL, 5, OBJ_TYPE_SIMPLE, "LAddrInfo" },
#define obj_type_id_Options 6
#define obj_type_Options (obj_types[obj_type_id_Options])
  { NULL, 6, OBJ_TYPE_FLAG_WEAK_REF, "Options" },
#define obj_type_id_SetSocketOption 7
#define obj_type_SetSocketOption (obj_types[obj_type_id_SetSocketOption])
  { NULL, 7, OBJ_TYPE_FLAG_WEAK_REF, "SetSocketOption" },
#define obj_type_id_GetSocketOption 8
#define obj_type_GetSocketOption (obj_types[obj_type_id_GetSocketOption])
  { NULL, 8, OBJ_TYPE_FLAG_WEAK_REF, "GetSocketOption" },
#define obj_type_id_LSocketFD 9
#define obj_type_LSocketFD (obj_types[obj_type_id_LSocketFD])
  { NULL, 9, OBJ_TYPE_SIMPLE, "LSocketFD" },
#define obj_type_id_LIOBuffer 10
#define obj_type_LIOBuffer (obj_types[obj_type_id_LIOBuffer])
  { NULL, 10, OBJ_TYPE_SIMPLE, "LIOBuffer" },
  {NULL, -1, 0, NULL},
};


#if LUAJIT_FFI

/* nobj_ffi_support_enabled_hint should be set to 1 when FFI support is enabled in at-least one
 * instance of a LuaJIT state.  It should never be set back to 0. */
static int nobj_ffi_support_enabled_hint = 0;
static const char nobj_ffi_support_key[] = "LuaNativeObject_FFI_SUPPORT";
static const char nobj_check_ffi_support_code[] =
"local stat, ffi=pcall(require,\"ffi\")\n" /* try loading LuaJIT`s FFI module. */
"if not stat then return false end\n"
"return true\n";

static int nobj_check_ffi_support(lua_State *L) {
	int rc;
	int err;

	/* check if ffi test has already been done. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	if(!lua_isnil(L, -1)) {
		rc = lua_toboolean(L, -1);
		lua_pop(L, 1);
		/* use results of previous check. */
		goto finished;
	}
	lua_pop(L, 1); /* pop nil. */

	err = luaL_loadbuffer(L, nobj_check_ffi_support_code,
		sizeof(nobj_check_ffi_support_code) - 1, nobj_ffi_support_key);
	if(0 == err) {
		err = lua_pcall(L, 0, 1, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Error when checking for FFI-support: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
		return 0;
	}
	/* check results of test. */
	rc = lua_toboolean(L, -1);
	lua_pop(L, 1); /* pop results. */
		/* cache results. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_pushboolean(L, rc);
	lua_rawset(L, LUA_REGISTRYINDEX);

finished:
	/* turn-on hint that there is FFI code enabled. */
	if(rc) {
		nobj_ffi_support_enabled_hint = 1;
	}

	return rc;
}

typedef struct {
	const char **ffi_init_code;
	int offset;
} nobj_reader_state;

static const char *nobj_lua_Reader(lua_State *L, void *data, size_t *size) {
	nobj_reader_state *state = (nobj_reader_state *)data;
	const char *ptr;

	(void)L;
	ptr = state->ffi_init_code[state->offset];
	if(ptr != NULL) {
		*size = strlen(ptr);
		state->offset++;
	} else {
		*size = 0;
	}
	return ptr;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code[], const ffi_export_symbol *ffi_exports, int priv_table)
{
	nobj_reader_state state = { ffi_init_code, 0 };
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym.data);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = lua_load_no_mode(L, nobj_lua_Reader, &state, ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		err = lua_pcall(L, 3, 0, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Failed to install FFI-based bindings: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
	}
	return err;
}
#endif


typedef struct {
	void *impl;
	void *obj;
} obj_implement;

static FUNC_UNUSED void *obj_implement_luaoptional(lua_State *L, int _index, void **impl, char *if_name) {
	void *ud;
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	/* get the implements table for this interface. */
	lua_pushlightuserdata(L, if_name);
	lua_rawget(L, LUA_REGISTRYINDEX);

	/* get pointer to userdata value & check if it is a userdata value. */
	ud = (obj_implement *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* get the userdata's metatable */
		if(lua_getmetatable(L, _index)) {
			/* lookup metatable in interface table for this object's implementation of the interface. */
			lua_gettable(L, -2);
		} else {
			/* no metatable. */
			goto no_interface;
		}
#if LUAJIT_FFI
	} else if(nobj_ffi_support_enabled_hint) { /* handle cdata. */
		/* get cdata interface check function from interface table. */
		lua_getfield(L, -1, "cdata");
		if(lua_isfunction(L, -1)) {
			/* pass cdata to function, return value should be an implmentation. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			/* get pointer to cdata. */
			ud = (void *)lua_topointer(L, _index);
		} else {
			lua_pop(L, 1); /* pop non-function. */
			goto no_interface;
		}
#endif
	} else {
		goto no_interface;
	}

	if(!lua_isnil(L, -1)) {
		*impl = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop interface table & implementation. */
		/* object implements interface. */
		return ud;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
no_interface:
	lua_pop(L, 1); /* pop interface table. */
	return NULL;
}

static FUNC_UNUSED void *obj_implement_luacheck(lua_State *L, int _index, void **impl, char *type) {
	void *ud = obj_implement_luaoptional(L, _index, impl, type);
	if(ud == NULL) {
#define ERROR_BUFFER_SIZE 256
		char buf[ERROR_BUFFER_SIZE];
		snprintf(buf, ERROR_BUFFER_SIZE-1,"Expected object with %s interface", type);
		/* value doesn't implement this interface. */
		luaL_argerror(L, _index, buf);
	}
	return ud;
}

/* use static pointer as key to interfaces table. (version 1.0) */
static char obj_interfaces_table_key[] = "obj_interfaces<1.0>_table_key";

static void obj_get_global_interfaces_table(lua_State *L) {
	/* get global interfaces table. */
	lua_getfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	if(lua_isnil(L, -1)) {
		/* Need to create global interfaces table. */
		lua_pop(L, 1); /* pop nil */
		lua_createtable(L, 0, 4); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		/* store interfaces table in Lua registery. */
		lua_setfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	}
}

static void obj_get_interface(lua_State *L, const char *name, int global_if_tab) {
	/* get a interface's implementation table */
	lua_getfield(L, global_if_tab, name);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil */
		/* new interface. (i.e. no object implement it yet.)
		 *
		 * create an empty table for this interface that will be used when an
		 * implementation is registered for this interface.
		 */
		lua_createtable(L, 0, 2); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		lua_setfield(L, global_if_tab, name); /* store interface in global interfaces table. */
	}
}

static int obj_get_userdata_interface(lua_State *L) {
	/* get the userdata's metatable */
	if(lua_getmetatable(L, 2)) {
		/* lookup metatable in interface table for the userdata's implementation of the interface. */
		lua_gettable(L, 1);
		if(!lua_isnil(L, -1)) {
			/* return the implementation. */
			return 1;
		}
	}
	/* no metatable or no implementation. */
	return 0;
}

static void obj_interface_register(lua_State *L, char *name, int global_if_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, name, global_if_tab);

	/* check for 'userdata' function. */
	lua_getfield(L, -1, "userdata");
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* add C function for getting a userdata's implementation. */
		lua_pushcfunction(L, obj_get_userdata_interface);
		lua_setfield(L, -2, "userdata");
	} else {
		/* already have function. */
		lua_pop(L, 1); /* pop C function. */
	}
	/* we are going to use a lightuserdata pointer for fast lookup of the interface's impl. table. */
	lua_pushlightuserdata(L, name);
	lua_insert(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);
}

static void obj_register_interfaces(lua_State *L, char *interfaces[]) {
	int i;
	int if_tab;
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(i = 0; interfaces[i] != NULL ; i++) {
		obj_interface_register(L, interfaces[i], if_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

static void obj_type_register_implement(lua_State *L, const reg_impl *impl, int global_if_tab, int mt_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, impl->if_name, global_if_tab);

	/* register object's implement in the interface table. */
	lua_pushvalue(L, mt_tab);
	lua_pushlightuserdata(L, (void *)impl->impl);
	lua_settable(L, -3);

	lua_pop(L, 1); /* pop inteface table. */
}

static void obj_type_register_implements(lua_State *L, const reg_impl *impls) {
	int if_tab;
	int mt_tab;
	/* get absolute position of object's metatable. */
	mt_tab = lua_gettop(L);
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(; impls->if_name != NULL ; impls++) {
		obj_type_register_implement(L, impls, if_tab, mt_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

/* For Lua 5.2 don't register modules as globals. */
#if LUA_VERSION_NUM == 502
#undef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED int obj_import_external_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the metatable. */
		/* REGISTERY[lightuserdata<type>] = REGISTERY[type->name] */
		lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
		lua_pushvalue(L, -2); /* dup. type's metatable. */
		lua_rawset(L, LUA_REGISTRYINDEX); /* save external type's metatable. */
		/* NOTE: top of Lua stack still has the type's metatable. */
		return 1;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED int obj_import_external_ffi_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the C check function. */
		/* _priv_table[lightuserdata<type>] = REGISTERY[type->name].c_check */
		lua_getfield(L, -1, "c_check");
		lua_remove(L, -2); /* remove metatable. */
		if(lua_isfunction(L, -1)) {
			lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
			lua_pushvalue(L, -2); /* dup. check function */
			lua_rawset(L, -4); /* save check function to module's private table. */
			/* NOTE: top of Lua stack still has the type's C check function. */
			return 1;
		} else {
			lua_pop(L, 1); /* pop non function value. */
		}
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_rawlen(L, _index);
	if(len != sizeof(obj_udata)) {
		/* This shouldn't be possible */
		luaL_error(L, "invalid userdata size: size=%d, expected=%d", len, sizeof(obj_udata));
	}
	return ud;
}

static FUNC_UNUSED int obj_udata_is_compatible(lua_State *L, obj_udata *ud, void **obj, base_caster_t *caster, obj_type *type) {
	obj_base *base;
	obj_type *ud_type;
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
	} else if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		if((type->flags & OBJ_TYPE_IMPORT) == 0) {
			/* can't resolve internal type. */
			luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
		}
		/* try to import external type. */
		if(obj_import_external_type(L, type)) {
			/* imported type, re-try metatable check. */
			goto recheck_metatable;
		}
		/* External type not yet available, so the object can't be compatible. */
	} else {
		/* Different types see if we can cast to the required type. */
		lua_rawgeti(L, -2, type->id);
		base = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop obj_base or nil */
		if(base != NULL) {
			*caster = base->bcaster;
			/* get the obj_type for this userdata. */
			lua_pushliteral(L, ".type");
			lua_rawget(L, -3); /* type's metatable. */
			ud_type = lua_touserdata(L, -1);
			lua_pop(L, 1); /* pop obj_type or nil */
			if(base == NULL) {
				luaL_error(L, "bad userdata, missing type info.");
				return 0;
			}
			/* check if userdata is a simple object. */
			if(ud_type->flags & OBJ_TYPE_SIMPLE) {
				*obj = ud;
			} else {
				*obj = ud->obj;
			}
			return 1;
		}
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_luacheck_internal(lua_State *L, int _index, void **obj, obj_type *type, int not_delete) {
	obj_udata *ud;
	base_caster_t caster = NULL;
	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			if(obj_udata_is_compatible(L, ud, obj, &(caster), type)) {
				lua_pop(L, 2); /* pop both metatables. */
				/* apply caster function if needed. */
				if(caster != NULL && *obj != NULL) {
					caster(obj);
				}
				/* check object pointer. */
				if(*obj == NULL) {
					if(not_delete) {
						luaL_error(L, "null %s", type->name); /* object was garbage collected? */
					}
					return NULL;
				}
				return ud;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				*obj = *(void **)lua_topointer(L, _index);
				return ud;
			}
			lua_pop(L, 2);
		} else {
			lua_pop(L, 1); /* pop nil. */
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	if(not_delete) {
		luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	}
	return NULL;
}

static FUNC_UNUSED void *obj_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	if(lua_isnoneornil(L, _index)) {
		return obj;
	}
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;
	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);
}

static FUNC_UNUSED void *obj_udata_luadelete_weak(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* remove object from weak table. */
	lua_pushlightuserdata(L, obj);
	lua_pushnil(L);
	lua_rawset(L, -3);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush_weak(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;

	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* lookup userdata instance from pointer. */
	lua_pushlightuserdata(L, obj);
	lua_rawget(L, -2);
	if(!lua_isnil(L, -1)) {
		lua_remove(L, -2);     /* remove objects table. */
		return;
	}
	lua_pop(L, 1);  /* pop nil. */

#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		lua_remove(L, -2);
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	/* add weak reference to object. */
	lua_pushlightuserdata(L, obj); /* push object pointer as the 'key' */
	lua_pushvalue(L, -2);          /* push object's udata */
	lua_rawset(L, -4);             /* add weak reference to object. */
	lua_remove(L, -2);     /* remove objects table. */
}

/* default object equal method. */
static FUNC_UNUSED int obj_udata_default_equal(lua_State *L) {
	obj_udata *ud1 = obj_udata_toobj(L, 1);
	obj_udata *ud2 = obj_udata_toobj(L, 2);

	lua_pushboolean(L, (ud1->obj == ud2->obj));
	return 1;
}

/* default object tostring method. */
static FUNC_UNUSED int obj_udata_default_tostring(lua_State *L) {
	obj_udata *ud = obj_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p, flags=%d", ud->obj, ud->flags);
	lua_concat(L, 2);

	return 1;
}

/*
 * Simple userdata objects.
 */
static FUNC_UNUSED void *obj_simple_udata_toobj(lua_State *L, int _index) {
	void *ud;

	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	return ud;
}

static FUNC_UNUSED void * obj_simple_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *ud;
	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			lua_pushlightuserdata(L, type);
			lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			} else if(lua_isnil(L, -1)) {
				lua_pop(L, 1); /* pop nil. */
				if((type->flags & OBJ_TYPE_IMPORT) == 0) {
					/* can't resolve internal type. */
					luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
				}
				/* try to import external type. */
				if(obj_import_external_type(L, type)) {
					/* imported type, re-try metatable check. */
					goto recheck_metatable;
				}
				/* External type not yet available, so the object can't be compatible. */
				return 0;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				return (void *)lua_topointer(L, _index);
			}
		} else {
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	return obj_simple_udata_luacheck(L, _index, type);
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	void *ud;
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_call(L, 1, 1);
		return obj;
	}
#endif
	/* create new userdata. */
	ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_rawlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_rawlen(L, 2);

	if(len1 == len2) {
		lua_pushboolean(L, (memcmp(ud1, ud2, len1) == 0));
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

/* default simple object tostring method. */
static FUNC_UNUSED int obj_simple_udata_default_tostring(lua_State *L) {
	void *ud = obj_simple_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p", ud);
	lua_concat(L, 2);

	return 1;
}

static int obj_constructor_call_wrapper(lua_State *L) {
	/* replace '__call' table with constructor function. */
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_replace(L, 1);

	/* call constructor function with all parameters after the '__call' table. */
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	/* return all results from constructor. */
	return lua_gettop(L);
}

static void obj_type_register_constants(lua_State *L, const obj_const *constants, int tab_idx,
		int bidirectional) {
	/* register constants. */
	while(constants->name != NULL) {
		lua_pushstring(L, constants->name);
		switch(constants->type) {
		case CONST_BOOLEAN:
			lua_pushboolean(L, constants->num != 0.0);
			break;
		case CONST_NUMBER:
			lua_pushnumber(L, constants->num);
			break;
		case CONST_STRING:
			lua_pushstring(L, constants->str);
			break;
		default:
			lua_pushnil(L);
			break;
		}
		/* map values back to keys. */
		if(bidirectional) {
			/* check if value already exists. */
			lua_pushvalue(L, -1);
			lua_rawget(L, tab_idx - 3);
			if(lua_isnil(L, -1)) {
				lua_pop(L, 1);
				/* add value->key mapping. */
				lua_pushvalue(L, -1);
				lua_pushvalue(L, -3);
				lua_rawset(L, tab_idx - 4);
			} else {
				/* value already exists. */
				lua_pop(L, 1);
			}
		}
		lua_rawset(L, tab_idx - 2);
		constants++;
	}
}

static void obj_type_register_package(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register_meta(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list;

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	/* register methods. */
	luaL_setfuncs(L, type_reg->methods, 0);

	/* create metatable table. */
	lua_newtable(L);
	luaL_setfuncs(L, type_reg->metas, 0); /* fill metatable */
	/* setmetatable on meta-object. */
	lua_setmetatable(L, -2);

	lua_pop(L, 1);  /* drop meta-object */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_Reg *reg_list;
	obj_type *type = type_reg->type;
	const obj_base *base = type_reg->bases;

	if(type_reg->req_type == REG_PACKAGE) {
		obj_type_register_package(L, type_reg);
		return;
	}
	if(type_reg->req_type == REG_META) {
		obj_type_register_meta(L, type_reg);
		return;
	}

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register "constructors" as to object's public API */
		luaL_setfuncs(L, reg_list, 0); /* fill public API table. */

		/* make public API table callable as the default constructor. */
		lua_newtable(L); /* create metatable */
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, reg_list[0].func); /* push first constructor function. */
		lua_pushcclosure(L, obj_constructor_call_wrapper, 1); /* make __call wrapper. */
		lua_rawset(L, -3);         /* metatable.__call = <default constructor> */

#if OBJ_DATA_HIDDEN_METATABLE
		lua_pushliteral(L, "__metatable");
		lua_pushboolean(L, 0);
		lua_rawset(L, -3);         /* metatable.__metatable = false */
#endif

		/* setmetatable on public API table. */
		lua_setmetatable(L, -2);

		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
	} else {
		/* register all methods as public functions. */
#if OBJ_DATA_HIDDEN_METATABLE
		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
#endif
	}

	luaL_setfuncs(L, type_reg->methods, 0); /* fill methods table. */

	luaL_newmetatable(L, type->name); /* create metatable */
	lua_pushliteral(L, ".name");
	lua_pushstring(L, type->name);
	lua_rawset(L, -3);    /* metatable['.name'] = "<object_name>" */
	lua_pushliteral(L, ".type");
	lua_pushlightuserdata(L, type);
	lua_rawset(L, -3);    /* metatable['.type'] = lightuserdata -> obj_type */
	lua_pushlightuserdata(L, type);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, LUA_REGISTRYINDEX);    /* REGISTRY[type] = metatable */

	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */

	luaL_setfuncs(L, type_reg->metas, 0); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2, type_reg->bidirectional_consts);

	obj_type_register_implements(L, type_reg->implements);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table */
	lua_rawset(L, -3);                  /* metatable.__index = methods */
#if OBJ_DATA_HIDDEN_METATABLE
	lua_pushliteral(L, "__metatable");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);                  /* hide metatable:
	                                       metatable.__metatable = false */
#endif
	lua_pop(L, 2);                      /* drop metatable & methods */
}

static FUNC_UNUSED int lua_checktype_ref(lua_State *L, int _index, int _type) {
	luaL_checktype(L,_index,_type);
	lua_pushvalue(L,_index);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

/* use static pointer as key to weak callback_state table. */
static char obj_callback_state_weak_ref_key[] = "obj_callback_state_weak_ref_key";

static FUNC_UNUSED void *nobj_get_callback_state(lua_State *L, int owner_idx, int size) {
	void *cb_state;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* create weak table for callback_state */
		lua_newtable(L);               /* weak table. */
		lua_newtable(L);               /* metatable for weak table. */
		lua_pushliteral(L, "__mode");
		lua_pushliteral(L, "k");
		lua_rawset(L, -3);             /* metatable.__mode = 'k'  weak keys. */
		lua_setmetatable(L, -2);       /* add metatable to weak table. */
		lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
		lua_pushvalue(L, -2);          /* dup weak table. */
		lua_rawset(L, LUA_REGISTRYINDEX);  /* add weak table to registry. */
	}

	/* check weak table for callback_state. */
	lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
		/* create new callback state. */
		cb_state = lua_newuserdata(L, size);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	} else {
		/* got existing callback state. */
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop <weak table>, <callback_state> */
	}

	return cb_state;
}

static FUNC_UNUSED void *nobj_delete_callback_state(lua_State *L, int owner_idx) {
	void *cb_state = NULL;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil.  no weak table, so there is no callback state. */
		return NULL;
	}
	/* get callback state. */
	lua_pushvalue(L, owner_idx); /* dup. owner */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2); /* pop <weak table>, nil.  No callback state for the owner. */
	} else {
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop <state> */
		/* remove callback state. */
		lua_pushvalue(L, owner_idx); /* dup. owner */
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	}

	return cb_state;
}





typedef struct MutableBuffer_if {
  uint8_t * (* const data)(void *this_v);
  size_t (* const get_size)(void *this_v);
} MutableBufferIF;

/* a per-module unique pointer for fast lookup of an interface's implementation table. */
static char obj_interface_MutableBufferIF[] = "MutableBufferIF";

#define MutableBufferIF_VAR(var_name) \
	MutableBufferIF *var_name ## _if; \
	void *var_name;

#define MutableBufferIF_LUA_OPTIONAL(L, _index, var_name) \
	var_name = obj_implement_luaoptional(L, _index, (void **)&(var_name ## _if), \
		obj_interface_MutableBufferIF)

#define MutableBufferIF_LUA_CHECK(L, _index, var_name) \
	var_name = obj_implement_luacheck(L, _index, (void **)&(var_name ## _if), \
		obj_interface_MutableBufferIF)






typedef struct Buffer_if {
  const uint8_t * (* const const_data)(void *this_v);
  size_t (* const get_size)(void *this_v);
} BufferIF;

/* a per-module unique pointer for fast lookup of an interface's implementation table. */
static char obj_interface_BufferIF[] = "BufferIF";

#define BufferIF_VAR(var_name) \
	BufferIF *var_name ## _if; \
	void *var_name;

#define BufferIF_LUA_OPTIONAL(L, _index, var_name) \
	var_name = obj_implement_luaoptional(L, _index, (void **)&(var_name ## _if), \
		obj_interface_BufferIF)

#define BufferIF_LUA_CHECK(L, _index, var_name) \
	var_name = obj_implement_luacheck(L, _index, (void **)&(var_name ## _if), \
		obj_interface_BufferIF)




static char *obj_interfaces[] = {
  obj_interface_MutableBufferIF,
  obj_interface_BufferIF,
  NULL,
};

/**
 * LIOBuffer implements Buffer interface
 */
/** 
 * Buffer interface method const_data
 */
static const uint8_t * LIOBuffer_Buffer_const_data(void *this_v) {
  LIOBuffer * this_p = this_v;
  return this_p->buf;
}
/** 
 * Buffer interface method get_size
 */
static size_t LIOBuffer_Buffer_get_size(void *this_v) {
  LIOBuffer * this_p = this_v;
  return this_p->_size;
}

static const BufferIF LIOBuffer_Buffer = {
  LIOBuffer_Buffer_const_data,
  LIOBuffer_Buffer_get_size
};

/**
 * LIOBuffer implements MutableBuffer interface
 */
/** 
 * MutableBuffer interface method data
 */
static uint8_t * LIOBuffer_MutableBuffer_data(void *this_v) {
  LIOBuffer * this_p = this_v;
  return this_p->buf;
}
/** 
 * MutableBuffer interface method get_size
 */
static size_t LIOBuffer_MutableBuffer_get_size(void *this_v) {
  LIOBuffer * this_p = this_v;
  return this_p->_capacity;
}

static const MutableBufferIF LIOBuffer_MutableBuffer = {
  LIOBuffer_MutableBuffer_data,
  LIOBuffer_MutableBuffer_get_size
};



#define obj_type_LSockAddr_check(L, _index) \
	(LSockAddr *)obj_simple_udata_luacheck(L, _index, &(obj_type_LSockAddr))
#define obj_type_LSockAddr_optional(L, _index) \
	(LSockAddr *)obj_simple_udata_luaoptional(L, _index, &(obj_type_LSockAddr))
#define obj_type_LSockAddr_delete(L, _index) \
	(LSockAddr *)obj_simple_udata_luadelete(L, _index, &(obj_type_LSockAddr))
#define obj_type_LSockAddr_push(L, obj) \
	obj_simple_udata_luapush(L, obj, sizeof(LSockAddr), &(obj_type_LSockAddr))

#define obj_type_LAddrInfo_check(L, _index) \
	(LAddrInfo *)obj_simple_udata_luacheck(L, _index, &(obj_type_LAddrInfo))
#define obj_type_LAddrInfo_optional(L, _index) \
	(LAddrInfo *)obj_simple_udata_luaoptional(L, _index, &(obj_type_LAddrInfo))
#define obj_type_LAddrInfo_delete(L, _index) \
	(LAddrInfo *)obj_simple_udata_luadelete(L, _index, &(obj_type_LAddrInfo))
#define obj_type_LAddrInfo_push(L, obj) \
	obj_simple_udata_luapush(L, obj, sizeof(LAddrInfo), &(obj_type_LAddrInfo))

#define obj_type_LSocketFD_check(L, _index) \
	*((LSocketFD *)obj_simple_udata_luacheck(L, _index, &(obj_type_LSocketFD)))
#define obj_type_LSocketFD_optional(L, _index) \
	*((LSocketFD *)obj_simple_udata_luaoptional(L, _index, &(obj_type_LSocketFD)))
#define obj_type_LSocketFD_delete(L, _index) \
	*((LSocketFD *)obj_simple_udata_luadelete(L, _index, &(obj_type_LSocketFD)))
#define obj_type_LSocketFD_push(L, obj) \
	obj_simple_udata_luapush(L, &(obj), sizeof(LSocketFD), &(obj_type_LSocketFD))

#define obj_type_LIOBuffer_check(L, _index) \
	(LIOBuffer *)obj_simple_udata_luacheck(L, _index, &(obj_type_LIOBuffer))
#define obj_type_LIOBuffer_optional(L, _index) \
	(LIOBuffer *)obj_simple_udata_luaoptional(L, _index, &(obj_type_LIOBuffer))
#define obj_type_LIOBuffer_delete(L, _index) \
	(LIOBuffer *)obj_simple_udata_luadelete(L, _index, &(obj_type_LIOBuffer))
#define obj_type_LIOBuffer_push(L, obj) \
	obj_simple_udata_luapush(L, obj, sizeof(LIOBuffer), &(obj_type_LIOBuffer))




static const char *llnet_ffi_lua_code[] = { "local ffi=require\"ffi\"\n"
"local function ffi_safe_load(name, global)\n"
"	local stat, C = pcall(ffi.load, name, global)\n"
"	if not stat then return nil, C end\n"
"	if global then return ffi.C end\n"
"	return C\n"
"end\n"
"local function ffi_load(name, global)\n"
"	return assert(ffi_safe_load(name, global))\n"
"end\n"
"\n"
"local function ffi_string(ptr)\n"
"	if ptr ~= nil then\n"
"		return ffi.string(ptr)\n"
"	end\n"
"	return nil\n"
"end\n"
"\n"
"local function ffi_string_len(ptr, len)\n"
"	if ptr ~= nil then\n"
"		return ffi.string(ptr, len)\n"
"	end\n"
"	return nil\n"
"end\n"
"\n"
"local f_cast = ffi.cast\n"
"local pcall = pcall\n"
"local error = error\n"
"local type = type\n"
"local tonumber = tonumber\n"
"local tostring = tostring\n"
"local sformat = require\"string\".format\n"
"local rawset = rawset\n"
"local setmetatable = setmetatable\n"
"local package = (require\"package\") or {}\n"
"local p_config = package.config\n"
"local p_cpath = package.cpath\n"
"\n"
"\n"
"local ffi_load_cmodule\n"
"\n"
"-- try to detect luvit.\n"
"if p_config == nil and p_cpath == nil then\n"
"	ffi_load_cmodule = function(name, global)\n"
"		for path,module in pairs(package.loaded) do\n"
"			if module == name then\n"
"				local C, err = ffi_safe_load(path, global)\n"
"				-- return opened library\n"
"				if C then return C end\n"
"			end\n"
"		end\n"
"		error(\"Failed to find: \" .. name)\n"
"	end\n"
"else\n"
"	ffi_load_cmodule = function(name, global)\n"
"		local dir_sep = p_config:sub(1,1)\n"
"		local path_sep = p_config:sub(3,3)\n"
"		local path_mark = p_config:sub(5,5)\n"
"		local path_match = \"([^\" .. path_sep .. \"]*)\" .. path_sep\n"
"		-- convert dotted name to directory path.\n"
"		name = name:gsub('%.', dir_sep)\n"
"		-- try each path in search path.\n"
"		for path in p_cpath:gmatch(path_match) do\n"
"			local fname = path:gsub(path_mark, name)\n"
"			local C, err = ffi_safe_load(fname, global)\n"
"			-- return opened library\n"
"			if C then return C end\n"
"		end\n"
"		error(\"Failed to find: \" .. name)\n"
"	end\n"
"end\n"
"\n"
"local _M, _priv, reg_table = ...\n"
"local REG_OBJECTS_AS_GLOBALS = false\n"
"local C = ffi.C\n"
"\n"
"local OBJ_UDATA_FLAG_OWN		= 1\n"
"\n"
"local function ffi_safe_cdef(block_name, cdefs)\n"
"	local fake_type = \"struct sentinel_\" .. block_name .. \"_ty\"\n"
"	local stat, size = pcall(ffi.sizeof, fake_type)\n"
"	if stat and size > 0 then\n"
"		-- already loaded this cdef block\n"
"		return\n"
"	end\n"
"	cdefs = fake_type .. \"{ int a; int b; int c; };\" .. cdefs\n"
"	return ffi.cdef(cdefs)\n"
"end\n"
"\n"
"ffi_safe_cdef(\"LuaNativeObjects\", [[\n"
"\n"
"typedef struct obj_type obj_type;\n"
"\n"
"typedef void (*base_caster_t)(void **obj);\n"
"\n"
"typedef void (*dyn_caster_t)(void **obj, obj_type **type);\n"
"\n"
"struct obj_type {\n"
"	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */\n"
"	int32_t         id;       /**< type's id. */\n"
"	uint32_t        flags;    /**< type's flags (weak refs) */\n"
"	const char      *name;    /**< type's object name. */\n"
"};\n"
"\n"
"typedef struct obj_base {\n"
"	int32_t        id;\n"
"	base_caster_t  bcaster;\n"
"} obj_base;\n"
"\n"
"typedef struct obj_udata {\n"
"	void     *obj;\n"
"	uint32_t flags;  /**< lua_own:1bit */\n"
"} obj_udata;\n"
"\n"
"int memcmp(const void *s1, const void *s2, size_t n);\n"
"\n"
"]])\n"
"\n"
"local nobj_callback_states = {}\n"
"local nobj_weak_objects = setmetatable({}, {__mode = \"v\"})\n"
"local nobj_obj_flags = {}\n"
"\n"
"local function obj_ptr_to_id(ptr)\n"
"	return tonumber(f_cast('uintptr_t', ptr))\n"
"end\n"
"\n"
"local function obj_to_id(ptr)\n"
"	return tonumber(f_cast('uintptr_t', f_cast('void *', ptr)))\n"
"end\n"
"\n"
"local function register_default_constructor(_pub, obj_name, constructor)\n"
"	local obj_pub = _pub[obj_name]\n"
"	if type(obj_pub) == 'table' then\n"
"		-- copy table since it might have a locked metatable\n"
"		local new_pub = {}\n"
"		for k,v in pairs(obj_pub) do\n"
"			new_pub[k] = v\n"
"		end\n"
"		setmetatable(new_pub, { __call = function(t,...)\n"
"			return constructor(...)\n"
"		end,\n"
"		__metatable = false,\n"
"		})\n"
"		obj_pub = new_pub\n"
"	else\n"
"		obj_pub = constructor\n"
"	end\n"
"	_pub[obj_name] = obj_pub\n"
"	_M[obj_name] = obj_pub\n"
"	if REG_OBJECTS_AS_GLOBALS then\n"
"		_G[obj_name] = obj_pub\n"
"	end\n"
"end\n"
"\n"
"ffi_safe_cdef(\"MutableBufferIF\", [[\n"
"typedef struct MutableBuffer_if {\n"
"  uint8_t * (* const data)(void *this_v);\n"
"  size_t (* const get_size)(void *this_v);\n"
"} MutableBufferIF;\n"
"]])\n"
"\n"
"ffi_safe_cdef(\"BufferIF\", [[\n"
"typedef struct Buffer_if {\n"
"  const uint8_t * (* const const_data)(void *this_v);\n"
"  size_t (* const get_size)(void *this_v);\n"
"} BufferIF;\n"
"]])\n"
"\n"
"local Cmod = ffi_load_cmodule(\"llnet\", false)\n"
"local C = Cmod\n"
"\n"
"ffi.cdef[[\n"
"typedef int errno_rc;\n"
"\n"
"typedef int eai_rc;\n"
"\n"
"typedef struct LSockAddr LSockAddr;\n"
"typedef struct LAddrInfo LAddrInfo;\n"
"typedef int LSocketFD;\n"
"typedef struct LIOBuffer LIOBuffer;\n"
"\n"
"]]\n"
"\n"
"ffi.cdef[[\n"
"typedef struct sockaddr sockaddr;\n"
"typedef int socklen_t;\n"
"typedef int sa_family_t;\n"
"\n"
"struct LSockAddr {\n"
"	struct sockaddr  *_addr;\n"
"	socklen_t        _addrlen;\n"
"};\n"
"\n"
"typedef struct LSockAddr LSockAddr;\n"
"\n"
"int l_sockaddr_tostring(LSockAddr *addr, char *buf, size_t buf_len);\n"
"\n"
"\n"
"\n"
"int l_sockaddr_init(LSockAddr *);\n"
"\n"
"int l_sockaddr_set_ip_port(LSockAddr *, const char *, int);\n"
"\n"
"int l_sockaddr_set_unix(LSockAddr *, const char *);\n"
"\n"
"int l_sockaddr_set_family(LSockAddr *, sa_family_t);\n"
"\n"
"void l_sockaddr_cleanup(LSockAddr *);\n"
"\n"
"void l_sockaddr_resize(LSockAddr *, socklen_t);\n"
"\n"
"sa_family_t l_sockaddr_get_family(LSockAddr *);\n"
"\n"
"int l_sockaddr_set_port(LSockAddr *, int);\n"
"\n"
"int l_sockaddr_get_port(LSockAddr *);\n"
"\n"
"sockaddr * l_sockaddr_get_addr(LSockAddr *);\n"
"\n"
"socklen_t l_sockaddr_get_addrlen(LSockAddr *);\n"
"\n"
"eai_rc l_sockaddr_lookup_full(LSockAddr *, const char *, const char *, int, int, int, int);\n"
"\n"
"struct LAddrInfo {\n"
"	struct addrinfo  *_res;\n"
"	struct addrinfo  *_cur;\n"
"};\n"
"\n"
"typedef struct LAddrInfo LAddrInfo;\n"
"\n"
"eai_rc l_addrinfo_init_ip(LAddrInfo *, const char *, const char *);\n"
"\n"
"eai_rc l_addrinfo_init_ipv4(LAddrInfo *, const char *, const char *);\n"
"\n"
"eai_rc l_addrinfo_init_ipv6(LAddrInfo *, const char *, const char *);\n"
"\n"
"eai_rc l_addrinfo_init_full(LAddrInfo *, const char *, const char *, int, int, int, int);\n"
"\n"
"void l_addrinfo_cleanup(LAddrInfo *);\n"
"\n"
"bool l_addrinfo_first(LAddrInfo *);\n"
"\n"
"int l_addrinfo_get_addr(LAddrInfo *, LSockAddr *);\n"
"\n"
"const char * l_addrinfo_get_canonname(LAddrInfo *);\n"
"\n"
"int l_addrinfo_get_family(LAddrInfo *);\n"
"\n"
"int l_addrinfo_get_socktype(LAddrInfo *);\n"
"\n"
"int l_addrinfo_get_protocol(LAddrInfo *);\n"
"\n"
"bool l_addrinfo_next(LAddrInfo *);\n"
"\n"
"errno_rc lsocket_opt_set_IP_RECVOPTS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_MTU(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_TOS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_OPTIONS(LSocketFD, const char *, socklen_t);\n"
"\n"
"errno_rc lsocket_opt_set_IP_TTL(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_RECVTOS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_MINTTL(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_MULTICAST_TTL(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_RECVTTL(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_FREEBIND(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_PKTINFO(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_HDRINCL(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_MTU_DISCOVER(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_RETOPTS(LSocketFD, const char *, socklen_t);\n"
"\n"
"errno_rc lsocket_opt_set_IP_RECVERR(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_ROUTER_ALERT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_RECVRETOPTS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_RECVORIGDSTADDR(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IP_MULTICAST_LOOP(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_MULTICAST_HOPS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_RECVERR(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_CHECKSUM(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_MULTICAST_LOOP(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_MULTICAST_IF(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_RECVRTHDR(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_MTU_DISCOVER(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_UNICAST_HOPS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_NEXTHOP(LSocketFD, LSockAddr *);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_RECVHOPLIMIT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_MTU(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_HOPLIMIT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_ADDRFORM(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_V6ONLY(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_RECVDSTOPTS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_TCLASS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_RECVTCLASS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_RECVHOPOPTS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_RECVPKTINFO(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_IPV6_ROUTER_ALERT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_SNDBUFFORCE(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_TIMESTAMP(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_SNDBUF(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_NO_CHECK(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_BSDCOMPAT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_REUSEADDR(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_MARK(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_TIMESTAMPING(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_TIMESTAMPNS(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_BINDTODEVICE(LSocketFD, const char *, socklen_t);\n"
"\n"
"errno_rc lsocket_opt_set_SO_DONTROUTE(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_RCVBUFFORCE(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_SNDLOWAT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_OOBINLINE(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_RCVLOWAT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_PASSCRED(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_RCVBUF(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_PRIORITY(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_BROADCAST(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_KEEPALIVE(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_SO_DEBUG(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_CORK(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_KEEPIDLE(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_QUICKACK(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_CONGESTION(LSocketFD, const char *, socklen_t);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_WINDOW_CLAMP(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_DEFER_ACCEPT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_MAXSEG(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_NODELAY(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_KEEPCNT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_LINGER2(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_SYNCNT(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_set_TCP_KEEPINTVL(LSocketFD, int);\n"
"\n"
"errno_rc lsocket_opt_get_IP_RECVOPTS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_MTU(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_TOS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_OPTIONS(LSocketFD, char *, socklen_t*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_TTL(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_RECVTOS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_MINTTL(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_MULTICAST_TTL(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_RECVTTL(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_FREEBIND(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_PKTINFO(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_HDRINCL(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_MTU_DISCOVER(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_RETOPTS(LSocketFD, char *, socklen_t*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_RECVERR(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_ROUTER_ALERT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_RECVRETOPTS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_RECVORIGDSTADDR(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IP_MULTICAST_LOOP(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_MULTICAST_HOPS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_RECVERR(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_CHECKSUM(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_MULTICAST_LOOP(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_MULTICAST_IF(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_RECVRTHDR(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_MTU_DISCOVER(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_UNICAST_HOPS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_NEXTHOP(LSocketFD, LSockAddr *);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_RECVHOPLIMIT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_MTU(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_HOPLIMIT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_V6ONLY(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_RECVDSTOPTS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_TCLASS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_RECVTCLASS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_RECVHOPOPTS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_RECVPKTINFO(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_IPV6_ROUTER_ALERT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_SNDBUFFORCE(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_ACCEPTCONN(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_PROTOCOL(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_TIMESTAMP(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_SNDBUF(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_NO_CHECK(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_BSDCOMPAT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_REUSEADDR(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_MARK(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_TIMESTAMPING(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_TIMESTAMPNS(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_BINDTODEVICE(LSocketFD, char *, socklen_t*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_DONTROUTE(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_RCVBUFFORCE(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_SNDLOWAT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_OOBINLINE(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_DOMAIN(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_RCVLOWAT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_PASSCRED(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_RCVBUF(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_PRIORITY(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_TYPE(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_BROADCAST(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_KEEPALIVE(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_DEBUG(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_SO_ERROR(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_CORK(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_KEEPIDLE(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_QUICKACK(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_CONGESTION(LSocketFD, char *, socklen_t*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_WINDOW_CLAMP(LSocketFD, int*);\n"
"\n", /* ----- CUT ----- */
"errno_rc lsocket_opt_get_TCP_DEFER_ACCEPT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_MAXSEG(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_NODELAY(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_KEEPCNT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_LINGER2(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_SYNCNT(LSocketFD, int*);\n"
"\n"
"errno_rc lsocket_opt_get_TCP_KEEPINTVL(LSocketFD, int*);\n"
"\n"
"int l_socket_send(LSocketFD sock, const void *buf, size_t len, int flags);\n"
"int l_socket_recv(LSocketFD sock, void *buf, size_t len, int flags);\n"
"\n"
"LSocketFD l_socket_open(int, int, int, int);\n"
"\n"
"void l_socket_close_internal(LSocketFD);\n"
"\n"
"int l_socket_shutdown(LSocketFD, int);\n"
"\n"
"errno_rc l_socket_set_nonblock(LSocketFD, bool);\n"
"\n"
"errno_rc l_socket_connect(LSocketFD, LSockAddr *);\n"
"\n"
"errno_rc l_socket_bind(LSocketFD, LSockAddr *);\n"
"\n"
"errno_rc l_socket_listen(LSocketFD, int);\n"
"\n"
"errno_rc l_socket_get_sockname(LSocketFD, LSockAddr *);\n"
"\n"
"errno_rc l_socket_get_peername(LSocketFD, LSockAddr *);\n"
"\n"
"errno_rc l_socket_accept(LSocketFD, LSockAddr *, int);\n"
"\n"
"typedef struct LIOBuffer LIOBuffer;\n"
"\n"
"struct LIOBuffer {\n"
"	uint8_t *buf;     /**< buffer. */\n"
"	size_t  _size;     /**< size of data in buffer. */\n"
"	size_t  _capacity; /**< memory size of buffer. */\n"
"};\n"
"\n"
"void l_iobuffer_init(LIOBuffer *buf, const uint8_t *data, size_t size);\n"
"\n"
"void l_iobuffer_init_cap(LIOBuffer *buf, size_t capacity);\n"
"\n"
"bool l_iobuffer_set_capacity(LIOBuffer *buf, size_t capacity);\n"
"\n"
"\n"
"void l_iobuffer_cleanup(LIOBuffer *);\n"
"\n"
"void l_iobuffer_reset(LIOBuffer *);\n"
"\n"
"bool l_iobuffer_set_size(LIOBuffer *, size_t);\n"
"\n"
"bool l_iobuffer_set_capacity(LIOBuffer *, size_t);\n"
"\n"
"\n"
"]]\n"
"\n"
"REG_OBJECTS_AS_GLOBALS = false\n"
"local _obj_interfaces_ffi = {}\n"
"local _pub = {}\n"
"local _meth = {}\n"
"local _push = {}\n"
"local _obj_subs = {}\n"
"for obj_name,mt in pairs(_priv) do\n"
"	if type(mt) == 'table' then\n"
"		_obj_subs[obj_name] = {}\n"
"		if mt.__index then\n"
"			_meth[obj_name] = mt.__index\n"
"		end\n"
"	end\n"
"end\n"
"for obj_name,pub in pairs(_M) do\n"
"	_pub[obj_name] = pub\n"
"end\n"
"\n"
"--\n"
"-- CData Metatable access\n"
"--\n"
"local _ctypes = {}\n"
"local _type_names = {}\n"
"local _get_mt_key = {}\n"
"local _ctype_meta_map = {}\n"
"\n"
"local f_typeof = ffi.typeof\n"
"local function get_cdata_type_id(cdata)\n"
"	return tonumber(f_typeof(cdata))\n"
"end\n"
"local function get_cdata_mt(cdata)\n"
"	return _ctype_meta_map[tonumber(f_typeof(cdata))]\n"
"end\n"
"\n"
"local function obj_register_ctype(name, ctype)\n"
"	local obj_mt = _priv[name]\n"
"	local obj_type = obj_mt['.type']\n"
"	local obj_ctype = ffi.typeof(ctype)\n"
"	local obj_type_id = tonumber(obj_ctype)\n"
"	_ctypes[name] = obj_ctype\n"
"	_type_names[name] = tostring(obj_ctype)\n"
"	_ctype_meta_map[obj_type_id] = obj_mt\n"
"	_ctype_meta_map[obj_mt] = obj_type_id\n"
"	return obj_mt, obj_type, obj_ctype\n"
"end\n"
"\n"
"--\n"
"-- Interfaces helper code.\n"
"--\n"
"local _obj_interfaces_key = \"obj_interfaces<1.0>_table_key\"\n"
"local _obj_interfaces_ud = reg_table[_obj_interfaces_key]\n"
"local _obj_interfaces_key_ffi = _obj_interfaces_key .. \"_LJ2_FFI\"\n"
"_obj_interfaces_ffi = reg_table[_obj_interfaces_key_ffi]\n"
"if not _obj_interfaces_ffi then\n"
"	-- create missing interfaces table for FFI bindings.\n"
"	_obj_interfaces_ffi = {}\n"
"	reg_table[_obj_interfaces_key_ffi] = _obj_interfaces_ffi\n"
"end\n"
"\n"
"local function obj_get_userdata_interface(if_name, expected_err)\n"
"	local impls_ud = _obj_interfaces_ud[if_name]\n"
"	if not impls_ud then\n"
"		impls_ud = {}\n"
"		_obj_interfaces_ud[if_name] = impls_ud\n"
"	end\n"
"	-- create cdata check function to be used by non-ffi bindings.\n"
"	if not impls_ud.cdata then\n"
"		function impls_ud.cdata(obj)\n"
"			return assert(impls_ud[get_cdata_mt(obj)], expected_err)\n"
"		end\n"
"	end\n"
"	return impls_ud\n"
"end\n"
"\n"
"local function obj_get_interface_check(if_name, expected_err)\n"
"	local impls_ffi = _obj_interfaces_ffi[if_name]\n"
"	if not impls_ffi then\n"
"		local if_type = ffi.typeof(if_name .. \" *\")\n"
"		local impls_ud = obj_get_userdata_interface(if_name, expected_err)\n"
"		-- create table for FFI-based interface implementations.\n"
"		impls_ffi = setmetatable({}, {\n"
"		__index = function(impls_ffi, mt)\n"
"			local impl = impls_ud[mt]\n"
"			if impl then\n"
"				-- cast to cdata\n"
"				impl = if_type(impl)\n"
"				rawset(impls_ffi, mt, impl)\n"
"			end\n"
"			return impl\n"
"		end})\n"
"		_obj_interfaces_ffi[if_name] = impls_ffi\n"
"\n"
"		-- create check function for this interface.\n"
"		function impls_ffi.check(obj)\n"
"			local impl\n"
"			if type(obj) == 'cdata' then\n"
"				impl = impls_ffi[get_cdata_type_id(obj)]\n"
"			else\n"
"				impl = impls_ud.userdata(impls_ffi, obj)\n"
"			end\n"
"			return assert(impl, expected_err)\n"
"		end\n"
"	end\n"
"	return impls_ffi.check\n"
"end\n"
"\n"
"local function obj_register_interface(if_name, obj_name)\n"
"	-- loopkup cdata id\n"
"	local obj_mt = _priv[obj_name]\n"
"	local obj_type_id = _ctype_meta_map[obj_mt]\n"
"	local impl_meths = {}\n"
"	local ffi_impls = _obj_interfaces_ffi[if_name]\n"
"	ffi_impls[obj_type_id] = impl_meths\n"
"	_meth[obj_name]['NOBJ_get_' .. if_name] = impl_meths\n"
"	return impl_meths\n"
"end\n"
"\n"
"\n"
"local obj_type_LSockAddr_check\n"
"local obj_type_LSockAddr_delete\n"
"local obj_type_LSockAddr_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"LSockAddr\", \"LSockAddr\")\n"
"	local LSockAddr_sizeof = ffi.sizeof\"LSockAddr\"\n"
"\n"
"	function obj_type_LSockAddr_check(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_LSockAddr_delete(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_LSockAddr_push(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"LSockAddr: %p\", self)\n"
"	end\n"
"\n"
"	function obj_mt.__eq(val1, val2)\n"
"		if not ffi.istype(obj_type, val2) then return false end\n"
"		assert(ffi.istype(obj_type, val1), \"expected LSockAddr\")\n"
"		return (C.memcmp(val1, val2, LSockAddr_sizeof) == 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	local function c_check(obj)\n"
"		if ffi.istype(obj_type, obj) then return obj end\n"
"		return nil\n"
"	end\n"
"	_priv[obj_type] = c_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr)\n"
"		local obj = obj_ctype()\n"
"		ffi.copy(obj, ptr, LSockAddr_sizeof);\n"
"		return obj\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = c_check\n"
"	obj_mt.ffi_check = obj_type_LSockAddr_check\n"
"end\n"
"\n"
"\n"
"local obj_type_LAddrInfo_check\n"
"local obj_type_LAddrInfo_delete\n"
"local obj_type_LAddrInfo_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"LAddrInfo\", \"LAddrInfo\")\n"
"	local LAddrInfo_sizeof = ffi.sizeof\"LAddrInfo\"\n"
"\n"
"	function obj_type_LAddrInfo_check(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_LAddrInfo_delete(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_LAddrInfo_push(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"LAddrInfo: %p\", self)\n"
"	end\n"
"\n"
"	function obj_mt.__eq(val1, val2)\n"
"		if not ffi.istype(obj_type, val2) then return false end\n"
"		assert(ffi.istype(obj_type, val1), \"expected LAddrInfo\")\n"
"		return (C.memcmp(val1, val2, LAddrInfo_sizeof) == 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	local function c_check(obj)\n"
"		if ffi.istype(obj_type, obj) then return obj end\n"
"		return nil\n"
"	end\n"
"	_priv[obj_type] = c_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr)\n"
"		local obj = obj_ctype()\n"
"		ffi.copy(obj, ptr, LAddrInfo_sizeof);\n"
"		return obj\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = c_check\n"
"	obj_mt.ffi_check = obj_type_LAddrInfo_check\n"
"end\n"
"\n"
"\n"
"local obj_type_LSocketFD_check\n"
"local obj_type_LSocketFD_delete\n"
"local obj_type_LSocketFD_push\n"
"\n"
"do\n"
"	ffi_safe_cdef(\"LSocketFD_simple_wrapper\", [=[\n"
"		struct LSocketFD_t {\n"
"			const LSocketFD _wrapped_val;\n"
"		};\n"
"		typedef struct LSocketFD_t LSocketFD_t;\n"
"	]=])\n"
"\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"LSocketFD\", \"LSocketFD_t\")\n"
"\n"
"	function obj_type_LSocketFD_check(obj)\n"
"		return obj._wrapped_val\n"
"	end\n"
"\n"
"	function obj_type_LSocketFD_delete(obj)\n"
"		local id = obj_to_id(obj)\n"
"		local valid = nobj_obj_flags[id]\n"
"		if not valid then return nil end\n"
"		local val = obj._wrapped_val\n"
"		nobj_obj_flags[id] = nil\n"
"		return val\n"
"	end\n"
"\n"
"	function obj_type_LSocketFD_push(val)\n"
"		local obj = obj_ctype(val)\n"
"		local id = obj_to_id(obj)\n"
"		nobj_obj_flags[id] = true\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"LSocketFD: %d\", tonumber(self._wrapped_val))\n"
"	end\n"
"\n"
"	function obj_mt.__eq(val1, val2)\n"
"		if not ffi.istype(obj_ctype, val2) then return false end\n"
"		return (val1._wrapped_val == val2._wrapped_val)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	local function c_check(obj)\n"
"		if ffi.istype(obj_ctype, obj) then return obj._wrapped_val end\n"
"		return nil\n"
"	end\n"
"	_priv[obj_type] = c_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr)\n"
"		return obj_type_LSocketFD_push(ffi.cast(\"LSocketFD *\", ptr)[0])\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = c_check\n"
"	obj_mt.ffi_check = obj_type_LSocketFD_check\n"
"end\n"
"\n"
"\n"
"local obj_type_LIOBuffer_check\n"
"local obj_type_LIOBuffer_delete\n"
"local obj_type_LIOBuffer_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"LIOBuffer\", \"LIOBuffer\")\n"
"	local LIOBuffer_sizeof = ffi.sizeof\"LIOBuffer\"\n"
"\n"
"	function obj_type_LIOBuffer_check(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_LIOBuffer_delete(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_LIOBuffer_push(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"LIOBuffer: %p\", self)\n"
"	end\n"
"\n"
"	function obj_mt.__eq(val1, val2)\n"
"		if not ffi.istype(obj_type, val2) then return false end\n"
"		assert(ffi.istype(obj_type, val1), \"expected LIOBuffer\")\n"
"		return (C.memcmp(val1, val2, LIOBuffer_sizeof) == 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	local function c_check(obj)\n"
"		if ffi.istype(obj_type, obj) then return obj end\n"
"		return nil\n"
"	end\n"
"	_priv[obj_type] = c_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr)\n"
"		local obj = obj_ctype()\n"
"		ffi.copy(obj, ptr, LIOBuffer_sizeof);\n"
"		return obj\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = c_check\n"
"	obj_mt.ffi_check = obj_type_LIOBuffer_check\n"
"end\n"
"\n"
"\n"
"local obj_type_MutableBuffer_check =\n"
"	obj_get_interface_check(\"MutableBufferIF\", \"Expected object with MutableBuffer interface\")\n"
"\n"
"local obj_type_Buffer_check =\n"
"	obj_get_interface_check(\"BufferIF\", \"Expected object with Buffer interface\")\n"
"\n"
"\n"
"-- Start \"Errors\" FFI interface\n"
"-- End \"Errors\" FFI interface\n"
"\n"
"-- get Errors table to map errno to error name.\n"
"local Error_names = _M.Errors\n"
"\n"
"local function error_code__errno_rc__push(err)\n"
"  local err_str\n"
"	if(-1 == err) then\n"
"		err_str = Error_names[ffi.errno()]\n"
"	end\n"
"\n"
"	return err_str\n"
"end\n"
"\n"
"\n"
"-- Start \"EAI_Errors\" FFI interface\n"
"-- End \"EAI_Errors\" FFI interface\n"
"\n"
"-- get EAI_Errors table to map errno to error name.\n"
"local EAI_Error_names = _M.EAI_Errors\n"
"\n"
"local function error_code__eai_rc__push(err)\n"
"  local err_str\n"
"	if(0 ~= err) then\n"
"		err_str = EAI_Error_names[err]\n"
"	end\n"
"\n"
"	return err_str\n"
"end\n"
"\n"
"\n"
"-- Start \"Protocols\" FFI interface\n"
"-- End \"Protocols\" FFI interface\n"
"\n"
"\n"
"-- Start \"Services\" FFI interface\n"
"-- End \"Services\" FFI interface\n"
"\n"
"\n"
"-- Start \"LSockAddr\" FFI interface\n"
"-- method: new\n"
"function _pub.LSockAddr.new()\n"
"  local self = ffi.new(\"LSockAddr\")\n"
"  local rc_l_sockaddr_init2 = 0\n"
"  rc_l_sockaddr_init2 = C.l_sockaddr_init(self)\n"
"  return obj_type_LSockAddr_push(self), rc_l_sockaddr_init2\n"
"end\n"
"register_default_constructor(_pub,\"LSockAddr\",_pub.LSockAddr.new)\n"
"\n"
"-- method: ip_port\n"
"function _pub.LSockAddr.ip_port(ip1, port2)\n"
"  local ip_len1 = #ip1\n"
"  \n"
"  local self = ffi.new(\"LSockAddr\")\n"
"  local rc_l_sockaddr_init2 = 0\n"
"  local rc_l_sockaddr_set_ip_port3 = 0\n"
"  rc_l_sockaddr_init2 = C.l_sockaddr_init(self)\n"
"  rc_l_sockaddr_set_ip_port3 = C.l_sockaddr_set_ip_port(self, ip1, port2)\n"
"  return obj_type_LSockAddr_push(self), rc_l_sockaddr_init2, rc_l_sockaddr_set_ip_port3\n"
"end\n"
"\n"
"-- method: unix\n"
"function _pub.LSockAddr.unix(unix1)\n"
"  local unix_len1 = #unix1\n"
"  local self = ffi.new(\"LSockAddr\")\n"
"  local rc_l_sockaddr_init2 = 0\n"
"  local rc_l_sockaddr_set_unix3 = 0\n"
"  rc_l_sockaddr_init2 = C.l_sockaddr_init(self)\n"
"  rc_l_sockaddr_set_unix3 = C.l_sockaddr_set_unix(self, unix1)\n"
"  return obj_type_LSockAddr_push(self), rc_l_sockaddr_init2, rc_l_sockaddr_set_unix3\n"
"end\n"
"\n"
"-- method: family\n"
"function _pub.LSockAddr.family(family1)\n"
"  \n"
"  local self = ffi.new(\"LSockAddr\")\n"
"  local rc_l_sockaddr_init2 = 0\n"
"  local rc_l_sockaddr_set_family3 = 0\n"
"  rc_l_sockaddr_init2 = C.l_sockaddr_init(self)\n"
"  rc_l_sockaddr_set_family3 = C.l_sockaddr_set_family(self, family1)\n"
"  return obj_type_LSockAddr_push(self), rc_l_sockaddr_init2, rc_l_sockaddr_set_family3\n"
"end\n"
"\n"
"-- method: __gc\n"
"function _priv.LSockAddr.__gc(self)\n"
"  local self = obj_type_LSockAddr_delete(self)\n"
"  if not self then return end\n"
"  C.l_sockaddr_cleanup(self)\n"
"  return \n"
"end\n"
"\n"
"-- method: set_ip_port\n"
"function _meth.LSockAddr.set_ip_port(self, ip2, port3)\n"
"  \n"
"  local ip_len2 = #ip2\n"
"  \n"
"  local rc_l_sockaddr_set_ip_port1 = 0\n"
"  rc_l_sockaddr_set_ip_port1 = C.l_sockaddr_set_ip_port(self, ip2, port3)\n"
"  return rc_l_sockaddr_set_ip_port1\n"
"end\n"
"\n"
"-- method: set_unix\n"
"function _meth.LSockAddr.set_unix(self, unix2)\n"
"  \n"
"  local unix_len2 = #unix2\n"
"  local rc_l_sockaddr_set_unix1 = 0\n"
"  rc_l_sockaddr_set_unix1 = C.l_sockaddr_set_unix(self, unix2)\n"
"  return rc_l_sockaddr_set_unix1\n"
"end\n"
"\n"
"-- method: resize\n"
"function _meth.LSockAddr.resize(self, addrlen2)\n"
"  \n"
"  \n"
"  C.l_sockaddr_resize(self, addrlen2)\n"
"  return \n"
"end\n"
"\n"
"-- method: get_family\n"
"function _meth.LSockAddr.get_family(self)\n"
"  \n"
"  local rc_l_sockaddr_get_family1 = 0\n"
"  rc_l_sockaddr_get_family1 = C.l_sockaddr_get_family(self)\n"
"  return rc_l_sockaddr_get_family1\n"
"end\n"
"\n"
"-- method: set_port\n"
"function _meth.LSockAddr.set_port(self, port2)\n"
"  \n"
"  \n"
"  local rc_l_sockaddr_set_port1 = 0\n"
"  rc_l_sockaddr_set_port1 = C.l_sockaddr_set_port(self, port2)\n"
"  return rc_l_sockaddr_set_port1\n"
"end\n"
"\n"
"-- method: get_port\n"
"function _meth.LSockAddr.get_port(self)\n"
"  \n"
"  local rc_l_sockaddr_get_port1 = 0\n"
"  rc_l_sockaddr_get_port1 = C.l_sockaddr_get_port(self)\n"
"  return rc_l_sockaddr_get_port1\n"
"end\n"
"\n"
"-- method: addr\n"
"function _meth.LSockAddr.addr(self)\n"
"  \n"
"  local rc_l_sockaddr_get_addr1\n"
"  rc_l_sockaddr_get_addr1 = C.l_sockaddr_get_addr(self)\n"
"  return rc_l_sockaddr_get_addr1\n"
"end\n"
"\n"
"-- method: addrlen\n"
"function _meth.LSockAddr.addrlen(self)\n"
"  \n"
"  local rc_l_sockaddr_get_addrlen1 = 0\n"
"  rc_l_sockaddr_get_addrlen1 = C.l_sockaddr_get_addrlen(self)\n"
"  return rc_l_sockaddr_get_addrlen1\n"
"end\n"
"\n"
"-- method: __tostring\n"
"function _priv.LSockAddr.__tostring(self)\n"
"  \n"
"  local str_len1 = 1023\n"
"  local str1 = ffi.new(\"char[?]\", 1024)\n"
"	str_len1 = C.l_sockaddr_tostring(self, str1, str_len1)\n"
"\n"
"  return ffi_string_len(str1,str_len1)\n"
"end\n"
"\n", /* ----- CUT ----- */
"-- method: lookup_full\n"
"function _meth.LSockAddr.lookup_full(self, host2, port3, ai_family4, ai_socktype5, ai_protocol6, ai_flags7)\n"
"  \n"
"  local host_len2 = #host2\n"
"  port3 = tostring(port3)\n"
"  local port_len3 = port3 and #port3 or 0\n"
"  ai_family4 = ai_family4 or 0\n"
"  ai_socktype5 = ai_socktype5 or 0\n"
"  ai_protocol6 = ai_protocol6 or 0\n"
"  ai_flags7 = ai_flags7 or 0\n"
"  local rc_l_sockaddr_lookup_full1 = 0\n"
"  rc_l_sockaddr_lookup_full1 = C.l_sockaddr_lookup_full(self, host2, port3, ai_family4, ai_socktype5, ai_protocol6, ai_flags7)\n"
"  -- check for error.\n"
"  if (0 ~= rc_l_sockaddr_lookup_full1) then\n"
"    return nil, error_code__eai_rc__push(rc_l_sockaddr_lookup_full1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"_push.LSockAddr = obj_type_LSockAddr_push\n"
"ffi.metatype(\"LSockAddr\", _priv.LSockAddr)\n"
"-- End \"LSockAddr\" FFI interface\n"
"\n"
"\n"
"-- Start \"LAddrInfo\" FFI interface\n"
"-- method: new\n"
"function _pub.LAddrInfo.new(host1, port2)\n"
"  local host_len1 = #host1\n"
"  port2 = tostring(port2)\n"
"  local port_len2 = port2 and #port2 or 0\n"
"  local self = ffi.new(\"LAddrInfo\")\n"
"  local rc_l_addrinfo_init_ip2 = 0\n"
"  rc_l_addrinfo_init_ip2 = C.l_addrinfo_init_ip(self, host1, port2)\n"
"  if (0 ~= rc_l_addrinfo_init_ip2) then\n"
"    return nil,error_code__eai_rc__push(rc_l_addrinfo_init_ip2)\n"
"  end\n"
"  return obj_type_LAddrInfo_push(self)\n"
"end\n"
"register_default_constructor(_pub,\"LAddrInfo\",_pub.LAddrInfo.new)\n"
"\n"
"-- method: ipv4\n"
"function _pub.LAddrInfo.ipv4(host1, port2)\n"
"  local host_len1 = #host1\n"
"  port2 = tostring(port2)\n"
"  local port_len2 = port2 and #port2 or 0\n"
"  local self = ffi.new(\"LAddrInfo\")\n"
"  local rc_l_addrinfo_init_ipv42 = 0\n"
"  rc_l_addrinfo_init_ipv42 = C.l_addrinfo_init_ipv4(self, host1, port2)\n"
"  if (0 ~= rc_l_addrinfo_init_ipv42) then\n"
"    return nil,error_code__eai_rc__push(rc_l_addrinfo_init_ipv42)\n"
"  end\n"
"  return obj_type_LAddrInfo_push(self)\n"
"end\n"
"\n"
"-- method: ipv6\n"
"function _pub.LAddrInfo.ipv6(host1, port2)\n"
"  local host_len1 = #host1\n"
"  port2 = tostring(port2)\n"
"  local port_len2 = port2 and #port2 or 0\n"
"  local self = ffi.new(\"LAddrInfo\")\n"
"  local rc_l_addrinfo_init_ipv62 = 0\n"
"  rc_l_addrinfo_init_ipv62 = C.l_addrinfo_init_ipv6(self, host1, port2)\n"
"  if (0 ~= rc_l_addrinfo_init_ipv62) then\n"
"    return nil,error_code__eai_rc__push(rc_l_addrinfo_init_ipv62)\n"
"  end\n"
"  return obj_type_LAddrInfo_push(self)\n"
"end\n"
"\n"
"-- method: full\n"
"function _pub.LAddrInfo.full(host1, port2, ai_family3, ai_socktype4, ai_protocol5, ai_flags6)\n"
"  local host_len1 = #host1\n"
"  port2 = tostring(port2)\n"
"  local port_len2 = port2 and #port2 or 0\n"
"  ai_family3 = ai_family3 or 0\n"
"  ai_socktype4 = ai_socktype4 or 0\n"
"  ai_protocol5 = ai_protocol5 or 0\n"
"  ai_flags6 = ai_flags6 or 0\n"
"  local self = ffi.new(\"LAddrInfo\")\n"
"  local rc_l_addrinfo_init_full2 = 0\n"
"  rc_l_addrinfo_init_full2 = C.l_addrinfo_init_full(self, host1, port2, ai_family3, ai_socktype4, ai_protocol5, ai_flags6)\n"
"  if (0 ~= rc_l_addrinfo_init_full2) then\n"
"    return nil,error_code__eai_rc__push(rc_l_addrinfo_init_full2)\n"
"  end\n"
"  return obj_type_LAddrInfo_push(self)\n"
"end\n"
"\n"
"-- method: __gc\n"
"function _priv.LAddrInfo.__gc(self)\n"
"  local self = obj_type_LAddrInfo_delete(self)\n"
"  if not self then return end\n"
"  C.l_addrinfo_cleanup(self)\n"
"  return \n"
"end\n"
"\n"
"-- method: first\n"
"function _meth.LAddrInfo.first(self)\n"
"  \n"
"  local rc_l_addrinfo_first1 = 0\n"
"  rc_l_addrinfo_first1 = C.l_addrinfo_first(self)\n"
"  return rc_l_addrinfo_first1\n"
"end\n"
"\n"
"-- method: get_addr\n"
"function _meth.LAddrInfo.get_addr(self, addr2)\n"
"  \n"
"  \n"
"  local rc_l_addrinfo_get_addr1 = 0\n"
"  rc_l_addrinfo_get_addr1 = C.l_addrinfo_get_addr(self, addr2)\n"
"  return rc_l_addrinfo_get_addr1\n"
"end\n"
"\n"
"-- method: get_canonname\n"
"function _meth.LAddrInfo.get_canonname(self)\n"
"  \n"
"  local rc_l_addrinfo_get_canonname1\n"
"  rc_l_addrinfo_get_canonname1 = C.l_addrinfo_get_canonname(self)\n"
"  return ffi_string(rc_l_addrinfo_get_canonname1)\n"
"end\n"
"\n"
"-- method: get_family\n"
"function _meth.LAddrInfo.get_family(self)\n"
"  \n"
"  local rc_l_addrinfo_get_family1 = 0\n"
"  rc_l_addrinfo_get_family1 = C.l_addrinfo_get_family(self)\n"
"  return rc_l_addrinfo_get_family1\n"
"end\n"
"\n"
"-- method: get_socktype\n"
"function _meth.LAddrInfo.get_socktype(self)\n"
"  \n"
"  local rc_l_addrinfo_get_socktype1 = 0\n"
"  rc_l_addrinfo_get_socktype1 = C.l_addrinfo_get_socktype(self)\n"
"  return rc_l_addrinfo_get_socktype1\n"
"end\n"
"\n"
"-- method: get_protocol\n"
"function _meth.LAddrInfo.get_protocol(self)\n"
"  \n"
"  local rc_l_addrinfo_get_protocol1 = 0\n"
"  rc_l_addrinfo_get_protocol1 = C.l_addrinfo_get_protocol(self)\n"
"  return rc_l_addrinfo_get_protocol1\n"
"end\n"
"\n"
"-- method: next\n"
"function _meth.LAddrInfo.next(self)\n"
"  \n"
"  local rc_l_addrinfo_next1 = 0\n"
"  rc_l_addrinfo_next1 = C.l_addrinfo_next(self)\n"
"  return rc_l_addrinfo_next1\n"
"end\n"
"\n"
"_push.LAddrInfo = obj_type_LAddrInfo_push\n"
"ffi.metatype(\"LAddrInfo\", _priv.LAddrInfo)\n"
"-- End \"LAddrInfo\" FFI interface\n"
"\n"
"\n"
"-- Start \"Options\" FFI interface\n"
"-- End \"Options\" FFI interface\n"
"\n"
"\n"
"-- Start \"SetSocketOption\" FFI interface\n"
"-- method: IP_RECVOPTS\n"
"if (_pub.SetSocketOption.IP_RECVOPTS) then\n"
"function _pub.SetSocketOption.IP_RECVOPTS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_RECVOPTS1 = 0\n"
"  rc_lsocket_opt_set_IP_RECVOPTS1 = C.lsocket_opt_set_IP_RECVOPTS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_RECVOPTS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_RECVOPTS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_MTU\n"
"if (_pub.SetSocketOption.IP_MTU) then\n"
"function _pub.SetSocketOption.IP_MTU(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_MTU1 = 0\n"
"  rc_lsocket_opt_set_IP_MTU1 = C.lsocket_opt_set_IP_MTU(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_MTU1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_MTU1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_TOS\n"
"if (_pub.SetSocketOption.IP_TOS) then\n"
"function _pub.SetSocketOption.IP_TOS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_TOS1 = 0\n"
"  rc_lsocket_opt_set_IP_TOS1 = C.lsocket_opt_set_IP_TOS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_TOS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_TOS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_OPTIONS\n"
"if (_pub.SetSocketOption.IP_OPTIONS) then\n"
"function _pub.SetSocketOption.IP_OPTIONS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value_len2 = #value2\n"
"  local rc_lsocket_opt_set_IP_OPTIONS1 = 0\n"
"  rc_lsocket_opt_set_IP_OPTIONS1 = C.lsocket_opt_set_IP_OPTIONS(sock1, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_OPTIONS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_OPTIONS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_TTL\n"
"if (_pub.SetSocketOption.IP_TTL) then\n"
"function _pub.SetSocketOption.IP_TTL(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_TTL1 = 0\n"
"  rc_lsocket_opt_set_IP_TTL1 = C.lsocket_opt_set_IP_TTL(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_TTL1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_TTL1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_RECVTOS\n"
"if (_pub.SetSocketOption.IP_RECVTOS) then\n"
"function _pub.SetSocketOption.IP_RECVTOS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_RECVTOS1 = 0\n"
"  rc_lsocket_opt_set_IP_RECVTOS1 = C.lsocket_opt_set_IP_RECVTOS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_RECVTOS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_RECVTOS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_MINTTL\n"
"if (_pub.SetSocketOption.IP_MINTTL) then\n"
"function _pub.SetSocketOption.IP_MINTTL(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_MINTTL1 = 0\n"
"  rc_lsocket_opt_set_IP_MINTTL1 = C.lsocket_opt_set_IP_MINTTL(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_MINTTL1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_MINTTL1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_MULTICAST_TTL\n"
"if (_pub.SetSocketOption.IP_MULTICAST_TTL) then\n"
"function _pub.SetSocketOption.IP_MULTICAST_TTL(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_MULTICAST_TTL1 = 0\n"
"  rc_lsocket_opt_set_IP_MULTICAST_TTL1 = C.lsocket_opt_set_IP_MULTICAST_TTL(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_MULTICAST_TTL1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_MULTICAST_TTL1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_RECVTTL\n"
"if (_pub.SetSocketOption.IP_RECVTTL) then\n"
"function _pub.SetSocketOption.IP_RECVTTL(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_RECVTTL1 = 0\n"
"  rc_lsocket_opt_set_IP_RECVTTL1 = C.lsocket_opt_set_IP_RECVTTL(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_RECVTTL1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_RECVTTL1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_FREEBIND\n"
"if (_pub.SetSocketOption.IP_FREEBIND) then\n"
"function _pub.SetSocketOption.IP_FREEBIND(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_FREEBIND1 = 0\n"
"  rc_lsocket_opt_set_IP_FREEBIND1 = C.lsocket_opt_set_IP_FREEBIND(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_FREEBIND1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_FREEBIND1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_PKTINFO\n"
"if (_pub.SetSocketOption.IP_PKTINFO) then\n"
"function _pub.SetSocketOption.IP_PKTINFO(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_PKTINFO1 = 0\n"
"  rc_lsocket_opt_set_IP_PKTINFO1 = C.lsocket_opt_set_IP_PKTINFO(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_PKTINFO1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_PKTINFO1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_HDRINCL\n"
"if (_pub.SetSocketOption.IP_HDRINCL) then\n"
"function _pub.SetSocketOption.IP_HDRINCL(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_HDRINCL1 = 0\n"
"  rc_lsocket_opt_set_IP_HDRINCL1 = C.lsocket_opt_set_IP_HDRINCL(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_HDRINCL1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_HDRINCL1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_MTU_DISCOVER\n"
"if (_pub.SetSocketOption.IP_MTU_DISCOVER) then\n"
"function _pub.SetSocketOption.IP_MTU_DISCOVER(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_MTU_DISCOVER1 = 0\n"
"  rc_lsocket_opt_set_IP_MTU_DISCOVER1 = C.lsocket_opt_set_IP_MTU_DISCOVER(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_MTU_DISCOVER1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_MTU_DISCOVER1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_RETOPTS\n"
"if (_pub.SetSocketOption.IP_RETOPTS) then\n"
"function _pub.SetSocketOption.IP_RETOPTS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value_len2 = #value2\n"
"  local rc_lsocket_opt_set_IP_RETOPTS1 = 0\n"
"  rc_lsocket_opt_set_IP_RETOPTS1 = C.lsocket_opt_set_IP_RETOPTS(sock1, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_RETOPTS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_RETOPTS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_RECVERR\n"
"if (_pub.SetSocketOption.IP_RECVERR) then\n"
"function _pub.SetSocketOption.IP_RECVERR(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_RECVERR1 = 0\n"
"  rc_lsocket_opt_set_IP_RECVERR1 = C.lsocket_opt_set_IP_RECVERR(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_RECVERR1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_RECVERR1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_ROUTER_ALERT\n"
"if (_pub.SetSocketOption.IP_ROUTER_ALERT) then\n"
"function _pub.SetSocketOption.IP_ROUTER_ALERT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_ROUTER_ALERT1 = 0\n"
"  rc_lsocket_opt_set_IP_ROUTER_ALERT1 = C.lsocket_opt_set_IP_ROUTER_ALERT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_ROUTER_ALERT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_ROUTER_ALERT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_RECVRETOPTS\n"
"if (_pub.SetSocketOption.IP_RECVRETOPTS) then\n"
"function _pub.SetSocketOption.IP_RECVRETOPTS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_RECVRETOPTS1 = 0\n"
"  rc_lsocket_opt_set_IP_RECVRETOPTS1 = C.lsocket_opt_set_IP_RECVRETOPTS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_RECVRETOPTS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_RECVRETOPTS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_RECVORIGDSTADDR\n"
"if (_pub.SetSocketOption.IP_RECVORIGDSTADDR) then\n"
"function _pub.SetSocketOption.IP_RECVORIGDSTADDR(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_RECVORIGDSTADDR1 = 0\n"
"  rc_lsocket_opt_set_IP_RECVORIGDSTADDR1 = C.lsocket_opt_set_IP_RECVORIGDSTADDR(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_RECVORIGDSTADDR1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_RECVORIGDSTADDR1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IP_MULTICAST_LOOP\n"
"if (_pub.SetSocketOption.IP_MULTICAST_LOOP) then\n"
"function _pub.SetSocketOption.IP_MULTICAST_LOOP(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IP_MULTICAST_LOOP1 = 0\n"
"  rc_lsocket_opt_set_IP_MULTICAST_LOOP1 = C.lsocket_opt_set_IP_MULTICAST_LOOP(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IP_MULTICAST_LOOP1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IP_MULTICAST_LOOP1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_MULTICAST_HOPS\n"
"if (_pub.SetSocketOption.IPV6_MULTICAST_HOPS) then\n"
"function _pub.SetSocketOption.IPV6_MULTICAST_HOPS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_MULTICAST_HOPS1 = 0\n"
"  rc_lsocket_opt_set_IPV6_MULTICAST_HOPS1 = C.lsocket_opt_set_IPV6_MULTICAST_HOPS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_MULTICAST_HOPS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_MULTICAST_HOPS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_RECVERR\n"
"if (_pub.SetSocketOption.IPV6_RECVERR) then\n"
"function _pub.SetSocketOption.IPV6_RECVERR(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_RECVERR1 = 0\n"
"  rc_lsocket_opt_set_IPV6_RECVERR1 = C.lsocket_opt_set_IPV6_RECVERR(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_RECVERR1) then\n", /* ----- CUT ----- */
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_RECVERR1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_CHECKSUM\n"
"if (_pub.SetSocketOption.IPV6_CHECKSUM) then\n"
"function _pub.SetSocketOption.IPV6_CHECKSUM(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_CHECKSUM1 = 0\n"
"  rc_lsocket_opt_set_IPV6_CHECKSUM1 = C.lsocket_opt_set_IPV6_CHECKSUM(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_CHECKSUM1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_CHECKSUM1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_MULTICAST_LOOP\n"
"if (_pub.SetSocketOption.IPV6_MULTICAST_LOOP) then\n"
"function _pub.SetSocketOption.IPV6_MULTICAST_LOOP(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_MULTICAST_LOOP1 = 0\n"
"  rc_lsocket_opt_set_IPV6_MULTICAST_LOOP1 = C.lsocket_opt_set_IPV6_MULTICAST_LOOP(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_MULTICAST_LOOP1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_MULTICAST_LOOP1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_MULTICAST_IF\n"
"if (_pub.SetSocketOption.IPV6_MULTICAST_IF) then\n"
"function _pub.SetSocketOption.IPV6_MULTICAST_IF(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_MULTICAST_IF1 = 0\n"
"  rc_lsocket_opt_set_IPV6_MULTICAST_IF1 = C.lsocket_opt_set_IPV6_MULTICAST_IF(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_MULTICAST_IF1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_MULTICAST_IF1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_RECVRTHDR\n"
"if (_pub.SetSocketOption.IPV6_RECVRTHDR) then\n"
"function _pub.SetSocketOption.IPV6_RECVRTHDR(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_RECVRTHDR1 = 0\n"
"  rc_lsocket_opt_set_IPV6_RECVRTHDR1 = C.lsocket_opt_set_IPV6_RECVRTHDR(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_RECVRTHDR1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_RECVRTHDR1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_MTU_DISCOVER\n"
"if (_pub.SetSocketOption.IPV6_MTU_DISCOVER) then\n"
"function _pub.SetSocketOption.IPV6_MTU_DISCOVER(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_MTU_DISCOVER1 = 0\n"
"  rc_lsocket_opt_set_IPV6_MTU_DISCOVER1 = C.lsocket_opt_set_IPV6_MTU_DISCOVER(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_MTU_DISCOVER1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_MTU_DISCOVER1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_UNICAST_HOPS\n"
"if (_pub.SetSocketOption.IPV6_UNICAST_HOPS) then\n"
"function _pub.SetSocketOption.IPV6_UNICAST_HOPS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_UNICAST_HOPS1 = 0\n"
"  rc_lsocket_opt_set_IPV6_UNICAST_HOPS1 = C.lsocket_opt_set_IPV6_UNICAST_HOPS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_UNICAST_HOPS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_UNICAST_HOPS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_NEXTHOP\n"
"if (_pub.SetSocketOption.IPV6_NEXTHOP) then\n"
"function _pub.SetSocketOption.IPV6_NEXTHOP(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_NEXTHOP1 = 0\n"
"  rc_lsocket_opt_set_IPV6_NEXTHOP1 = C.lsocket_opt_set_IPV6_NEXTHOP(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_NEXTHOP1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_NEXTHOP1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_RECVHOPLIMIT\n"
"if (_pub.SetSocketOption.IPV6_RECVHOPLIMIT) then\n"
"function _pub.SetSocketOption.IPV6_RECVHOPLIMIT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_RECVHOPLIMIT1 = 0\n"
"  rc_lsocket_opt_set_IPV6_RECVHOPLIMIT1 = C.lsocket_opt_set_IPV6_RECVHOPLIMIT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_RECVHOPLIMIT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_RECVHOPLIMIT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_MTU\n"
"if (_pub.SetSocketOption.IPV6_MTU) then\n"
"function _pub.SetSocketOption.IPV6_MTU(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_MTU1 = 0\n"
"  rc_lsocket_opt_set_IPV6_MTU1 = C.lsocket_opt_set_IPV6_MTU(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_MTU1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_MTU1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_HOPLIMIT\n"
"if (_pub.SetSocketOption.IPV6_HOPLIMIT) then\n"
"function _pub.SetSocketOption.IPV6_HOPLIMIT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_HOPLIMIT1 = 0\n"
"  rc_lsocket_opt_set_IPV6_HOPLIMIT1 = C.lsocket_opt_set_IPV6_HOPLIMIT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_HOPLIMIT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_HOPLIMIT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_ADDRFORM\n"
"if (_pub.SetSocketOption.IPV6_ADDRFORM) then\n"
"function _pub.SetSocketOption.IPV6_ADDRFORM(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_ADDRFORM1 = 0\n"
"  rc_lsocket_opt_set_IPV6_ADDRFORM1 = C.lsocket_opt_set_IPV6_ADDRFORM(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_ADDRFORM1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_ADDRFORM1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_V6ONLY\n"
"if (_pub.SetSocketOption.IPV6_V6ONLY) then\n"
"function _pub.SetSocketOption.IPV6_V6ONLY(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_V6ONLY1 = 0\n"
"  rc_lsocket_opt_set_IPV6_V6ONLY1 = C.lsocket_opt_set_IPV6_V6ONLY(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_V6ONLY1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_V6ONLY1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_RECVDSTOPTS\n"
"if (_pub.SetSocketOption.IPV6_RECVDSTOPTS) then\n"
"function _pub.SetSocketOption.IPV6_RECVDSTOPTS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_RECVDSTOPTS1 = 0\n"
"  rc_lsocket_opt_set_IPV6_RECVDSTOPTS1 = C.lsocket_opt_set_IPV6_RECVDSTOPTS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_RECVDSTOPTS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_RECVDSTOPTS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_TCLASS\n"
"if (_pub.SetSocketOption.IPV6_TCLASS) then\n"
"function _pub.SetSocketOption.IPV6_TCLASS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_TCLASS1 = 0\n"
"  rc_lsocket_opt_set_IPV6_TCLASS1 = C.lsocket_opt_set_IPV6_TCLASS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_TCLASS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_TCLASS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_RECVTCLASS\n"
"if (_pub.SetSocketOption.IPV6_RECVTCLASS) then\n"
"function _pub.SetSocketOption.IPV6_RECVTCLASS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_RECVTCLASS1 = 0\n"
"  rc_lsocket_opt_set_IPV6_RECVTCLASS1 = C.lsocket_opt_set_IPV6_RECVTCLASS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_RECVTCLASS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_RECVTCLASS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_RECVHOPOPTS\n"
"if (_pub.SetSocketOption.IPV6_RECVHOPOPTS) then\n"
"function _pub.SetSocketOption.IPV6_RECVHOPOPTS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_RECVHOPOPTS1 = 0\n"
"  rc_lsocket_opt_set_IPV6_RECVHOPOPTS1 = C.lsocket_opt_set_IPV6_RECVHOPOPTS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_RECVHOPOPTS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_RECVHOPOPTS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_RECVPKTINFO\n"
"if (_pub.SetSocketOption.IPV6_RECVPKTINFO) then\n"
"function _pub.SetSocketOption.IPV6_RECVPKTINFO(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_RECVPKTINFO1 = 0\n"
"  rc_lsocket_opt_set_IPV6_RECVPKTINFO1 = C.lsocket_opt_set_IPV6_RECVPKTINFO(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_RECVPKTINFO1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_RECVPKTINFO1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_ROUTER_ALERT\n"
"if (_pub.SetSocketOption.IPV6_ROUTER_ALERT) then\n"
"function _pub.SetSocketOption.IPV6_ROUTER_ALERT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_IPV6_ROUTER_ALERT1 = 0\n"
"  rc_lsocket_opt_set_IPV6_ROUTER_ALERT1 = C.lsocket_opt_set_IPV6_ROUTER_ALERT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_IPV6_ROUTER_ALERT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_IPV6_ROUTER_ALERT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_SNDBUFFORCE\n"
"if (_pub.SetSocketOption.SO_SNDBUFFORCE) then\n"
"function _pub.SetSocketOption.SO_SNDBUFFORCE(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_SNDBUFFORCE1 = 0\n"
"  rc_lsocket_opt_set_SO_SNDBUFFORCE1 = C.lsocket_opt_set_SO_SNDBUFFORCE(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_SNDBUFFORCE1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_SNDBUFFORCE1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_TIMESTAMP\n"
"if (_pub.SetSocketOption.SO_TIMESTAMP) then\n"
"function _pub.SetSocketOption.SO_TIMESTAMP(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_TIMESTAMP1 = 0\n"
"  rc_lsocket_opt_set_SO_TIMESTAMP1 = C.lsocket_opt_set_SO_TIMESTAMP(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_TIMESTAMP1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_TIMESTAMP1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_SNDBUF\n"
"if (_pub.SetSocketOption.SO_SNDBUF) then\n"
"function _pub.SetSocketOption.SO_SNDBUF(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_SNDBUF1 = 0\n"
"  rc_lsocket_opt_set_SO_SNDBUF1 = C.lsocket_opt_set_SO_SNDBUF(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_SNDBUF1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_SNDBUF1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_NO_CHECK\n"
"if (_pub.SetSocketOption.SO_NO_CHECK) then\n"
"function _pub.SetSocketOption.SO_NO_CHECK(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_NO_CHECK1 = 0\n"
"  rc_lsocket_opt_set_SO_NO_CHECK1 = C.lsocket_opt_set_SO_NO_CHECK(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_NO_CHECK1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_NO_CHECK1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_BSDCOMPAT\n"
"if (_pub.SetSocketOption.SO_BSDCOMPAT) then\n"
"function _pub.SetSocketOption.SO_BSDCOMPAT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_BSDCOMPAT1 = 0\n"
"  rc_lsocket_opt_set_SO_BSDCOMPAT1 = C.lsocket_opt_set_SO_BSDCOMPAT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_BSDCOMPAT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_BSDCOMPAT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_REUSEADDR\n"
"if (_pub.SetSocketOption.SO_REUSEADDR) then\n"
"function _pub.SetSocketOption.SO_REUSEADDR(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_REUSEADDR1 = 0\n"
"  rc_lsocket_opt_set_SO_REUSEADDR1 = C.lsocket_opt_set_SO_REUSEADDR(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_REUSEADDR1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_REUSEADDR1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_MARK\n"
"if (_pub.SetSocketOption.SO_MARK) then\n"
"function _pub.SetSocketOption.SO_MARK(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_MARK1 = 0\n"
"  rc_lsocket_opt_set_SO_MARK1 = C.lsocket_opt_set_SO_MARK(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_MARK1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_MARK1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_TIMESTAMPING\n"
"if (_pub.SetSocketOption.SO_TIMESTAMPING) then\n"
"function _pub.SetSocketOption.SO_TIMESTAMPING(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_TIMESTAMPING1 = 0\n"
"  rc_lsocket_opt_set_SO_TIMESTAMPING1 = C.lsocket_opt_set_SO_TIMESTAMPING(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_TIMESTAMPING1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_TIMESTAMPING1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_TIMESTAMPNS\n"
"if (_pub.SetSocketOption.SO_TIMESTAMPNS) then\n"
"function _pub.SetSocketOption.SO_TIMESTAMPNS(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_TIMESTAMPNS1 = 0\n"
"  rc_lsocket_opt_set_SO_TIMESTAMPNS1 = C.lsocket_opt_set_SO_TIMESTAMPNS(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_TIMESTAMPNS1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_TIMESTAMPNS1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_BINDTODEVICE\n"
"if (_pub.SetSocketOption.SO_BINDTODEVICE) then\n"
"function _pub.SetSocketOption.SO_BINDTODEVICE(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value_len2 = #value2\n"
"  local rc_lsocket_opt_set_SO_BINDTODEVICE1 = 0\n"
"  rc_lsocket_opt_set_SO_BINDTODEVICE1 = C.lsocket_opt_set_SO_BINDTODEVICE(sock1, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_BINDTODEVICE1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_BINDTODEVICE1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_DONTROUTE\n"
"if (_pub.SetSocketOption.SO_DONTROUTE) then\n"
"function _pub.SetSocketOption.SO_DONTROUTE(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_DONTROUTE1 = 0\n"
"  rc_lsocket_opt_set_SO_DONTROUTE1 = C.lsocket_opt_set_SO_DONTROUTE(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_DONTROUTE1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_DONTROUTE1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_RCVBUFFORCE\n"
"if (_pub.SetSocketOption.SO_RCVBUFFORCE) then\n"
"function _pub.SetSocketOption.SO_RCVBUFFORCE(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_RCVBUFFORCE1 = 0\n"
"  rc_lsocket_opt_set_SO_RCVBUFFORCE1 = C.lsocket_opt_set_SO_RCVBUFFORCE(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_RCVBUFFORCE1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_RCVBUFFORCE1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_SNDLOWAT\n"
"if (_pub.SetSocketOption.SO_SNDLOWAT) then\n"
"function _pub.SetSocketOption.SO_SNDLOWAT(sock1, value2)\n", /* ----- CUT ----- */
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_SNDLOWAT1 = 0\n"
"  rc_lsocket_opt_set_SO_SNDLOWAT1 = C.lsocket_opt_set_SO_SNDLOWAT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_SNDLOWAT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_SNDLOWAT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_OOBINLINE\n"
"if (_pub.SetSocketOption.SO_OOBINLINE) then\n"
"function _pub.SetSocketOption.SO_OOBINLINE(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_OOBINLINE1 = 0\n"
"  rc_lsocket_opt_set_SO_OOBINLINE1 = C.lsocket_opt_set_SO_OOBINLINE(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_OOBINLINE1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_OOBINLINE1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_RCVLOWAT\n"
"if (_pub.SetSocketOption.SO_RCVLOWAT) then\n"
"function _pub.SetSocketOption.SO_RCVLOWAT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_RCVLOWAT1 = 0\n"
"  rc_lsocket_opt_set_SO_RCVLOWAT1 = C.lsocket_opt_set_SO_RCVLOWAT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_RCVLOWAT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_RCVLOWAT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_PASSCRED\n"
"if (_pub.SetSocketOption.SO_PASSCRED) then\n"
"function _pub.SetSocketOption.SO_PASSCRED(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_PASSCRED1 = 0\n"
"  rc_lsocket_opt_set_SO_PASSCRED1 = C.lsocket_opt_set_SO_PASSCRED(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_PASSCRED1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_PASSCRED1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_RCVBUF\n"
"if (_pub.SetSocketOption.SO_RCVBUF) then\n"
"function _pub.SetSocketOption.SO_RCVBUF(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_RCVBUF1 = 0\n"
"  rc_lsocket_opt_set_SO_RCVBUF1 = C.lsocket_opt_set_SO_RCVBUF(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_RCVBUF1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_RCVBUF1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_PRIORITY\n"
"if (_pub.SetSocketOption.SO_PRIORITY) then\n"
"function _pub.SetSocketOption.SO_PRIORITY(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_PRIORITY1 = 0\n"
"  rc_lsocket_opt_set_SO_PRIORITY1 = C.lsocket_opt_set_SO_PRIORITY(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_PRIORITY1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_PRIORITY1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_BROADCAST\n"
"if (_pub.SetSocketOption.SO_BROADCAST) then\n"
"function _pub.SetSocketOption.SO_BROADCAST(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_BROADCAST1 = 0\n"
"  rc_lsocket_opt_set_SO_BROADCAST1 = C.lsocket_opt_set_SO_BROADCAST(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_BROADCAST1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_BROADCAST1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_KEEPALIVE\n"
"if (_pub.SetSocketOption.SO_KEEPALIVE) then\n"
"function _pub.SetSocketOption.SO_KEEPALIVE(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_KEEPALIVE1 = 0\n"
"  rc_lsocket_opt_set_SO_KEEPALIVE1 = C.lsocket_opt_set_SO_KEEPALIVE(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_KEEPALIVE1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_KEEPALIVE1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: SO_DEBUG\n"
"if (_pub.SetSocketOption.SO_DEBUG) then\n"
"function _pub.SetSocketOption.SO_DEBUG(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_SO_DEBUG1 = 0\n"
"  rc_lsocket_opt_set_SO_DEBUG1 = C.lsocket_opt_set_SO_DEBUG(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_SO_DEBUG1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_SO_DEBUG1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_CORK\n"
"if (_pub.SetSocketOption.TCP_CORK) then\n"
"function _pub.SetSocketOption.TCP_CORK(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_CORK1 = 0\n"
"  rc_lsocket_opt_set_TCP_CORK1 = C.lsocket_opt_set_TCP_CORK(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_CORK1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_CORK1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_KEEPIDLE\n"
"if (_pub.SetSocketOption.TCP_KEEPIDLE) then\n"
"function _pub.SetSocketOption.TCP_KEEPIDLE(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_KEEPIDLE1 = 0\n"
"  rc_lsocket_opt_set_TCP_KEEPIDLE1 = C.lsocket_opt_set_TCP_KEEPIDLE(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_KEEPIDLE1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_KEEPIDLE1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_QUICKACK\n"
"if (_pub.SetSocketOption.TCP_QUICKACK) then\n"
"function _pub.SetSocketOption.TCP_QUICKACK(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_QUICKACK1 = 0\n"
"  rc_lsocket_opt_set_TCP_QUICKACK1 = C.lsocket_opt_set_TCP_QUICKACK(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_QUICKACK1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_QUICKACK1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_CONGESTION\n"
"if (_pub.SetSocketOption.TCP_CONGESTION) then\n"
"function _pub.SetSocketOption.TCP_CONGESTION(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value_len2 = #value2\n"
"  local rc_lsocket_opt_set_TCP_CONGESTION1 = 0\n"
"  rc_lsocket_opt_set_TCP_CONGESTION1 = C.lsocket_opt_set_TCP_CONGESTION(sock1, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_CONGESTION1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_CONGESTION1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_WINDOW_CLAMP\n"
"if (_pub.SetSocketOption.TCP_WINDOW_CLAMP) then\n"
"function _pub.SetSocketOption.TCP_WINDOW_CLAMP(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_WINDOW_CLAMP1 = 0\n"
"  rc_lsocket_opt_set_TCP_WINDOW_CLAMP1 = C.lsocket_opt_set_TCP_WINDOW_CLAMP(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_WINDOW_CLAMP1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_WINDOW_CLAMP1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_DEFER_ACCEPT\n"
"if (_pub.SetSocketOption.TCP_DEFER_ACCEPT) then\n"
"function _pub.SetSocketOption.TCP_DEFER_ACCEPT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_DEFER_ACCEPT1 = 0\n"
"  rc_lsocket_opt_set_TCP_DEFER_ACCEPT1 = C.lsocket_opt_set_TCP_DEFER_ACCEPT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_DEFER_ACCEPT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_DEFER_ACCEPT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_MAXSEG\n"
"if (_pub.SetSocketOption.TCP_MAXSEG) then\n"
"function _pub.SetSocketOption.TCP_MAXSEG(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_MAXSEG1 = 0\n"
"  rc_lsocket_opt_set_TCP_MAXSEG1 = C.lsocket_opt_set_TCP_MAXSEG(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_MAXSEG1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_MAXSEG1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_NODELAY\n"
"if (_pub.SetSocketOption.TCP_NODELAY) then\n"
"function _pub.SetSocketOption.TCP_NODELAY(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_NODELAY1 = 0\n"
"  rc_lsocket_opt_set_TCP_NODELAY1 = C.lsocket_opt_set_TCP_NODELAY(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_NODELAY1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_NODELAY1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_KEEPCNT\n"
"if (_pub.SetSocketOption.TCP_KEEPCNT) then\n"
"function _pub.SetSocketOption.TCP_KEEPCNT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_KEEPCNT1 = 0\n"
"  rc_lsocket_opt_set_TCP_KEEPCNT1 = C.lsocket_opt_set_TCP_KEEPCNT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_KEEPCNT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_KEEPCNT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_LINGER2\n"
"if (_pub.SetSocketOption.TCP_LINGER2) then\n"
"function _pub.SetSocketOption.TCP_LINGER2(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_LINGER21 = 0\n"
"  rc_lsocket_opt_set_TCP_LINGER21 = C.lsocket_opt_set_TCP_LINGER2(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_LINGER21) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_LINGER21)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_SYNCNT\n"
"if (_pub.SetSocketOption.TCP_SYNCNT) then\n"
"function _pub.SetSocketOption.TCP_SYNCNT(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_SYNCNT1 = 0\n"
"  rc_lsocket_opt_set_TCP_SYNCNT1 = C.lsocket_opt_set_TCP_SYNCNT(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_SYNCNT1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_SYNCNT1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_KEEPINTVL\n"
"if (_pub.SetSocketOption.TCP_KEEPINTVL) then\n"
"function _pub.SetSocketOption.TCP_KEEPINTVL(sock1, value2)\n"
"  sock1 = sock1._wrapped_val\n"
"  \n"
"  local rc_lsocket_opt_set_TCP_KEEPINTVL1 = 0\n"
"  rc_lsocket_opt_set_TCP_KEEPINTVL1 = C.lsocket_opt_set_TCP_KEEPINTVL(sock1, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lsocket_opt_set_TCP_KEEPINTVL1) then\n"
"    return nil, error_code__errno_rc__push(rc_lsocket_opt_set_TCP_KEEPINTVL1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- End \"SetSocketOption\" FFI interface\n"
"\n"
"\n"
"-- Start \"GetSocketOption\" FFI interface\n"
"do\n"
"  local IP_RECVOPTS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_RECVOPTS\n"
"if (_pub.GetSocketOption.IP_RECVOPTS) then\n"
"function _pub.GetSocketOption.IP_RECVOPTS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_RECVOPTS_value_tmp\n"
"  local rc_lsocket_opt_get_IP_RECVOPTS2 = 0\n"
"  rc_lsocket_opt_get_IP_RECVOPTS2 = C.lsocket_opt_get_IP_RECVOPTS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_RECVOPTS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_RECVOPTS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_MTU_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_MTU\n"
"if (_pub.GetSocketOption.IP_MTU) then\n"
"function _pub.GetSocketOption.IP_MTU(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_MTU_value_tmp\n"
"  local rc_lsocket_opt_get_IP_MTU2 = 0\n"
"  rc_lsocket_opt_get_IP_MTU2 = C.lsocket_opt_get_IP_MTU(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_MTU2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_MTU2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_TOS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_TOS\n"
"if (_pub.GetSocketOption.IP_TOS) then\n"
"function _pub.GetSocketOption.IP_TOS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_TOS_value_tmp\n"
"  local rc_lsocket_opt_get_IP_TOS2 = 0\n"
"  rc_lsocket_opt_get_IP_TOS2 = C.lsocket_opt_get_IP_TOS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_TOS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_TOS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: IP_OPTIONS\n"
"if (_pub.GetSocketOption.IP_OPTIONS) then\n"
"function _pub.GetSocketOption.IP_OPTIONS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lsocket_opt_get_IP_OPTIONS2 = 0\n"
"  rc_lsocket_opt_get_IP_OPTIONS2 = C.lsocket_opt_get_IP_OPTIONS(sock1, value1, value_len1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_OPTIONS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_OPTIONS2)\n"
"  end\n"
"  return ffi_string_len(value1,value_len1)\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_TTL_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_TTL\n"
"if (_pub.GetSocketOption.IP_TTL) then\n"
"function _pub.GetSocketOption.IP_TTL(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_TTL_value_tmp\n"
"  local rc_lsocket_opt_get_IP_TTL2 = 0\n"
"  rc_lsocket_opt_get_IP_TTL2 = C.lsocket_opt_get_IP_TTL(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_TTL2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_TTL2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_RECVTOS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_RECVTOS\n"
"if (_pub.GetSocketOption.IP_RECVTOS) then\n"
"function _pub.GetSocketOption.IP_RECVTOS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_RECVTOS_value_tmp\n"
"  local rc_lsocket_opt_get_IP_RECVTOS2 = 0\n"
"  rc_lsocket_opt_get_IP_RECVTOS2 = C.lsocket_opt_get_IP_RECVTOS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_RECVTOS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_RECVTOS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_MINTTL_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_MINTTL\n"
"if (_pub.GetSocketOption.IP_MINTTL) then\n"
"function _pub.GetSocketOption.IP_MINTTL(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_MINTTL_value_tmp\n"
"  local rc_lsocket_opt_get_IP_MINTTL2 = 0\n"
"  rc_lsocket_opt_get_IP_MINTTL2 = C.lsocket_opt_get_IP_MINTTL(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_MINTTL2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_MINTTL2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_MULTICAST_TTL_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_MULTICAST_TTL\n"
"if (_pub.GetSocketOption.IP_MULTICAST_TTL) then\n"
"function _pub.GetSocketOption.IP_MULTICAST_TTL(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_MULTICAST_TTL_value_tmp\n"
"  local rc_lsocket_opt_get_IP_MULTICAST_TTL2 = 0\n"
"  rc_lsocket_opt_get_IP_MULTICAST_TTL2 = C.lsocket_opt_get_IP_MULTICAST_TTL(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_MULTICAST_TTL2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_MULTICAST_TTL2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_RECVTTL_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_RECVTTL\n"
"if (_pub.GetSocketOption.IP_RECVTTL) then\n"
"function _pub.GetSocketOption.IP_RECVTTL(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_RECVTTL_value_tmp\n"
"  local rc_lsocket_opt_get_IP_RECVTTL2 = 0\n"
"  rc_lsocket_opt_get_IP_RECVTTL2 = C.lsocket_opt_get_IP_RECVTTL(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_RECVTTL2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_RECVTTL2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_FREEBIND_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_FREEBIND\n"
"if (_pub.GetSocketOption.IP_FREEBIND) then\n"
"function _pub.GetSocketOption.IP_FREEBIND(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_FREEBIND_value_tmp\n", /* ----- CUT ----- */
"  local rc_lsocket_opt_get_IP_FREEBIND2 = 0\n"
"  rc_lsocket_opt_get_IP_FREEBIND2 = C.lsocket_opt_get_IP_FREEBIND(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_FREEBIND2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_FREEBIND2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_PKTINFO_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_PKTINFO\n"
"if (_pub.GetSocketOption.IP_PKTINFO) then\n"
"function _pub.GetSocketOption.IP_PKTINFO(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_PKTINFO_value_tmp\n"
"  local rc_lsocket_opt_get_IP_PKTINFO2 = 0\n"
"  rc_lsocket_opt_get_IP_PKTINFO2 = C.lsocket_opt_get_IP_PKTINFO(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_PKTINFO2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_PKTINFO2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_HDRINCL_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_HDRINCL\n"
"if (_pub.GetSocketOption.IP_HDRINCL) then\n"
"function _pub.GetSocketOption.IP_HDRINCL(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_HDRINCL_value_tmp\n"
"  local rc_lsocket_opt_get_IP_HDRINCL2 = 0\n"
"  rc_lsocket_opt_get_IP_HDRINCL2 = C.lsocket_opt_get_IP_HDRINCL(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_HDRINCL2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_HDRINCL2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_MTU_DISCOVER_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_MTU_DISCOVER\n"
"if (_pub.GetSocketOption.IP_MTU_DISCOVER) then\n"
"function _pub.GetSocketOption.IP_MTU_DISCOVER(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_MTU_DISCOVER_value_tmp\n"
"  local rc_lsocket_opt_get_IP_MTU_DISCOVER2 = 0\n"
"  rc_lsocket_opt_get_IP_MTU_DISCOVER2 = C.lsocket_opt_get_IP_MTU_DISCOVER(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_MTU_DISCOVER2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_MTU_DISCOVER2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: IP_RETOPTS\n"
"if (_pub.GetSocketOption.IP_RETOPTS) then\n"
"function _pub.GetSocketOption.IP_RETOPTS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lsocket_opt_get_IP_RETOPTS2 = 0\n"
"  rc_lsocket_opt_get_IP_RETOPTS2 = C.lsocket_opt_get_IP_RETOPTS(sock1, value1, value_len1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_RETOPTS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_RETOPTS2)\n"
"  end\n"
"  return ffi_string_len(value1,value_len1)\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_RECVERR_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_RECVERR\n"
"if (_pub.GetSocketOption.IP_RECVERR) then\n"
"function _pub.GetSocketOption.IP_RECVERR(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_RECVERR_value_tmp\n"
"  local rc_lsocket_opt_get_IP_RECVERR2 = 0\n"
"  rc_lsocket_opt_get_IP_RECVERR2 = C.lsocket_opt_get_IP_RECVERR(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_RECVERR2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_RECVERR2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_ROUTER_ALERT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_ROUTER_ALERT\n"
"if (_pub.GetSocketOption.IP_ROUTER_ALERT) then\n"
"function _pub.GetSocketOption.IP_ROUTER_ALERT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_ROUTER_ALERT_value_tmp\n"
"  local rc_lsocket_opt_get_IP_ROUTER_ALERT2 = 0\n"
"  rc_lsocket_opt_get_IP_ROUTER_ALERT2 = C.lsocket_opt_get_IP_ROUTER_ALERT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_ROUTER_ALERT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_ROUTER_ALERT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_RECVRETOPTS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_RECVRETOPTS\n"
"if (_pub.GetSocketOption.IP_RECVRETOPTS) then\n"
"function _pub.GetSocketOption.IP_RECVRETOPTS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_RECVRETOPTS_value_tmp\n"
"  local rc_lsocket_opt_get_IP_RECVRETOPTS2 = 0\n"
"  rc_lsocket_opt_get_IP_RECVRETOPTS2 = C.lsocket_opt_get_IP_RECVRETOPTS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_RECVRETOPTS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_RECVRETOPTS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_RECVORIGDSTADDR_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_RECVORIGDSTADDR\n"
"if (_pub.GetSocketOption.IP_RECVORIGDSTADDR) then\n"
"function _pub.GetSocketOption.IP_RECVORIGDSTADDR(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_RECVORIGDSTADDR_value_tmp\n"
"  local rc_lsocket_opt_get_IP_RECVORIGDSTADDR2 = 0\n"
"  rc_lsocket_opt_get_IP_RECVORIGDSTADDR2 = C.lsocket_opt_get_IP_RECVORIGDSTADDR(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_RECVORIGDSTADDR2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_RECVORIGDSTADDR2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IP_MULTICAST_LOOP_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IP_MULTICAST_LOOP\n"
"if (_pub.GetSocketOption.IP_MULTICAST_LOOP) then\n"
"function _pub.GetSocketOption.IP_MULTICAST_LOOP(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IP_MULTICAST_LOOP_value_tmp\n"
"  local rc_lsocket_opt_get_IP_MULTICAST_LOOP2 = 0\n"
"  rc_lsocket_opt_get_IP_MULTICAST_LOOP2 = C.lsocket_opt_get_IP_MULTICAST_LOOP(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IP_MULTICAST_LOOP2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IP_MULTICAST_LOOP2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_MULTICAST_HOPS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_MULTICAST_HOPS\n"
"if (_pub.GetSocketOption.IPV6_MULTICAST_HOPS) then\n"
"function _pub.GetSocketOption.IPV6_MULTICAST_HOPS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_MULTICAST_HOPS_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_MULTICAST_HOPS2 = 0\n"
"  rc_lsocket_opt_get_IPV6_MULTICAST_HOPS2 = C.lsocket_opt_get_IPV6_MULTICAST_HOPS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_MULTICAST_HOPS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_MULTICAST_HOPS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_RECVERR_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_RECVERR\n"
"if (_pub.GetSocketOption.IPV6_RECVERR) then\n"
"function _pub.GetSocketOption.IPV6_RECVERR(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_RECVERR_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_RECVERR2 = 0\n"
"  rc_lsocket_opt_get_IPV6_RECVERR2 = C.lsocket_opt_get_IPV6_RECVERR(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_RECVERR2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_RECVERR2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_CHECKSUM_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_CHECKSUM\n"
"if (_pub.GetSocketOption.IPV6_CHECKSUM) then\n"
"function _pub.GetSocketOption.IPV6_CHECKSUM(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_CHECKSUM_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_CHECKSUM2 = 0\n"
"  rc_lsocket_opt_get_IPV6_CHECKSUM2 = C.lsocket_opt_get_IPV6_CHECKSUM(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_CHECKSUM2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_CHECKSUM2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_MULTICAST_LOOP_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_MULTICAST_LOOP\n"
"if (_pub.GetSocketOption.IPV6_MULTICAST_LOOP) then\n"
"function _pub.GetSocketOption.IPV6_MULTICAST_LOOP(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_MULTICAST_LOOP_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_MULTICAST_LOOP2 = 0\n"
"  rc_lsocket_opt_get_IPV6_MULTICAST_LOOP2 = C.lsocket_opt_get_IPV6_MULTICAST_LOOP(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_MULTICAST_LOOP2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_MULTICAST_LOOP2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_MULTICAST_IF_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_MULTICAST_IF\n"
"if (_pub.GetSocketOption.IPV6_MULTICAST_IF) then\n"
"function _pub.GetSocketOption.IPV6_MULTICAST_IF(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_MULTICAST_IF_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_MULTICAST_IF2 = 0\n"
"  rc_lsocket_opt_get_IPV6_MULTICAST_IF2 = C.lsocket_opt_get_IPV6_MULTICAST_IF(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_MULTICAST_IF2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_MULTICAST_IF2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_RECVRTHDR_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_RECVRTHDR\n"
"if (_pub.GetSocketOption.IPV6_RECVRTHDR) then\n"
"function _pub.GetSocketOption.IPV6_RECVRTHDR(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_RECVRTHDR_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_RECVRTHDR2 = 0\n"
"  rc_lsocket_opt_get_IPV6_RECVRTHDR2 = C.lsocket_opt_get_IPV6_RECVRTHDR(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_RECVRTHDR2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_RECVRTHDR2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_MTU_DISCOVER_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_MTU_DISCOVER\n"
"if (_pub.GetSocketOption.IPV6_MTU_DISCOVER) then\n"
"function _pub.GetSocketOption.IPV6_MTU_DISCOVER(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_MTU_DISCOVER_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_MTU_DISCOVER2 = 0\n"
"  rc_lsocket_opt_get_IPV6_MTU_DISCOVER2 = C.lsocket_opt_get_IPV6_MTU_DISCOVER(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_MTU_DISCOVER2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_MTU_DISCOVER2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_UNICAST_HOPS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_UNICAST_HOPS\n"
"if (_pub.GetSocketOption.IPV6_UNICAST_HOPS) then\n"
"function _pub.GetSocketOption.IPV6_UNICAST_HOPS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_UNICAST_HOPS_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_UNICAST_HOPS2 = 0\n"
"  rc_lsocket_opt_get_IPV6_UNICAST_HOPS2 = C.lsocket_opt_get_IPV6_UNICAST_HOPS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_UNICAST_HOPS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_UNICAST_HOPS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: IPV6_NEXTHOP\n"
"if (_pub.GetSocketOption.IPV6_NEXTHOP) then\n"
"function _pub.GetSocketOption.IPV6_NEXTHOP(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = ffi.new(\"LSockAddr\")\n"
"  local rc_lsocket_opt_get_IPV6_NEXTHOP2 = 0\n"
"  rc_lsocket_opt_get_IPV6_NEXTHOP2 = C.lsocket_opt_get_IPV6_NEXTHOP(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_NEXTHOP2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_NEXTHOP2)\n"
"  end\n"
"  return obj_type_LSockAddr_push(value1)\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_RECVHOPLIMIT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_RECVHOPLIMIT\n"
"if (_pub.GetSocketOption.IPV6_RECVHOPLIMIT) then\n"
"function _pub.GetSocketOption.IPV6_RECVHOPLIMIT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_RECVHOPLIMIT_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_RECVHOPLIMIT2 = 0\n"
"  rc_lsocket_opt_get_IPV6_RECVHOPLIMIT2 = C.lsocket_opt_get_IPV6_RECVHOPLIMIT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_RECVHOPLIMIT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_RECVHOPLIMIT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_MTU_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_MTU\n"
"if (_pub.GetSocketOption.IPV6_MTU) then\n"
"function _pub.GetSocketOption.IPV6_MTU(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_MTU_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_MTU2 = 0\n"
"  rc_lsocket_opt_get_IPV6_MTU2 = C.lsocket_opt_get_IPV6_MTU(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_MTU2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_MTU2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_HOPLIMIT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_HOPLIMIT\n"
"if (_pub.GetSocketOption.IPV6_HOPLIMIT) then\n"
"function _pub.GetSocketOption.IPV6_HOPLIMIT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_HOPLIMIT_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_HOPLIMIT2 = 0\n"
"  rc_lsocket_opt_get_IPV6_HOPLIMIT2 = C.lsocket_opt_get_IPV6_HOPLIMIT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_HOPLIMIT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_HOPLIMIT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_V6ONLY_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_V6ONLY\n"
"if (_pub.GetSocketOption.IPV6_V6ONLY) then\n"
"function _pub.GetSocketOption.IPV6_V6ONLY(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_V6ONLY_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_V6ONLY2 = 0\n"
"  rc_lsocket_opt_get_IPV6_V6ONLY2 = C.lsocket_opt_get_IPV6_V6ONLY(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_V6ONLY2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_V6ONLY2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_RECVDSTOPTS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_RECVDSTOPTS\n"
"if (_pub.GetSocketOption.IPV6_RECVDSTOPTS) then\n"
"function _pub.GetSocketOption.IPV6_RECVDSTOPTS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_RECVDSTOPTS_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_RECVDSTOPTS2 = 0\n"
"  rc_lsocket_opt_get_IPV6_RECVDSTOPTS2 = C.lsocket_opt_get_IPV6_RECVDSTOPTS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_RECVDSTOPTS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_RECVDSTOPTS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_TCLASS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_TCLASS\n"
"if (_pub.GetSocketOption.IPV6_TCLASS) then\n"
"function _pub.GetSocketOption.IPV6_TCLASS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_TCLASS_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_TCLASS2 = 0\n"
"  rc_lsocket_opt_get_IPV6_TCLASS2 = C.lsocket_opt_get_IPV6_TCLASS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_TCLASS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_TCLASS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_RECVTCLASS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_RECVTCLASS\n"
"if (_pub.GetSocketOption.IPV6_RECVTCLASS) then\n"
"function _pub.GetSocketOption.IPV6_RECVTCLASS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_RECVTCLASS_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_RECVTCLASS2 = 0\n"
"  rc_lsocket_opt_get_IPV6_RECVTCLASS2 = C.lsocket_opt_get_IPV6_RECVTCLASS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_RECVTCLASS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_RECVTCLASS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_RECVHOPOPTS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_RECVHOPOPTS\n"
"if (_pub.GetSocketOption.IPV6_RECVHOPOPTS) then\n"
"function _pub.GetSocketOption.IPV6_RECVHOPOPTS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_RECVHOPOPTS_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_RECVHOPOPTS2 = 0\n"
"  rc_lsocket_opt_get_IPV6_RECVHOPOPTS2 = C.lsocket_opt_get_IPV6_RECVHOPOPTS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_RECVHOPOPTS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_RECVHOPOPTS2)\n", /* ----- CUT ----- */
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_RECVPKTINFO_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_RECVPKTINFO\n"
"if (_pub.GetSocketOption.IPV6_RECVPKTINFO) then\n"
"function _pub.GetSocketOption.IPV6_RECVPKTINFO(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_RECVPKTINFO_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_RECVPKTINFO2 = 0\n"
"  rc_lsocket_opt_get_IPV6_RECVPKTINFO2 = C.lsocket_opt_get_IPV6_RECVPKTINFO(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_RECVPKTINFO2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_RECVPKTINFO2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local IPV6_ROUTER_ALERT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: IPV6_ROUTER_ALERT\n"
"if (_pub.GetSocketOption.IPV6_ROUTER_ALERT) then\n"
"function _pub.GetSocketOption.IPV6_ROUTER_ALERT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = IPV6_ROUTER_ALERT_value_tmp\n"
"  local rc_lsocket_opt_get_IPV6_ROUTER_ALERT2 = 0\n"
"  rc_lsocket_opt_get_IPV6_ROUTER_ALERT2 = C.lsocket_opt_get_IPV6_ROUTER_ALERT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_IPV6_ROUTER_ALERT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_IPV6_ROUTER_ALERT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_SNDBUFFORCE_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_SNDBUFFORCE\n"
"if (_pub.GetSocketOption.SO_SNDBUFFORCE) then\n"
"function _pub.GetSocketOption.SO_SNDBUFFORCE(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_SNDBUFFORCE_value_tmp\n"
"  local rc_lsocket_opt_get_SO_SNDBUFFORCE2 = 0\n"
"  rc_lsocket_opt_get_SO_SNDBUFFORCE2 = C.lsocket_opt_get_SO_SNDBUFFORCE(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_SNDBUFFORCE2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_SNDBUFFORCE2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_ACCEPTCONN_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_ACCEPTCONN\n"
"if (_pub.GetSocketOption.SO_ACCEPTCONN) then\n"
"function _pub.GetSocketOption.SO_ACCEPTCONN(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_ACCEPTCONN_value_tmp\n"
"  local rc_lsocket_opt_get_SO_ACCEPTCONN2 = 0\n"
"  rc_lsocket_opt_get_SO_ACCEPTCONN2 = C.lsocket_opt_get_SO_ACCEPTCONN(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_ACCEPTCONN2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_ACCEPTCONN2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_PROTOCOL_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_PROTOCOL\n"
"if (_pub.GetSocketOption.SO_PROTOCOL) then\n"
"function _pub.GetSocketOption.SO_PROTOCOL(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_PROTOCOL_value_tmp\n"
"  local rc_lsocket_opt_get_SO_PROTOCOL2 = 0\n"
"  rc_lsocket_opt_get_SO_PROTOCOL2 = C.lsocket_opt_get_SO_PROTOCOL(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_PROTOCOL2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_PROTOCOL2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_TIMESTAMP_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_TIMESTAMP\n"
"if (_pub.GetSocketOption.SO_TIMESTAMP) then\n"
"function _pub.GetSocketOption.SO_TIMESTAMP(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_TIMESTAMP_value_tmp\n"
"  local rc_lsocket_opt_get_SO_TIMESTAMP2 = 0\n"
"  rc_lsocket_opt_get_SO_TIMESTAMP2 = C.lsocket_opt_get_SO_TIMESTAMP(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_TIMESTAMP2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_TIMESTAMP2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_SNDBUF_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_SNDBUF\n"
"if (_pub.GetSocketOption.SO_SNDBUF) then\n"
"function _pub.GetSocketOption.SO_SNDBUF(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_SNDBUF_value_tmp\n"
"  local rc_lsocket_opt_get_SO_SNDBUF2 = 0\n"
"  rc_lsocket_opt_get_SO_SNDBUF2 = C.lsocket_opt_get_SO_SNDBUF(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_SNDBUF2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_SNDBUF2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_NO_CHECK_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_NO_CHECK\n"
"if (_pub.GetSocketOption.SO_NO_CHECK) then\n"
"function _pub.GetSocketOption.SO_NO_CHECK(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_NO_CHECK_value_tmp\n"
"  local rc_lsocket_opt_get_SO_NO_CHECK2 = 0\n"
"  rc_lsocket_opt_get_SO_NO_CHECK2 = C.lsocket_opt_get_SO_NO_CHECK(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_NO_CHECK2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_NO_CHECK2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_BSDCOMPAT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_BSDCOMPAT\n"
"if (_pub.GetSocketOption.SO_BSDCOMPAT) then\n"
"function _pub.GetSocketOption.SO_BSDCOMPAT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_BSDCOMPAT_value_tmp\n"
"  local rc_lsocket_opt_get_SO_BSDCOMPAT2 = 0\n"
"  rc_lsocket_opt_get_SO_BSDCOMPAT2 = C.lsocket_opt_get_SO_BSDCOMPAT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_BSDCOMPAT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_BSDCOMPAT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_REUSEADDR_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_REUSEADDR\n"
"if (_pub.GetSocketOption.SO_REUSEADDR) then\n"
"function _pub.GetSocketOption.SO_REUSEADDR(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_REUSEADDR_value_tmp\n"
"  local rc_lsocket_opt_get_SO_REUSEADDR2 = 0\n"
"  rc_lsocket_opt_get_SO_REUSEADDR2 = C.lsocket_opt_get_SO_REUSEADDR(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_REUSEADDR2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_REUSEADDR2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_MARK_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_MARK\n"
"if (_pub.GetSocketOption.SO_MARK) then\n"
"function _pub.GetSocketOption.SO_MARK(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_MARK_value_tmp\n"
"  local rc_lsocket_opt_get_SO_MARK2 = 0\n"
"  rc_lsocket_opt_get_SO_MARK2 = C.lsocket_opt_get_SO_MARK(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_MARK2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_MARK2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_TIMESTAMPING_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_TIMESTAMPING\n"
"if (_pub.GetSocketOption.SO_TIMESTAMPING) then\n"
"function _pub.GetSocketOption.SO_TIMESTAMPING(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_TIMESTAMPING_value_tmp\n"
"  local rc_lsocket_opt_get_SO_TIMESTAMPING2 = 0\n"
"  rc_lsocket_opt_get_SO_TIMESTAMPING2 = C.lsocket_opt_get_SO_TIMESTAMPING(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_TIMESTAMPING2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_TIMESTAMPING2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_TIMESTAMPNS_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_TIMESTAMPNS\n"
"if (_pub.GetSocketOption.SO_TIMESTAMPNS) then\n"
"function _pub.GetSocketOption.SO_TIMESTAMPNS(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_TIMESTAMPNS_value_tmp\n"
"  local rc_lsocket_opt_get_SO_TIMESTAMPNS2 = 0\n"
"  rc_lsocket_opt_get_SO_TIMESTAMPNS2 = C.lsocket_opt_get_SO_TIMESTAMPNS(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_TIMESTAMPNS2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_TIMESTAMPNS2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: SO_BINDTODEVICE\n"
"if (_pub.GetSocketOption.SO_BINDTODEVICE) then\n"
"function _pub.GetSocketOption.SO_BINDTODEVICE(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lsocket_opt_get_SO_BINDTODEVICE2 = 0\n"
"  rc_lsocket_opt_get_SO_BINDTODEVICE2 = C.lsocket_opt_get_SO_BINDTODEVICE(sock1, value1, value_len1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_BINDTODEVICE2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_BINDTODEVICE2)\n"
"  end\n"
"  return ffi_string_len(value1,value_len1)\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_DONTROUTE_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_DONTROUTE\n"
"if (_pub.GetSocketOption.SO_DONTROUTE) then\n"
"function _pub.GetSocketOption.SO_DONTROUTE(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_DONTROUTE_value_tmp\n"
"  local rc_lsocket_opt_get_SO_DONTROUTE2 = 0\n"
"  rc_lsocket_opt_get_SO_DONTROUTE2 = C.lsocket_opt_get_SO_DONTROUTE(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_DONTROUTE2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_DONTROUTE2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_RCVBUFFORCE_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_RCVBUFFORCE\n"
"if (_pub.GetSocketOption.SO_RCVBUFFORCE) then\n"
"function _pub.GetSocketOption.SO_RCVBUFFORCE(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_RCVBUFFORCE_value_tmp\n"
"  local rc_lsocket_opt_get_SO_RCVBUFFORCE2 = 0\n"
"  rc_lsocket_opt_get_SO_RCVBUFFORCE2 = C.lsocket_opt_get_SO_RCVBUFFORCE(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_RCVBUFFORCE2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_RCVBUFFORCE2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_SNDLOWAT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_SNDLOWAT\n"
"if (_pub.GetSocketOption.SO_SNDLOWAT) then\n"
"function _pub.GetSocketOption.SO_SNDLOWAT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_SNDLOWAT_value_tmp\n"
"  local rc_lsocket_opt_get_SO_SNDLOWAT2 = 0\n"
"  rc_lsocket_opt_get_SO_SNDLOWAT2 = C.lsocket_opt_get_SO_SNDLOWAT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_SNDLOWAT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_SNDLOWAT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_OOBINLINE_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_OOBINLINE\n"
"if (_pub.GetSocketOption.SO_OOBINLINE) then\n"
"function _pub.GetSocketOption.SO_OOBINLINE(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_OOBINLINE_value_tmp\n"
"  local rc_lsocket_opt_get_SO_OOBINLINE2 = 0\n"
"  rc_lsocket_opt_get_SO_OOBINLINE2 = C.lsocket_opt_get_SO_OOBINLINE(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_OOBINLINE2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_OOBINLINE2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_DOMAIN_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_DOMAIN\n"
"if (_pub.GetSocketOption.SO_DOMAIN) then\n"
"function _pub.GetSocketOption.SO_DOMAIN(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_DOMAIN_value_tmp\n"
"  local rc_lsocket_opt_get_SO_DOMAIN2 = 0\n"
"  rc_lsocket_opt_get_SO_DOMAIN2 = C.lsocket_opt_get_SO_DOMAIN(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_DOMAIN2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_DOMAIN2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_RCVLOWAT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_RCVLOWAT\n"
"if (_pub.GetSocketOption.SO_RCVLOWAT) then\n"
"function _pub.GetSocketOption.SO_RCVLOWAT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_RCVLOWAT_value_tmp\n"
"  local rc_lsocket_opt_get_SO_RCVLOWAT2 = 0\n"
"  rc_lsocket_opt_get_SO_RCVLOWAT2 = C.lsocket_opt_get_SO_RCVLOWAT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_RCVLOWAT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_RCVLOWAT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_PASSCRED_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_PASSCRED\n"
"if (_pub.GetSocketOption.SO_PASSCRED) then\n"
"function _pub.GetSocketOption.SO_PASSCRED(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_PASSCRED_value_tmp\n"
"  local rc_lsocket_opt_get_SO_PASSCRED2 = 0\n"
"  rc_lsocket_opt_get_SO_PASSCRED2 = C.lsocket_opt_get_SO_PASSCRED(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_PASSCRED2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_PASSCRED2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_RCVBUF_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_RCVBUF\n"
"if (_pub.GetSocketOption.SO_RCVBUF) then\n"
"function _pub.GetSocketOption.SO_RCVBUF(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_RCVBUF_value_tmp\n"
"  local rc_lsocket_opt_get_SO_RCVBUF2 = 0\n"
"  rc_lsocket_opt_get_SO_RCVBUF2 = C.lsocket_opt_get_SO_RCVBUF(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_RCVBUF2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_RCVBUF2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_PRIORITY_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_PRIORITY\n"
"if (_pub.GetSocketOption.SO_PRIORITY) then\n"
"function _pub.GetSocketOption.SO_PRIORITY(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_PRIORITY_value_tmp\n"
"  local rc_lsocket_opt_get_SO_PRIORITY2 = 0\n"
"  rc_lsocket_opt_get_SO_PRIORITY2 = C.lsocket_opt_get_SO_PRIORITY(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_PRIORITY2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_PRIORITY2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_TYPE_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_TYPE\n"
"if (_pub.GetSocketOption.SO_TYPE) then\n"
"function _pub.GetSocketOption.SO_TYPE(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_TYPE_value_tmp\n"
"  local rc_lsocket_opt_get_SO_TYPE2 = 0\n"
"  rc_lsocket_opt_get_SO_TYPE2 = C.lsocket_opt_get_SO_TYPE(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_TYPE2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_TYPE2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_BROADCAST_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_BROADCAST\n"
"if (_pub.GetSocketOption.SO_BROADCAST) then\n"
"function _pub.GetSocketOption.SO_BROADCAST(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_BROADCAST_value_tmp\n"
"  local rc_lsocket_opt_get_SO_BROADCAST2 = 0\n"
"  rc_lsocket_opt_get_SO_BROADCAST2 = C.lsocket_opt_get_SO_BROADCAST(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_BROADCAST2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_BROADCAST2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_KEEPALIVE_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_KEEPALIVE\n"
"if (_pub.GetSocketOption.SO_KEEPALIVE) then\n"
"function _pub.GetSocketOption.SO_KEEPALIVE(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_KEEPALIVE_value_tmp\n"
"  local rc_lsocket_opt_get_SO_KEEPALIVE2 = 0\n"
"  rc_lsocket_opt_get_SO_KEEPALIVE2 = C.lsocket_opt_get_SO_KEEPALIVE(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_KEEPALIVE2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_KEEPALIVE2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_DEBUG_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_DEBUG\n"
"if (_pub.GetSocketOption.SO_DEBUG) then\n"
"function _pub.GetSocketOption.SO_DEBUG(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_DEBUG_value_tmp\n"
"  local rc_lsocket_opt_get_SO_DEBUG2 = 0\n"
"  rc_lsocket_opt_get_SO_DEBUG2 = C.lsocket_opt_get_SO_DEBUG(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_DEBUG2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_DEBUG2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local SO_ERROR_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: SO_ERROR\n"
"if (_pub.GetSocketOption.SO_ERROR) then\n", /* ----- CUT ----- */
"function _pub.GetSocketOption.SO_ERROR(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = SO_ERROR_value_tmp\n"
"  local rc_lsocket_opt_get_SO_ERROR2 = 0\n"
"  rc_lsocket_opt_get_SO_ERROR2 = C.lsocket_opt_get_SO_ERROR(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_SO_ERROR2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_SO_ERROR2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_CORK_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_CORK\n"
"if (_pub.GetSocketOption.TCP_CORK) then\n"
"function _pub.GetSocketOption.TCP_CORK(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_CORK_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_CORK2 = 0\n"
"  rc_lsocket_opt_get_TCP_CORK2 = C.lsocket_opt_get_TCP_CORK(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_CORK2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_CORK2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_KEEPIDLE_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_KEEPIDLE\n"
"if (_pub.GetSocketOption.TCP_KEEPIDLE) then\n"
"function _pub.GetSocketOption.TCP_KEEPIDLE(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_KEEPIDLE_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_KEEPIDLE2 = 0\n"
"  rc_lsocket_opt_get_TCP_KEEPIDLE2 = C.lsocket_opt_get_TCP_KEEPIDLE(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_KEEPIDLE2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_KEEPIDLE2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_QUICKACK_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_QUICKACK\n"
"if (_pub.GetSocketOption.TCP_QUICKACK) then\n"
"function _pub.GetSocketOption.TCP_QUICKACK(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_QUICKACK_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_QUICKACK2 = 0\n"
"  rc_lsocket_opt_get_TCP_QUICKACK2 = C.lsocket_opt_get_TCP_QUICKACK(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_QUICKACK2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_QUICKACK2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: TCP_CONGESTION\n"
"if (_pub.GetSocketOption.TCP_CONGESTION) then\n"
"function _pub.GetSocketOption.TCP_CONGESTION(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lsocket_opt_get_TCP_CONGESTION2 = 0\n"
"  rc_lsocket_opt_get_TCP_CONGESTION2 = C.lsocket_opt_get_TCP_CONGESTION(sock1, value1, value_len1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_CONGESTION2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_CONGESTION2)\n"
"  end\n"
"  return ffi_string_len(value1,value_len1)\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_WINDOW_CLAMP_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_WINDOW_CLAMP\n"
"if (_pub.GetSocketOption.TCP_WINDOW_CLAMP) then\n"
"function _pub.GetSocketOption.TCP_WINDOW_CLAMP(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_WINDOW_CLAMP_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_WINDOW_CLAMP2 = 0\n"
"  rc_lsocket_opt_get_TCP_WINDOW_CLAMP2 = C.lsocket_opt_get_TCP_WINDOW_CLAMP(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_WINDOW_CLAMP2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_WINDOW_CLAMP2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_DEFER_ACCEPT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_DEFER_ACCEPT\n"
"if (_pub.GetSocketOption.TCP_DEFER_ACCEPT) then\n"
"function _pub.GetSocketOption.TCP_DEFER_ACCEPT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_DEFER_ACCEPT_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_DEFER_ACCEPT2 = 0\n"
"  rc_lsocket_opt_get_TCP_DEFER_ACCEPT2 = C.lsocket_opt_get_TCP_DEFER_ACCEPT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_DEFER_ACCEPT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_DEFER_ACCEPT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_MAXSEG_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_MAXSEG\n"
"if (_pub.GetSocketOption.TCP_MAXSEG) then\n"
"function _pub.GetSocketOption.TCP_MAXSEG(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_MAXSEG_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_MAXSEG2 = 0\n"
"  rc_lsocket_opt_get_TCP_MAXSEG2 = C.lsocket_opt_get_TCP_MAXSEG(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_MAXSEG2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_MAXSEG2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_NODELAY_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_NODELAY\n"
"if (_pub.GetSocketOption.TCP_NODELAY) then\n"
"function _pub.GetSocketOption.TCP_NODELAY(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_NODELAY_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_NODELAY2 = 0\n"
"  rc_lsocket_opt_get_TCP_NODELAY2 = C.lsocket_opt_get_TCP_NODELAY(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_NODELAY2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_NODELAY2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_KEEPCNT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_KEEPCNT\n"
"if (_pub.GetSocketOption.TCP_KEEPCNT) then\n"
"function _pub.GetSocketOption.TCP_KEEPCNT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_KEEPCNT_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_KEEPCNT2 = 0\n"
"  rc_lsocket_opt_get_TCP_KEEPCNT2 = C.lsocket_opt_get_TCP_KEEPCNT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_KEEPCNT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_KEEPCNT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_LINGER2_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_LINGER2\n"
"if (_pub.GetSocketOption.TCP_LINGER2) then\n"
"function _pub.GetSocketOption.TCP_LINGER2(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_LINGER2_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_LINGER22 = 0\n"
"  rc_lsocket_opt_get_TCP_LINGER22 = C.lsocket_opt_get_TCP_LINGER2(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_LINGER22) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_LINGER22)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_SYNCNT_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_SYNCNT\n"
"if (_pub.GetSocketOption.TCP_SYNCNT) then\n"
"function _pub.GetSocketOption.TCP_SYNCNT(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_SYNCNT_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_SYNCNT2 = 0\n"
"  rc_lsocket_opt_get_TCP_SYNCNT2 = C.lsocket_opt_get_TCP_SYNCNT(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_SYNCNT2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_SYNCNT2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local TCP_KEEPINTVL_value_tmp = ffi.new(\"int[1]\")\n"
"-- method: TCP_KEEPINTVL\n"
"if (_pub.GetSocketOption.TCP_KEEPINTVL) then\n"
"function _pub.GetSocketOption.TCP_KEEPINTVL(sock1)\n"
"  sock1 = sock1._wrapped_val\n"
"  local value1 = TCP_KEEPINTVL_value_tmp\n"
"  local rc_lsocket_opt_get_TCP_KEEPINTVL2 = 0\n"
"  rc_lsocket_opt_get_TCP_KEEPINTVL2 = C.lsocket_opt_get_TCP_KEEPINTVL(sock1, value1)\n"
"  if (-1 == rc_lsocket_opt_get_TCP_KEEPINTVL2) then\n"
"    return nil,error_code__errno_rc__push(rc_lsocket_opt_get_TCP_KEEPINTVL2)\n"
"  end\n"
"  return value1[0]\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- End \"GetSocketOption\" FFI interface\n"
"\n"
"\n"
"-- Start \"LSocketFD\" FFI interface\n"
"-- method: new\n"
"function _pub.LSocketFD.new(domain1, type2, protocol3, flags4)\n"
"  \n"
"  \n"
"  protocol3 = protocol3 or 0\n"
"  flags4 = flags4 or 0\n"
"  local self\n"
"  self = C.l_socket_open(domain1, type2, protocol3, flags4)\n"
"  return obj_type_LSocketFD_push(self)\n"
"end\n"
"register_default_constructor(_pub,\"LSocketFD\",_pub.LSocketFD.new)\n"
"\n"
"-- method: fd\n"
"function _pub.LSocketFD.fd(fd1)\n"
"  \n"
"  local self\n"
"  self = fd1\n"
"  return obj_type_LSocketFD_push(self)\n"
"end\n"
"\n"
"-- method: close\n"
"function _meth.LSocketFD.close(self)\n"
"  local self = obj_type_LSocketFD_delete(self)\n"
"  if not self then return end\n"
"  C.l_socket_close_internal(self)\n"
"  return \n"
"end\n"
"_priv.LSocketFD.__gc = _meth.LSocketFD.close\n"
"\n"
"-- method: __tostring\n"
"function _priv.LSocketFD.__tostring(self)\n"
"  self = self._wrapped_val\n"
"  local str1\n"
"	str1 = string.format(\"LSocketFD: fd=%i\", self)\n"
"\n"
"  return ffi_string(str1)\n"
"end\n"
"\n"
"-- method: shutdown\n"
"function _meth.LSocketFD.shutdown(self, how2)\n"
"  self = self._wrapped_val\n"
"  \n"
"  local rc_l_socket_shutdown1 = 0\n"
"  rc_l_socket_shutdown1 = C.l_socket_shutdown(self, how2)\n"
"  return rc_l_socket_shutdown1\n"
"end\n"
"\n"
"-- method: fileno\n"
"function _meth.LSocketFD.fileno(self)\n"
"  self = self._wrapped_val\n"
"  local fd1 = 0\n"
"  fd1 = self\n"
"  return fd1\n"
"end\n"
"\n"
"-- method: set_nonblock\n"
"function _meth.LSocketFD.set_nonblock(self, nonblock2)\n"
"  self = self._wrapped_val\n"
"  \n"
"  local rc_l_socket_set_nonblock1 = 0\n"
"  rc_l_socket_set_nonblock1 = C.l_socket_set_nonblock(self, nonblock2)\n"
"  -- check for error.\n"
"  if (-1 == rc_l_socket_set_nonblock1) then\n"
"    return nil, error_code__errno_rc__push(rc_l_socket_set_nonblock1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: connect\n"
"function _meth.LSocketFD.connect(self, addr2)\n"
"  self = self._wrapped_val\n"
"  \n"
"  local rc_l_socket_connect1 = 0\n"
"  rc_l_socket_connect1 = C.l_socket_connect(self, addr2)\n"
"  -- check for error.\n"
"  if (-1 == rc_l_socket_connect1) then\n"
"    return nil, error_code__errno_rc__push(rc_l_socket_connect1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: bind\n"
"function _meth.LSocketFD.bind(self, addr2)\n"
"  self = self._wrapped_val\n"
"  \n"
"  local rc_l_socket_bind1 = 0\n"
"  rc_l_socket_bind1 = C.l_socket_bind(self, addr2)\n"
"  -- check for error.\n"
"  if (-1 == rc_l_socket_bind1) then\n"
"    return nil, error_code__errno_rc__push(rc_l_socket_bind1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: listen\n"
"function _meth.LSocketFD.listen(self, backlog2)\n"
"  self = self._wrapped_val\n"
"  \n"
"  local rc_l_socket_listen1 = 0\n"
"  rc_l_socket_listen1 = C.l_socket_listen(self, backlog2)\n"
"  -- check for error.\n"
"  if (-1 == rc_l_socket_listen1) then\n"
"    return nil, error_code__errno_rc__push(rc_l_socket_listen1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: get_sockname\n"
"function _meth.LSocketFD.get_sockname(self, addr2)\n"
"  self = self._wrapped_val\n"
"  \n"
"  local rc_l_socket_get_sockname1 = 0\n"
"  rc_l_socket_get_sockname1 = C.l_socket_get_sockname(self, addr2)\n"
"  -- check for error.\n"
"  if (-1 == rc_l_socket_get_sockname1) then\n"
"    return nil, error_code__errno_rc__push(rc_l_socket_get_sockname1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: get_peername\n"
"function _meth.LSocketFD.get_peername(self, addr2)\n"
"  self = self._wrapped_val\n"
"  \n"
"  local rc_l_socket_get_peername1 = 0\n"
"  rc_l_socket_get_peername1 = C.l_socket_get_peername(self, addr2)\n"
"  -- check for error.\n"
"  if (-1 == rc_l_socket_get_peername1) then\n"
"    return nil, error_code__errno_rc__push(rc_l_socket_get_peername1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: accept\n"
"function _meth.LSocketFD.accept(self, peer2, flags3)\n"
"  self = self._wrapped_val\n"
"  peer2 = peer2 and obj_type_LSockAddr_check(peer2) or nil\n"
"  flags3 = flags3 or 0\n"
"  local client1\n"
"  local rc2 = 0\n"
"  rc2 = C.l_socket_accept(self, peer2, flags3)\n"
"	client1 = rc2;\n"
"\n"
"  if (-1 == rc2) then\n"
"    return nil,error_code__errno_rc__push(rc2)\n"
"  end\n"
"  return obj_type_LSocketFD_push(client1)\n"
"end\n"
"\n"
"-- method: send\n"
"function _meth.LSocketFD.send(self, data2, flags3)\n"
"  self = self._wrapped_val\n"
"  local data_len2 = #data2\n"
"  flags3 = flags3 or 0\n"
"  local rc1 = 0\n"
"	rc1 = C.l_socket_send(self, data2, data_len2, flags3)\n"
"	-- rc1 >= 0, then return number of bytes sent.\n"
"	if rc1 >= 0 then return rc1 end\n"
"\n"
"  -- check for error.\n"
"  if (-1 == rc1) then\n"
"    return nil, error_code__errno_rc__push(rc1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"local tmp_buf_len = 8192\n"
"local tmp_buf = ffi.new(\"char[?]\", tmp_buf_len)\n"
"\n"
"-- method: recv\n"
"function _meth.LSocketFD.recv(self, len2, flags3)\n"
"  self = self._wrapped_val\n"
"  \n"
"  flags3 = flags3 or 0\n"
"  local data_len1 = 0\n"
"  local data1\n"
"  local rc2 = 0\n"
"	local buf_len = (tmp_buf_len < len2) and tmp_buf_len or len2\n"
"	rc2 = C.l_socket_recv(self, tmp_buf, buf_len, flags3)\n"
"	-- rc2 == 0, then socket is closed.\n"
"	if rc2 == 0 then return nil, \"CLOSED\" end\n"
"	data1 = tmp_buf;\n"
"	data_len1 = rc2;\n"
"\n"
"  if (-1 == rc2) then\n"
"    return nil,error_code__errno_rc__push(rc2)\n"
"  end\n"
"  return ffi_string_len(data1,data_len1)\n"
"end\n"
"\n"
"-- method: send_buffer\n"
"function _meth.LSocketFD.send_buffer(self, buf2, off3, len4, flags5)\n"
"  self = self._wrapped_val\n"
"  buf2_if = buf2.NOBJ_get_BufferIF or obj_type_Buffer_check(buf2)\n"
"  off3 = off3 or 0\n"
"  len4 = len4 or 0\n"
"  flags5 = flags5 or 0\n"
"  local rc1 = 0\n"
"	local data_len = buf2_if.get_size(buf2)\n"
"	local data = buf2_if.const_data(buf2)\n"
"	-- apply offset.\n"
"	if(off3 > 0) then\n"
"		if(off3 >= data_len) then\n"
"			error(\"Offset out-of-bounds.\");\n"
"		end\n"
"		data = data + off3;\n"
"		data_len = data_len - off3;\n"
"	end\n"
"	-- apply length.\n"
"	if(len4 > 0) then\n"
"		if(len4 > data_len) then\n"
"			error(\"Length out-of-bounds.\");\n"
"		end\n"
"		data_len = len4;\n"
"	end\n"
"	rc1 = C.l_socket_send(self, data, data_len, flags5)\n"
"	-- rc1 >= 0, then return number of bytes sent.\n"
"	if rc1 >= 0 then return rc1 end\n"
"\n"
"  -- check for error.\n"
"  if (-1 == rc1) then\n"
"    return nil, error_code__errno_rc__push(rc1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: recv_buffer\n"
"function _meth.LSocketFD.recv_buffer(self, buf2, off3, len4, flags5)\n"
"  self = self._wrapped_val\n"
"  buf2_if = buf2.NOBJ_get_MutableBufferIF or obj_type_MutableBuffer_check(buf2)\n"
"  off3 = off3 or 0\n"
"  len4 = len4 or 4096\n"
"  flags5 = flags5 or 0\n"
"  local read_len1 = 0\n"
"  local rc2 = 0\n"
"	local cap_len = buf2_if.get_size(buf2)\n"
"	local data = buf2_if.data(buf2)\n"
"	-- apply offset.\n"
"	if(off3 > 0) then\n"
"		if(off3 >= cap_len) then\n"
"			error(\"Offset out-of-bounds.\");\n"
"		end\n"
"		data = data + off3;\n"
"		cap_len = cap_len - off3;\n"
"	end\n"
"	-- calculate read length.\n"
"	if(len4 < cap_len) then\n"
"		cap_len = len4;\n"
"	end\n"
"	if(0 == cap_len) then\n"
"		return nil, \"ENOBUFS\"\n"
"	end\n"
"	rc2 = C.l_socket_recv(self, data, cap_len, flags5)\n"
"	-- rc2 == 0, then socket is closed.\n"
"	if rc2 == 0 then return nil, \"CLOSED\" end\n"
"	read_len1 = rc2\n"
"\n"
"  if (-1 == rc2) then\n"
"    return nil,error_code__errno_rc__push(rc2)\n"
"  end\n"
"  return read_len1\n"
"end\n"
"\n"
"-- method: send_buf\n"
"function _meth.LSocketFD.send_buf(self, data2, off3, len4, flags5)\n"
"  self = self._wrapped_val\n"
"  \n"
"  off3 = off3 or 0\n"
"  len4 = len4 or 0\n"
"  flags5 = flags5 or 0\n"
"  local rc1 = 0\n"
"	data2 = ffi.cast(\"uint8_t *\",data2) + off3\n"
"	rc1 = C.l_socket_send(self, data2, len4, flags5)\n"
"	-- rc1 >= 0, then return number of bytes sent.\n"
"	if rc1 >= 0 then return rc1 end\n"
"\n"
"  -- check for error.\n"
"  if (-1 == rc1) then\n"
"    return nil, error_code__errno_rc__push(rc1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: recv_buf\n"
"function _meth.LSocketFD.recv_buf(self, data2, off3, len4, flags5)\n"
"  self = self._wrapped_val\n"
"  \n"
"  off3 = off3 or 0\n"
"  len4 = len4 or 4096\n"
"  flags5 = flags5 or 0\n"
"  local data_len1 = 0\n"
"  local rc2 = 0\n"
"	data2 = ffi.cast(\"uint8_t *\",data2) + off3\n"
"	rc2 = C.l_socket_recv(self, data2, len4, flags5)\n"
"	-- rc2 == 0, then socket is closed.\n"
"	if rc2 == 0 then return nil, \"CLOSED\" end\n"
"	data_len1 = rc2\n"
"\n"
"  if (-1 == rc2) then\n", /* ----- CUT ----- */
"    return nil,error_code__errno_rc__push(rc2)\n"
"  end\n"
"  return data_len1\n"
"end\n"
"\n"
"_push.LSocketFD = obj_type_LSocketFD_push\n"
"ffi.metatype(\"LSocketFD_t\", _priv.LSocketFD)\n"
"-- End \"LSocketFD\" FFI interface\n"
"\n"
"\n"
"-- Start \"LIOBuffer\" FFI interface\n"
"-- method: new\n"
"function _pub.LIOBuffer.new(size_or_data1)\n"
"  local self = ffi.new(\"LIOBuffer\")\n"
"	local buf\n"
"	local data\n"
"	local len\n"
"	local ltype = type(size_or_data1)\n"
"\n"
"	if ltype == 'string' then\n"
"		data = size_or_data1\n"
"		len = #data\n"
"		C.l_iobuffer_init(self, data, len)\n"
"	elseif ltype == 'number' then\n"
"		len = size_or_data1\n"
"		C.l_iobuffer_init_cap(self, len)\n"
"	end\n"
"\n"
"\n"
"  return obj_type_LIOBuffer_push(self)\n"
"end\n"
"register_default_constructor(_pub,\"LIOBuffer\",_pub.LIOBuffer.new)\n"
"\n"
"-- method: free\n"
"function _meth.LIOBuffer.free(self)\n"
"  local self = obj_type_LIOBuffer_delete(self)\n"
"  if not self then return end\n"
"  C.l_iobuffer_cleanup(self)\n"
"  return \n"
"end\n"
"_priv.LIOBuffer.__gc = _meth.LIOBuffer.free\n"
"\n"
"-- method: __tostring\n"
"function _priv.LIOBuffer.__tostring(self)\n"
"  \n"
"  local data_len1 = 0\n"
"  local data1\n"
" data1 = self.buf; data_len1 = self._size; \n"
"  return ffi_string_len(data1,data_len1)\n"
"end\n"
"\n"
"-- method: tostring\n"
"function _meth.LIOBuffer.tostring(self)\n"
"  \n"
"  local data_len1 = 0\n"
"  local data1\n"
" data1 = self.buf; data_len1 = self._size; \n"
"  return ffi_string_len(data1,data_len1)\n"
"end\n"
"\n"
"-- method: get_byte\n"
"function _meth.LIOBuffer.get_byte(self, offset2)\n"
"  \n"
"  \n"
"  local val1 = 0\n"
"	-- check offset.\n"
"	if(offset2 >= self._size or offset2 < 0) then\n"
"		return error(\"Offset out-of-bounds.\")\n"
"	end\n"
"	val1 = tonumber(self.buf[offset2])\n"
"\n"
"  return val1\n"
"end\n"
"\n"
"-- method: set_byte\n"
"function _meth.LIOBuffer.set_byte(self, offset2, val3)\n"
"  \n"
"  \n"
"  \n"
"	-- check offset.\n"
"	if(offset2 >= self._size or offset2 < 0) then\n"
"		return error(\"Offset out-of-bounds.\")\n"
"	end\n"
"	self.buf[offset2] = val3\n"
"\n"
"  return \n"
"end\n"
"\n"
"local function next_byte(data, i, j)\n"
"	if i < j then\n"
"		return data[i], next_byte(data, i+1, j)\n"
"	end\n"
"	return data[i]\n"
"end\n"
"\n"
"-- method: get_bytes\n"
"function _meth.LIOBuffer.get_bytes(self, offset2, length3)\n"
"  \n"
"  offset2 = offset2 or 0\n"
"  length3 = length3 or 0\n"
"	local data_len = self._size\n"
"	local data = self.buf\n"
"	-- apply offset.\n"
"	if(offset2 ~= 0) then\n"
"		if(offset2 >= data_len or offset2 < 0) then\n"
"			return error(\"Offset out-of-bounds.\")\n"
"		end\n"
"		data = data + offset2\n"
"		data_len = data_len - offset2\n"
"	end\n"
"	-- apply length.\n"
"	if(length3 ~= 0) then\n"
"		if(length3 > data_len or length3 < 0) then\n"
"			return error(\"Length out-of-bounds.\")\n"
"		end\n"
"		data_len = length3\n"
"	end\n"
"	if data_len > 0 then\n"
"		return next_byte(data, 0, data_len - 1)\n"
"	end\n"
"\n"
"  return \n"
"end\n"
"\n"
"-- method: get_data\n"
"function _meth.LIOBuffer.get_data(self, offset2, length3)\n"
"  \n"
"  offset2 = offset2 or 0\n"
"  length3 = length3 or 0\n"
"  local data_len1 = 0\n"
"  local data1\n"
"	data_len1 = self._size\n"
"	data1 = self.buf\n"
"	-- apply offset.\n"
"	if(offset2 ~= 0) then\n"
"		if(offset2 >= data_len1 or offset2 < 0) then\n"
"			return error(\"Offset out-of-bounds.\")\n"
"		end\n"
"		data1 = data1 + offset2\n"
"		data_len1 = data_len1 - offset2\n"
"	end\n"
"	-- apply length.\n"
"	if(length3 ~= 0) then\n"
"		if(length3 > data_len1 or length3 < 0) then\n"
"			return error(\"Length out-of-bounds.\")\n"
"		end\n"
"		data_len1 = length3\n"
"	end\n"
"\n"
"  return ffi_string_len(data1,data_len1)\n"
"end\n"
"\n"
"-- method: set_data\n"
"function _meth.LIOBuffer.set_data(self, data2)\n"
"  \n"
"  local data_len2 = #data2\n"
"	-- check capacity\n"
"	if(data_len2 > self._capacity) then\n"
"		if(C.l_iobuffer_set_capacity(self, data_len2) == 0) then\n"
"			return error(\"Can't grow buffer, not enough space.\");\n"
"		end\n"
"	end\n"
"	ffi.copy(self.buf, data2, data_len2);\n"
"	self._size = data_len2;\n"
"\n"
"  return \n"
"end\n"
"\n"
"-- method: copy_buffer\n"
"function _meth.LIOBuffer.copy_buffer(self, src2, offset3, length4)\n"
"  \n"
"  src2_if = src2.NOBJ_get_BufferIF or obj_type_Buffer_check(src2)\n"
"  offset3 = offset3 or 0\n"
"  length4 = length4 or 0\n"
"	local data_len = src2_if.get_size(src2)\n"
"	local data = src2_if.const_data(src2)\n"
"	-- apply offset.\n"
"	if(offset3 ~= 0) then\n"
"		if(offset3 >= data_len or offset3 < 0) then\n"
"			error(\"Offset out-of-bounds.\");\n"
"		end\n"
"		data = data + offset3;\n"
"		data_len = data_len - offset3;\n"
"	end\n"
"	-- apply length.\n"
"	if(length4 ~= 0) then\n"
"		if(length4 > data_len or length4 < 0) then\n"
"			error(\"Length out-of-bounds.\");\n"
"		end\n"
"		data_len = length4;\n"
"	end\n"
"	-- check capacity\n"
"	if(data_len > self._capacity) then\n"
"		if(C.l_iobuffer_set_capacity(self, data_len) == 0) then\n"
"			return error(\"Can't grow buffer, not enough space.\");\n"
"		end\n"
"	end\n"
"	ffi.copy(self.buf, data, data_len)\n"
"	self._size = data_len\n"
"\n"
"  return \n"
"end\n"
"\n"
"-- method: reset\n"
"function _meth.LIOBuffer.reset(self)\n"
"  \n"
"  C.l_iobuffer_reset(self)\n"
"  return \n"
"end\n"
"\n"
"-- method: __len\n"
"function _priv.LIOBuffer.__len(self)\n"
"  \n"
"  local size1 = 0\n"
"size1 = self._size; \n"
"  return tonumber(size1)\n"
"end\n"
"\n"
"-- method: size\n"
"function _meth.LIOBuffer.size(self)\n"
"  \n"
"  local size1 = 0\n"
"size1 = self._size; \n"
"  return tonumber(size1)\n"
"end\n"
"\n"
"-- method: set_size\n"
"function _meth.LIOBuffer.set_size(self, size2)\n"
"  \n"
"  \n"
"  local rc_l_iobuffer_set_size1 = 0\n"
"  rc_l_iobuffer_set_size1 = C.l_iobuffer_set_size(self, size2)\n"
"  return rc_l_iobuffer_set_size1\n"
"end\n"
"\n"
"-- method: capacity\n"
"function _meth.LIOBuffer.capacity(self)\n"
"  \n"
"  local capacity1 = 0\n"
"capacity1 = self._capacity; \n"
"  return tonumber(capacity1)\n"
"end\n"
"\n"
"-- method: set_capacity\n"
"function _meth.LIOBuffer.set_capacity(self, capacity2)\n"
"  \n"
"  \n"
"  local rc_l_iobuffer_set_capacity1 = 0\n"
"  rc_l_iobuffer_set_capacity1 = C.l_iobuffer_set_capacity(self, capacity2)\n"
"  return rc_l_iobuffer_set_capacity1\n"
"end\n"
"\n"
"-- LIOBuffer implements Buffer interface\n"
"do\n"
"  local impl_meths = obj_register_interface(\"BufferIF\", \"LIOBuffer\")\n"
"-- Buffer interface method const_data\n"
"function impl_meths.const_data(this_p)\n"
"  return this_p.buf\n"
"end\n"
"-- Buffer interface method get_size\n"
"function impl_meths.get_size(this_p)\n"
"  return this_p._size\n"
"end\n"
"end\n"
"\n"
"-- LIOBuffer implements MutableBuffer interface\n"
"do\n"
"  local impl_meths = obj_register_interface(\"MutableBufferIF\", \"LIOBuffer\")\n"
"-- MutableBuffer interface method data\n"
"function impl_meths.data(this_p)\n"
"  return this_p.buf\n"
"end\n"
"-- MutableBuffer interface method get_size\n"
"function impl_meths.get_size(this_p)\n"
"  return this_p._capacity\n"
"end\n"
"end\n"
"\n"
"_push.LIOBuffer = obj_type_LIOBuffer_push\n"
"ffi.metatype(\"LIOBuffer\", _priv.LIOBuffer)\n"
"-- End \"LIOBuffer\" FFI interface\n"
"\n", NULL };
static char llnet_Errors_key[] = "llnet_Errors_key";

static char llnet_EAI_Errors_key[] = "llnet_EAI_Errors_key";

#ifndef __WINDOWS__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <netdb.h>
#endif


typedef struct sockaddr sockaddr;

#ifdef __WINDOWS__
static void lsock_init_winsock2(lua_State *L) {
	WSADATA wsaData;
	int err;

	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(err != 0) {
		luaL_error(L, "winsock2 failed with error: %d\n", err);
	}
}
#else
#define lsock_init_winsock2(L)
#endif

#ifdef IP_RECVOPTS
errno_rc lsocket_opt_set_IP_RECVOPTS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_RECVOPTS, value);
}
errno_rc lsocket_opt_get_IP_RECVOPTS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_RECVOPTS, value);
}

#endif
#ifdef IP_MTU
errno_rc lsocket_opt_set_IP_MTU(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_MTU, value);
}
errno_rc lsocket_opt_get_IP_MTU(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_MTU, value);
}

#endif
#ifdef IP_TOS
errno_rc lsocket_opt_set_IP_TOS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_TOS, value);
}
errno_rc lsocket_opt_get_IP_TOS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_TOS, value);
}

#endif
#ifdef IP_OPTIONS
errno_rc lsocket_opt_set_IP_OPTIONS(LSocketFD sock, const char *value, size_t len) {
	return l_socket_set_option(sock, SOL_IP, IP_OPTIONS, value, len);
}
errno_rc lsocket_opt_get_IP_OPTIONS(LSocketFD sock, char *value, size_t *len) {
	socklen_t slen = *len;
	int rc = l_socket_get_option(sock, SOL_IP, IP_OPTIONS, value, &slen);
	*len = slen;
	return rc;
}

#endif
#ifdef IP_TTL
errno_rc lsocket_opt_set_IP_TTL(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_TTL, value);
}
errno_rc lsocket_opt_get_IP_TTL(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_TTL, value);
}

#endif
#ifdef IP_RECVTOS
errno_rc lsocket_opt_set_IP_RECVTOS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_RECVTOS, value);
}
errno_rc lsocket_opt_get_IP_RECVTOS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_RECVTOS, value);
}

#endif
#ifdef IP_MINTTL
errno_rc lsocket_opt_set_IP_MINTTL(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_MINTTL, value);
}
errno_rc lsocket_opt_get_IP_MINTTL(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_MINTTL, value);
}

#endif
#ifdef IP_MULTICAST_TTL
errno_rc lsocket_opt_set_IP_MULTICAST_TTL(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_MULTICAST_TTL, value);
}
errno_rc lsocket_opt_get_IP_MULTICAST_TTL(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_MULTICAST_TTL, value);
}

#endif
#ifdef IP_RECVTTL
errno_rc lsocket_opt_set_IP_RECVTTL(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_RECVTTL, value);
}
errno_rc lsocket_opt_get_IP_RECVTTL(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_RECVTTL, value);
}

#endif
#ifdef IP_FREEBIND
errno_rc lsocket_opt_set_IP_FREEBIND(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_FREEBIND, value);
}
errno_rc lsocket_opt_get_IP_FREEBIND(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_FREEBIND, value);
}

#endif
#ifdef IP_PKTINFO
errno_rc lsocket_opt_set_IP_PKTINFO(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_PKTINFO, value);
}
errno_rc lsocket_opt_get_IP_PKTINFO(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_PKTINFO, value);
}

#endif
#ifdef IP_HDRINCL
errno_rc lsocket_opt_set_IP_HDRINCL(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_HDRINCL, value);
}
errno_rc lsocket_opt_get_IP_HDRINCL(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_HDRINCL, value);
}

#endif
#ifdef IP_MTU_DISCOVER
errno_rc lsocket_opt_set_IP_MTU_DISCOVER(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_MTU_DISCOVER, value);
}
errno_rc lsocket_opt_get_IP_MTU_DISCOVER(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_MTU_DISCOVER, value);
}

#endif
#ifdef IP_RETOPTS
errno_rc lsocket_opt_set_IP_RETOPTS(LSocketFD sock, const char *value, size_t len) {
	return l_socket_set_option(sock, SOL_IP, IP_RETOPTS, value, len);
}
errno_rc lsocket_opt_get_IP_RETOPTS(LSocketFD sock, char *value, size_t *len) {
	socklen_t slen = *len;
	int rc = l_socket_get_option(sock, SOL_IP, IP_RETOPTS, value, &slen);
	*len = slen;
	return rc;
}

#endif
#ifdef IP_RECVERR
errno_rc lsocket_opt_set_IP_RECVERR(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_RECVERR, value);
}
errno_rc lsocket_opt_get_IP_RECVERR(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_RECVERR, value);
}

#endif
#ifdef IP_ROUTER_ALERT
errno_rc lsocket_opt_set_IP_ROUTER_ALERT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_ROUTER_ALERT, value);
}
errno_rc lsocket_opt_get_IP_ROUTER_ALERT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_ROUTER_ALERT, value);
}

#endif
#ifdef IP_RECVRETOPTS
errno_rc lsocket_opt_set_IP_RECVRETOPTS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_RECVRETOPTS, value);
}
errno_rc lsocket_opt_get_IP_RECVRETOPTS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_RECVRETOPTS, value);
}

#endif
#ifdef IP_RECVORIGDSTADDR
errno_rc lsocket_opt_set_IP_RECVORIGDSTADDR(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_RECVORIGDSTADDR, value);
}
errno_rc lsocket_opt_get_IP_RECVORIGDSTADDR(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_RECVORIGDSTADDR, value);
}

#endif
#ifdef IP_MULTICAST_LOOP
errno_rc lsocket_opt_set_IP_MULTICAST_LOOP(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IP, IP_MULTICAST_LOOP, value);
}
errno_rc lsocket_opt_get_IP_MULTICAST_LOOP(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IP, IP_MULTICAST_LOOP, value);
}

#endif
#ifdef IPV6_MULTICAST_HOPS
errno_rc lsocket_opt_set_IPV6_MULTICAST_HOPS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_MULTICAST_HOPS, value);
}
errno_rc lsocket_opt_get_IPV6_MULTICAST_HOPS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_MULTICAST_HOPS, value);
}

#endif
#ifdef IPV6_RECVERR
errno_rc lsocket_opt_set_IPV6_RECVERR(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_RECVERR, value);
}
errno_rc lsocket_opt_get_IPV6_RECVERR(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_RECVERR, value);
}

#endif
#ifdef IPV6_CHECKSUM
errno_rc lsocket_opt_set_IPV6_CHECKSUM(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_CHECKSUM, value);
}
errno_rc lsocket_opt_get_IPV6_CHECKSUM(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_CHECKSUM, value);
}

#endif
#ifdef IPV6_MULTICAST_LOOP
errno_rc lsocket_opt_set_IPV6_MULTICAST_LOOP(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_MULTICAST_LOOP, value);
}
errno_rc lsocket_opt_get_IPV6_MULTICAST_LOOP(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_MULTICAST_LOOP, value);
}

#endif
#ifdef IPV6_MULTICAST_IF
errno_rc lsocket_opt_set_IPV6_MULTICAST_IF(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_MULTICAST_IF, value);
}
errno_rc lsocket_opt_get_IPV6_MULTICAST_IF(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_MULTICAST_IF, value);
}

#endif
#ifdef IPV6_RECVRTHDR
errno_rc lsocket_opt_set_IPV6_RECVRTHDR(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_RECVRTHDR, value);
}
errno_rc lsocket_opt_get_IPV6_RECVRTHDR(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_RECVRTHDR, value);
}

#endif
#ifdef IPV6_MTU_DISCOVER
errno_rc lsocket_opt_set_IPV6_MTU_DISCOVER(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_MTU_DISCOVER, value);
}
errno_rc lsocket_opt_get_IPV6_MTU_DISCOVER(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_MTU_DISCOVER, value);
}

#endif
#ifdef IPV6_UNICAST_HOPS
errno_rc lsocket_opt_set_IPV6_UNICAST_HOPS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_UNICAST_HOPS, value);
}
errno_rc lsocket_opt_get_IPV6_UNICAST_HOPS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_UNICAST_HOPS, value);
}

#endif
#ifdef IPV6_NEXTHOP
errno_rc lsocket_opt_set_IPV6_NEXTHOP(LSocketFD sock, LSockAddr *addr) {
	return l_socket_set_option(sock, SOL_IPV6, IPV6_NEXTHOP, L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr));
}
errno_rc lsocket_opt_get_IPV6_NEXTHOP(LSocketFD sock, LSockAddr *addr) {
	struct sockaddr_storage saddr;
	socklen_t slen = sizeof(saddr);
	int rc = l_socket_get_option(sock, SOL_IPV6, IPV6_NEXTHOP, &addr, &slen);
	l_sockaddr_init(addr);
	l_sockaddr_fill(addr, (struct sockaddr *)&saddr, slen);
	return rc;
}

#endif
#ifdef IPV6_RECVHOPLIMIT
errno_rc lsocket_opt_set_IPV6_RECVHOPLIMIT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_RECVHOPLIMIT, value);
}
errno_rc lsocket_opt_get_IPV6_RECVHOPLIMIT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_RECVHOPLIMIT, value);
}

#endif
#ifdef IPV6_MTU
errno_rc lsocket_opt_set_IPV6_MTU(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_MTU, value);
}
errno_rc lsocket_opt_get_IPV6_MTU(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_MTU, value);
}

#endif
#ifdef IPV6_HOPLIMIT
errno_rc lsocket_opt_set_IPV6_HOPLIMIT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_HOPLIMIT, value);
}
errno_rc lsocket_opt_get_IPV6_HOPLIMIT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_HOPLIMIT, value);
}

#endif
#ifdef IPV6_ADDRFORM
errno_rc lsocket_opt_set_IPV6_ADDRFORM(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_ADDRFORM, value);
}

#endif
#ifdef IPV6_V6ONLY
errno_rc lsocket_opt_set_IPV6_V6ONLY(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_V6ONLY, value);
}
errno_rc lsocket_opt_get_IPV6_V6ONLY(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_V6ONLY, value);
}

#endif
#ifdef IPV6_RECVDSTOPTS
errno_rc lsocket_opt_set_IPV6_RECVDSTOPTS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_RECVDSTOPTS, value);
}
errno_rc lsocket_opt_get_IPV6_RECVDSTOPTS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_RECVDSTOPTS, value);
}

#endif
#ifdef IPV6_TCLASS
errno_rc lsocket_opt_set_IPV6_TCLASS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_TCLASS, value);
}
errno_rc lsocket_opt_get_IPV6_TCLASS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_TCLASS, value);
}

#endif
#ifdef IPV6_RECVTCLASS
errno_rc lsocket_opt_set_IPV6_RECVTCLASS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_RECVTCLASS, value);
}
errno_rc lsocket_opt_get_IPV6_RECVTCLASS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_RECVTCLASS, value);
}

#endif
#ifdef IPV6_RECVHOPOPTS
errno_rc lsocket_opt_set_IPV6_RECVHOPOPTS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_RECVHOPOPTS, value);
}
errno_rc lsocket_opt_get_IPV6_RECVHOPOPTS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_RECVHOPOPTS, value);
}

#endif
#ifdef IPV6_RECVPKTINFO
errno_rc lsocket_opt_set_IPV6_RECVPKTINFO(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_RECVPKTINFO, value);
}
errno_rc lsocket_opt_get_IPV6_RECVPKTINFO(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_RECVPKTINFO, value);
}

#endif
#ifdef IPV6_ROUTER_ALERT
errno_rc lsocket_opt_set_IPV6_ROUTER_ALERT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_IPV6, IPV6_ROUTER_ALERT, value);
}
errno_rc lsocket_opt_get_IPV6_ROUTER_ALERT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_IPV6, IPV6_ROUTER_ALERT, value);
}

#endif
#ifdef SO_SNDBUFFORCE
errno_rc lsocket_opt_set_SO_SNDBUFFORCE(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_SNDBUFFORCE, value);
}
errno_rc lsocket_opt_get_SO_SNDBUFFORCE(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_SNDBUFFORCE, value);
}

#endif
#ifdef SO_ACCEPTCONN
errno_rc lsocket_opt_get_SO_ACCEPTCONN(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_ACCEPTCONN, value);
}

#endif
#ifdef SO_PROTOCOL
errno_rc lsocket_opt_get_SO_PROTOCOL(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_PROTOCOL, value);
}

#endif
#ifdef SO_TIMESTAMP
errno_rc lsocket_opt_set_SO_TIMESTAMP(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_TIMESTAMP, value);
}
errno_rc lsocket_opt_get_SO_TIMESTAMP(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_TIMESTAMP, value);
}

#endif
#ifdef SO_SNDBUF
errno_rc lsocket_opt_set_SO_SNDBUF(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_SNDBUF, value);
}
errno_rc lsocket_opt_get_SO_SNDBUF(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_SNDBUF, value);
}

#endif
#ifdef SO_NO_CHECK
errno_rc lsocket_opt_set_SO_NO_CHECK(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_NO_CHECK, value);
}
errno_rc lsocket_opt_get_SO_NO_CHECK(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_NO_CHECK, value);
}

#endif
#ifdef SO_BSDCOMPAT
errno_rc lsocket_opt_set_SO_BSDCOMPAT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_BSDCOMPAT, value);
}
errno_rc lsocket_opt_get_SO_BSDCOMPAT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_BSDCOMPAT, value);
}

#endif
#ifdef SO_REUSEADDR
errno_rc lsocket_opt_set_SO_REUSEADDR(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_REUSEADDR, value);
}
errno_rc lsocket_opt_get_SO_REUSEADDR(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_REUSEADDR, value);
}

#endif
#ifdef SO_MARK
errno_rc lsocket_opt_set_SO_MARK(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_MARK, value);
}
errno_rc lsocket_opt_get_SO_MARK(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_MARK, value);
}

#endif
#ifdef SO_TIMESTAMPING
errno_rc lsocket_opt_set_SO_TIMESTAMPING(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_TIMESTAMPING, value);
}
errno_rc lsocket_opt_get_SO_TIMESTAMPING(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_TIMESTAMPING, value);
}

#endif
#ifdef SO_TIMESTAMPNS
errno_rc lsocket_opt_set_SO_TIMESTAMPNS(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_TIMESTAMPNS, value);
}
errno_rc lsocket_opt_get_SO_TIMESTAMPNS(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_TIMESTAMPNS, value);
}

#endif
#ifdef SO_BINDTODEVICE
errno_rc lsocket_opt_set_SO_BINDTODEVICE(LSocketFD sock, const char *value, size_t len) {
	return l_socket_set_option(sock, SOL_SOCKET, SO_BINDTODEVICE, value, len);
}
errno_rc lsocket_opt_get_SO_BINDTODEVICE(LSocketFD sock, char *value, size_t *len) {
	socklen_t slen = *len;
	int rc = l_socket_get_option(sock, SOL_SOCKET, SO_BINDTODEVICE, value, &slen);
	*len = slen;
	return rc;
}

#endif
#ifdef SO_DONTROUTE
errno_rc lsocket_opt_set_SO_DONTROUTE(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_DONTROUTE, value);
}
errno_rc lsocket_opt_get_SO_DONTROUTE(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_DONTROUTE, value);
}

#endif
#ifdef SO_RCVBUFFORCE
errno_rc lsocket_opt_set_SO_RCVBUFFORCE(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_RCVBUFFORCE, value);
}
errno_rc lsocket_opt_get_SO_RCVBUFFORCE(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_RCVBUFFORCE, value);
}

#endif
#ifdef SO_SNDLOWAT
errno_rc lsocket_opt_set_SO_SNDLOWAT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_SNDLOWAT, value);
}
errno_rc lsocket_opt_get_SO_SNDLOWAT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_SNDLOWAT, value);
}

#endif
#ifdef SO_OOBINLINE
errno_rc lsocket_opt_set_SO_OOBINLINE(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_OOBINLINE, value);
}
errno_rc lsocket_opt_get_SO_OOBINLINE(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_OOBINLINE, value);
}

#endif
#ifdef SO_DOMAIN
errno_rc lsocket_opt_get_SO_DOMAIN(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_DOMAIN, value);
}

#endif
#ifdef SO_RCVLOWAT
errno_rc lsocket_opt_set_SO_RCVLOWAT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_RCVLOWAT, value);
}
errno_rc lsocket_opt_get_SO_RCVLOWAT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_RCVLOWAT, value);
}

#endif
#ifdef SO_PASSCRED
errno_rc lsocket_opt_set_SO_PASSCRED(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_PASSCRED, value);
}
errno_rc lsocket_opt_get_SO_PASSCRED(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_PASSCRED, value);
}

#endif
#ifdef SO_RCVBUF
errno_rc lsocket_opt_set_SO_RCVBUF(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_RCVBUF, value);
}
errno_rc lsocket_opt_get_SO_RCVBUF(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_RCVBUF, value);
}

#endif
#ifdef SO_PRIORITY
errno_rc lsocket_opt_set_SO_PRIORITY(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_PRIORITY, value);
}
errno_rc lsocket_opt_get_SO_PRIORITY(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_PRIORITY, value);
}

#endif
#ifdef SO_TYPE
errno_rc lsocket_opt_get_SO_TYPE(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_TYPE, value);
}

#endif
#ifdef SO_BROADCAST
errno_rc lsocket_opt_set_SO_BROADCAST(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_BROADCAST, value);
}
errno_rc lsocket_opt_get_SO_BROADCAST(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_BROADCAST, value);
}

#endif
#ifdef SO_KEEPALIVE
errno_rc lsocket_opt_set_SO_KEEPALIVE(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_KEEPALIVE, value);
}
errno_rc lsocket_opt_get_SO_KEEPALIVE(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_KEEPALIVE, value);
}

#endif
#ifdef SO_DEBUG
errno_rc lsocket_opt_set_SO_DEBUG(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_SOCKET, SO_DEBUG, value);
}
errno_rc lsocket_opt_get_SO_DEBUG(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_DEBUG, value);
}

#endif
#ifdef SO_ERROR
errno_rc lsocket_opt_get_SO_ERROR(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_SOCKET, SO_ERROR, value);
}

#endif
#ifdef TCP_CORK
errno_rc lsocket_opt_set_TCP_CORK(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_CORK, value);
}
errno_rc lsocket_opt_get_TCP_CORK(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_CORK, value);
}

#endif
#ifdef TCP_KEEPIDLE
errno_rc lsocket_opt_set_TCP_KEEPIDLE(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_KEEPIDLE, value);
}
errno_rc lsocket_opt_get_TCP_KEEPIDLE(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_KEEPIDLE, value);
}

#endif
#ifdef TCP_QUICKACK
errno_rc lsocket_opt_set_TCP_QUICKACK(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_QUICKACK, value);
}
errno_rc lsocket_opt_get_TCP_QUICKACK(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_QUICKACK, value);
}

#endif
#ifdef TCP_CONGESTION
errno_rc lsocket_opt_set_TCP_CONGESTION(LSocketFD sock, const char *value, size_t len) {
	return l_socket_set_option(sock, SOL_TCP, TCP_CONGESTION, value, len);
}
errno_rc lsocket_opt_get_TCP_CONGESTION(LSocketFD sock, char *value, size_t *len) {
	socklen_t slen = *len;
	int rc = l_socket_get_option(sock, SOL_TCP, TCP_CONGESTION, value, &slen);
	*len = slen;
	return rc;
}

#endif
#ifdef TCP_WINDOW_CLAMP
errno_rc lsocket_opt_set_TCP_WINDOW_CLAMP(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_WINDOW_CLAMP, value);
}
errno_rc lsocket_opt_get_TCP_WINDOW_CLAMP(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_WINDOW_CLAMP, value);
}

#endif
#ifdef TCP_DEFER_ACCEPT
errno_rc lsocket_opt_set_TCP_DEFER_ACCEPT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_DEFER_ACCEPT, value);
}
errno_rc lsocket_opt_get_TCP_DEFER_ACCEPT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_DEFER_ACCEPT, value);
}

#endif
#ifdef TCP_MAXSEG
errno_rc lsocket_opt_set_TCP_MAXSEG(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_MAXSEG, value);
}
errno_rc lsocket_opt_get_TCP_MAXSEG(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_MAXSEG, value);
}

#endif
#ifdef TCP_NODELAY
errno_rc lsocket_opt_set_TCP_NODELAY(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_NODELAY, value);
}
errno_rc lsocket_opt_get_TCP_NODELAY(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_NODELAY, value);
}

#endif
#ifdef TCP_KEEPCNT
errno_rc lsocket_opt_set_TCP_KEEPCNT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_KEEPCNT, value);
}
errno_rc lsocket_opt_get_TCP_KEEPCNT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_KEEPCNT, value);
}

#endif
#ifdef TCP_LINGER2
errno_rc lsocket_opt_set_TCP_LINGER2(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_LINGER2, value);
}
errno_rc lsocket_opt_get_TCP_LINGER2(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_LINGER2, value);
}

#endif
#ifdef TCP_SYNCNT
errno_rc lsocket_opt_set_TCP_SYNCNT(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_SYNCNT, value);
}
errno_rc lsocket_opt_get_TCP_SYNCNT(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_SYNCNT, value);
}

#endif
#ifdef TCP_KEEPINTVL
errno_rc lsocket_opt_set_TCP_KEEPINTVL(LSocketFD sock, int value) {
	return l_socket_set_int_option(sock, SOL_TCP, TCP_KEEPINTVL, value);
}
errno_rc lsocket_opt_get_TCP_KEEPINTVL(LSocketFD sock, int *value) {
	return l_socket_get_int_option(sock, SOL_TCP, TCP_KEEPINTVL, value);
}

#endif



/* method: socketpair */
static int llnet__socketpair__func(lua_State *L) {
  int type1;
  int flags2;
  LSocketFD sock11;
  LSocketFD sock22;
  errno_rc rc3 = 0;
	LSocketFD sv[2];

  type1 = luaL_checkinteger(L,1);
  flags2 = luaL_optinteger(L,2,0);
	rc3 = l_socket_pair(type1, flags2, sv);
	if(rc3 == 0) {
		sock11 = sv[0];
		sock22 = sv[1];
	}

  if(!(-1 == rc3)) {
    obj_type_LSocketFD_push(L, sock11);
  } else {
    lua_pushnil(L);
  }
  if(!(-1 == rc3)) {
    obj_type_LSocketFD_push(L, sock22);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc3);
  return 3;
}

/* method: description */
static int Errors__description__meth(lua_State *L) {
  size_t msg_len1 = 1023;
  char msg1_buf[1024];
  char * msg1 = msg1_buf;
	int err_type;
	int err_num = -1;

	err_type = lua_type(L, 2);
	if(err_type == LUA_TSTRING) {
		lua_pushvalue(L, 2);
		lua_rawget(L, 1);
		if(lua_isnumber(L, -1)) {
			err_num = lua_tointeger(L, -1);
		}
		lua_pop(L, 1);
	} else if(err_type == LUA_TNUMBER) {
		err_num = lua_tointeger(L, 2);
	} else {
		return luaL_argerror(L, 2, "expected string/number");
	}
	if(err_num < 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "UNKNOWN ERROR");
		return 2;
	}
	strerror_r(err_num, msg1, msg_len1);
	msg_len1 = strlen(msg1);

  if(msg1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, msg1,msg_len1);
  return 1;
}

static void error_code__errno_rc__push(lua_State *L, errno_rc err) {
  const char *err_str = NULL;
	if(-1 == err) {
		/* get Errors table. */
		lua_pushlightuserdata(L, llnet_Errors_key);
		lua_rawget(L, LUA_REGISTRYINDEX);
		/* convert errno to string. */
		lua_rawgeti(L, -1, errno);
		/* remove Errors table. */
		lua_remove(L, -2);
		if(!lua_isnil(L, -1)) {
			/* found error. */
			return;
		}
		/* Unknown error. */
		lua_pop(L, 1);
		err_str = "UNKNOWN ERROR";
	}

	if(err_str) {
		lua_pushstring(L, err_str);
	} else {
		lua_pushnil(L);
	}
}

/* method: description */
static int EAI_Errors__description__meth(lua_State *L) {
  const char * msg1 = NULL;
	int err_type;
	int err_num = -1;

	err_type = lua_type(L, 2);
	if(err_type == LUA_TSTRING) {
		lua_pushvalue(L, 2);
		lua_rawget(L, 1);
		if(lua_isnumber(L, -1)) {
			err_num = lua_tointeger(L, -1);
		}
		lua_pop(L, 1);
	} else if(err_type == LUA_TNUMBER) {
		err_num = lua_tointeger(L, 2);
	} else {
		return luaL_argerror(L, 2, "expected string/number");
	}
	msg1 = gai_strerror(err_num);

  lua_pushstring(L, msg1);
  return 1;
}

static void error_code__eai_rc__push(lua_State *L, eai_rc err) {
  const char *err_str = NULL;
	if(0 != err) {
		/* get EAI_Errors table. */
		lua_pushlightuserdata(L, llnet_EAI_Errors_key);
		lua_rawget(L, LUA_REGISTRYINDEX);
		/* convert error to string. */
		lua_rawgeti(L, -1, err);
		/* remove EAI_Errors table. */
		lua_remove(L, -2);
		if(!lua_isnil(L, -1)) {
			/* found error. */
			return;
		}
		/* Unknown error. */
		lua_pop(L, 1);
		err_str = "UNKNOWN ERROR";
	}

	if(err_str) {
		lua_pushstring(L, err_str);
	} else {
		lua_pushnil(L);
	}
}

/* method: __index */
static int Protocols____index__meth(lua_State *L) {
	struct protoent *ent;
#if defined(HAVE_getprotobyname_r)
	struct protoent ent_buf;
	char buf[L_PROTO_MAX_BUF];
	int rc;
#endif
	const char *name;
	int proto;
	int key_type;

	luaL_checktype(L, 1, LUA_TTABLE);

	key_type = lua_type(L, 2);
	if(key_type == LUA_TSTRING) {
		name = lua_tostring(L, 2);
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
		proto = lua_tointeger(L, 2);
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
		return luaL_argerror(L, 2, "expected string/number");
	}

	/* cache results. */
	lua_pushvalue(L, 2);
	lua_pushvalue(L, -2);
	lua_rawset(L, 1);

	return 1;

  return 1;
}

/* method: byname */
static int Services__byname__func(lua_State *L) {
  size_t name_len1;
  const char * name1;
  size_t proto_len2;
  const char * proto2;
  name1 = luaL_checklstring(L,1,&(name_len1));
  proto2 = luaL_optlstring(L,2,"tcp",&(proto_len2));
	struct servent *ent;
#if defined(HAVE_getservbyname_r)
	struct servent ent_buf;
	char buf[L_SERV_MAX_BUF];
	int rc;

	rc = getservbyname_r(name1, proto2, &ent_buf, buf, L_SERV_MAX_BUF, &ent);
	if(rc != 0) {
		return luaL_error(L, "getservbyname_r(): buffer to small.");
	}
#else
	ent = getservbyname(name1, proto2);
#endif
	if(ent == NULL) {
		return luaL_error(L, "Unknown service: name=%s, proto=%s", name1, proto2);
	}

	lua_pushstring(L, ent->s_name);
	lua_pushinteger(L, ntohs(ent->s_port));
	lua_pushstring(L, ent->s_proto);

	return 3;

  return 0;
}

/* method: byport */
static int Services__byport__func(lua_State *L) {
  int port1;
  size_t proto_len2;
  const char * proto2;
  port1 = luaL_checkinteger(L,1);
  proto2 = luaL_optlstring(L,2,"tcp",&(proto_len2));
	struct servent *ent;
#if defined(HAVE_getservbyport_r)
	struct servent ent_buf;
	char buf[L_SERV_MAX_BUF];
	int rc;

	rc = getservbyport_r(htons(port1), proto2, &ent_buf, buf, L_SERV_MAX_BUF, &ent);
	if(rc != 0) {
		return luaL_error(L, "getservbyport_r(): buffer to small.");
	}
#else
	ent = getservbyport(htons(port1), proto2);
#endif

	if(ent == NULL) {
		return luaL_error(L, "Unknown service: port=%d, proto=%s", port1, proto2);
	}

	lua_pushstring(L, ent->s_name);
	lua_pushinteger(L, ntohs(ent->s_port));
	lua_pushstring(L, ent->s_proto);

	return 3;

  return 0;
}

/* method: __index */
static int Services____index__meth(lua_State *L) {
	struct servent *ent;
#if defined(HAVE_getservbyname_r)
	struct servent ent_buf;
	char buf[L_SERV_MAX_BUF];
	int rc;
#endif
	const char *name;
	int port;
	int key_type;

	luaL_checktype(L, 1, LUA_TTABLE);

	key_type = lua_type(L, 2);
	if(key_type == LUA_TSTRING) {
		name = lua_tostring(L, 2);
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
		port = lua_tointeger(L, 2);
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
		return luaL_argerror(L, 2, "expected string/number");
	}

	/* cache results. */
	lua_pushvalue(L, 2);
	lua_pushvalue(L, -2);
	lua_rawset(L, 1);

	return 1;

  return 1;
}

/* method: new */
static int LSockAddr__new__meth(lua_State *L) {
  LSockAddr this1_store;
  LSockAddr * this1 = &(this1_store);
  int rc_l_sockaddr_init2 = 0;
  rc_l_sockaddr_init2 = l_sockaddr_init(this1);
  obj_type_LSockAddr_push(L, this1);
  lua_pushinteger(L, rc_l_sockaddr_init2);
  return 2;
}

/* method: ip_port */
static int LSockAddr__ip_port__meth(lua_State *L) {
  size_t ip_len1;
  const char * ip1;
  int port2;
  LSockAddr this1_store;
  LSockAddr * this1 = &(this1_store);
  int rc_l_sockaddr_init2 = 0;
  int rc_l_sockaddr_set_ip_port3 = 0;
  ip1 = luaL_checklstring(L,1,&(ip_len1));
  port2 = luaL_checkinteger(L,2);
  rc_l_sockaddr_init2 = l_sockaddr_init(this1);
  rc_l_sockaddr_set_ip_port3 = l_sockaddr_set_ip_port(this1, ip1, port2);
  obj_type_LSockAddr_push(L, this1);
  lua_pushinteger(L, rc_l_sockaddr_init2);
  lua_pushinteger(L, rc_l_sockaddr_set_ip_port3);
  return 3;
}

/* method: unix */
static int LSockAddr__unix__meth(lua_State *L) {
  size_t unix_len1;
  const char * unix1;
  LSockAddr this1_store;
  LSockAddr * this1 = &(this1_store);
  int rc_l_sockaddr_init2 = 0;
  int rc_l_sockaddr_set_unix3 = 0;
  unix1 = luaL_checklstring(L,1,&(unix_len1));
  rc_l_sockaddr_init2 = l_sockaddr_init(this1);
  rc_l_sockaddr_set_unix3 = l_sockaddr_set_unix(this1, unix1);
  obj_type_LSockAddr_push(L, this1);
  lua_pushinteger(L, rc_l_sockaddr_init2);
  lua_pushinteger(L, rc_l_sockaddr_set_unix3);
  return 3;
}

/* method: family */
static int LSockAddr__family__meth(lua_State *L) {
  sa_family_t family1;
  LSockAddr this1_store;
  LSockAddr * this1 = &(this1_store);
  int rc_l_sockaddr_init2 = 0;
  int rc_l_sockaddr_set_family3 = 0;
  family1 = luaL_checkinteger(L,1);
  rc_l_sockaddr_init2 = l_sockaddr_init(this1);
  rc_l_sockaddr_set_family3 = l_sockaddr_set_family(this1, family1);
  obj_type_LSockAddr_push(L, this1);
  lua_pushinteger(L, rc_l_sockaddr_init2);
  lua_pushinteger(L, rc_l_sockaddr_set_family3);
  return 3;
}

/* method: _priv */
static int LSockAddr__delete__meth(lua_State *L) {
  LSockAddr * this1;
  this1 = obj_type_LSockAddr_delete(L,1);
  l_sockaddr_cleanup(this1);
  return 0;
}

/* method: set_ip_port */
static int LSockAddr__set_ip_port__meth(lua_State *L) {
  LSockAddr * this1;
  size_t ip_len2;
  const char * ip2;
  int port3;
  int rc_l_sockaddr_set_ip_port1 = 0;
  this1 = obj_type_LSockAddr_check(L,1);
  ip2 = luaL_checklstring(L,2,&(ip_len2));
  port3 = luaL_checkinteger(L,3);
  rc_l_sockaddr_set_ip_port1 = l_sockaddr_set_ip_port(this1, ip2, port3);
  lua_pushinteger(L, rc_l_sockaddr_set_ip_port1);
  return 1;
}

/* method: set_unix */
static int LSockAddr__set_unix__meth(lua_State *L) {
  LSockAddr * this1;
  size_t unix_len2;
  const char * unix2;
  int rc_l_sockaddr_set_unix1 = 0;
  this1 = obj_type_LSockAddr_check(L,1);
  unix2 = luaL_checklstring(L,2,&(unix_len2));
  rc_l_sockaddr_set_unix1 = l_sockaddr_set_unix(this1, unix2);
  lua_pushinteger(L, rc_l_sockaddr_set_unix1);
  return 1;
}

/* method: resize */
static int LSockAddr__resize__meth(lua_State *L) {
  LSockAddr * this1;
  socklen_t addrlen2;
  this1 = obj_type_LSockAddr_check(L,1);
  addrlen2 = luaL_checkinteger(L,2);
  l_sockaddr_resize(this1, addrlen2);
  return 0;
}

/* method: get_family */
static int LSockAddr__get_family__meth(lua_State *L) {
  LSockAddr * this1;
  sa_family_t rc_l_sockaddr_get_family1 = 0;
  this1 = obj_type_LSockAddr_check(L,1);
  rc_l_sockaddr_get_family1 = l_sockaddr_get_family(this1);
  lua_pushinteger(L, rc_l_sockaddr_get_family1);
  return 1;
}

/* method: set_port */
static int LSockAddr__set_port__meth(lua_State *L) {
  LSockAddr * this1;
  int port2;
  int rc_l_sockaddr_set_port1 = 0;
  this1 = obj_type_LSockAddr_check(L,1);
  port2 = luaL_checkinteger(L,2);
  rc_l_sockaddr_set_port1 = l_sockaddr_set_port(this1, port2);
  lua_pushinteger(L, rc_l_sockaddr_set_port1);
  return 1;
}

/* method: get_port */
static int LSockAddr__get_port__meth(lua_State *L) {
  LSockAddr * this1;
  int rc_l_sockaddr_get_port1 = 0;
  this1 = obj_type_LSockAddr_check(L,1);
  rc_l_sockaddr_get_port1 = l_sockaddr_get_port(this1);
  lua_pushinteger(L, rc_l_sockaddr_get_port1);
  return 1;
}

/* method: addr */
static int LSockAddr__addr__meth(lua_State *L) {
  LSockAddr * this1;
  sockaddr * rc_l_sockaddr_get_addr1 = NULL;
  this1 = obj_type_LSockAddr_check(L,1);
  rc_l_sockaddr_get_addr1 = l_sockaddr_get_addr(this1);
  lua_pushlightuserdata(L, rc_l_sockaddr_get_addr1);
  return 1;
}

/* method: addrlen */
static int LSockAddr__addrlen__meth(lua_State *L) {
  LSockAddr * this1;
  socklen_t rc_l_sockaddr_get_addrlen1 = 0;
  this1 = obj_type_LSockAddr_check(L,1);
  rc_l_sockaddr_get_addrlen1 = l_sockaddr_get_addrlen(this1);
  lua_pushinteger(L, rc_l_sockaddr_get_addrlen1);
  return 1;
}

/* method: __tostring */
static int LSockAddr____tostring__meth(lua_State *L) {
  LSockAddr * this1;
  size_t str_len1 = 1023;
  char str1_buf[1024];
  char * str1 = str1_buf;
  this1 = obj_type_LSockAddr_check(L,1);
	str_len1 = l_sockaddr_tostring(this1, str1, str_len1);

  if(str1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, str1,str_len1);
  return 1;
}

/* method: lookup_full */
static int LSockAddr__lookup_full__meth(lua_State *L) {
  LSockAddr * this1;
  size_t host_len2;
  const char * host2;
  size_t port_len3;
  const char * port3;
  int ai_family4;
  int ai_socktype5;
  int ai_protocol6;
  int ai_flags7;
  eai_rc rc_l_sockaddr_lookup_full1 = 0;
  this1 = obj_type_LSockAddr_check(L,1);
  host2 = luaL_checklstring(L,2,&(host_len2));
  port3 = luaL_optlstring(L,3,NULL,&(port_len3));
  ai_family4 = luaL_optinteger(L,4,0);
  ai_socktype5 = luaL_optinteger(L,5,0);
  ai_protocol6 = luaL_optinteger(L,6,0);
  ai_flags7 = luaL_optinteger(L,7,0);
  rc_l_sockaddr_lookup_full1 = l_sockaddr_lookup_full(this1, host2, port3, ai_family4, ai_socktype5, ai_protocol6, ai_flags7);
  /* check for error. */
  if((0 != rc_l_sockaddr_lookup_full1)) {
    lua_pushnil(L);
      error_code__eai_rc__push(L, rc_l_sockaddr_lookup_full1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: new */
static int LAddrInfo__new__meth(lua_State *L) {
  size_t host_len1;
  const char * host1;
  size_t port_len2;
  const char * port2;
  LAddrInfo this1_store;
  LAddrInfo * this1 = &(this1_store);
  eai_rc rc_l_addrinfo_init_ip2 = 0;
  host1 = luaL_checklstring(L,1,&(host_len1));
  port2 = luaL_optlstring(L,2,NULL,&(port_len2));
  rc_l_addrinfo_init_ip2 = l_addrinfo_init_ip(this1, host1, port2);
  if(!(0 != rc_l_addrinfo_init_ip2)) {
    obj_type_LAddrInfo_push(L, this1);
  } else {
    lua_pushnil(L);
  }
  error_code__eai_rc__push(L, rc_l_addrinfo_init_ip2);
  return 2;
}

/* method: ipv4 */
static int LAddrInfo__ipv4__meth(lua_State *L) {
  size_t host_len1;
  const char * host1;
  size_t port_len2;
  const char * port2;
  LAddrInfo this1_store;
  LAddrInfo * this1 = &(this1_store);
  eai_rc rc_l_addrinfo_init_ipv42 = 0;
  host1 = luaL_checklstring(L,1,&(host_len1));
  port2 = luaL_optlstring(L,2,NULL,&(port_len2));
  rc_l_addrinfo_init_ipv42 = l_addrinfo_init_ipv4(this1, host1, port2);
  if(!(0 != rc_l_addrinfo_init_ipv42)) {
    obj_type_LAddrInfo_push(L, this1);
  } else {
    lua_pushnil(L);
  }
  error_code__eai_rc__push(L, rc_l_addrinfo_init_ipv42);
  return 2;
}

/* method: ipv6 */
static int LAddrInfo__ipv6__meth(lua_State *L) {
  size_t host_len1;
  const char * host1;
  size_t port_len2;
  const char * port2;
  LAddrInfo this1_store;
  LAddrInfo * this1 = &(this1_store);
  eai_rc rc_l_addrinfo_init_ipv62 = 0;
  host1 = luaL_checklstring(L,1,&(host_len1));
  port2 = luaL_optlstring(L,2,NULL,&(port_len2));
  rc_l_addrinfo_init_ipv62 = l_addrinfo_init_ipv6(this1, host1, port2);
  if(!(0 != rc_l_addrinfo_init_ipv62)) {
    obj_type_LAddrInfo_push(L, this1);
  } else {
    lua_pushnil(L);
  }
  error_code__eai_rc__push(L, rc_l_addrinfo_init_ipv62);
  return 2;
}

/* method: full */
static int LAddrInfo__full__meth(lua_State *L) {
  size_t host_len1;
  const char * host1;
  size_t port_len2;
  const char * port2;
  int ai_family3;
  int ai_socktype4;
  int ai_protocol5;
  int ai_flags6;
  LAddrInfo this1_store;
  LAddrInfo * this1 = &(this1_store);
  eai_rc rc_l_addrinfo_init_full2 = 0;
  host1 = luaL_checklstring(L,1,&(host_len1));
  port2 = luaL_optlstring(L,2,NULL,&(port_len2));
  ai_family3 = luaL_optinteger(L,3,0);
  ai_socktype4 = luaL_optinteger(L,4,0);
  ai_protocol5 = luaL_optinteger(L,5,0);
  ai_flags6 = luaL_optinteger(L,6,0);
  rc_l_addrinfo_init_full2 = l_addrinfo_init_full(this1, host1, port2, ai_family3, ai_socktype4, ai_protocol5, ai_flags6);
  if(!(0 != rc_l_addrinfo_init_full2)) {
    obj_type_LAddrInfo_push(L, this1);
  } else {
    lua_pushnil(L);
  }
  error_code__eai_rc__push(L, rc_l_addrinfo_init_full2);
  return 2;
}

/* method: _priv */
static int LAddrInfo__delete__meth(lua_State *L) {
  LAddrInfo * this1;
  this1 = obj_type_LAddrInfo_delete(L,1);
  l_addrinfo_cleanup(this1);
  return 0;
}

/* method: first */
static int LAddrInfo__first__meth(lua_State *L) {
  LAddrInfo * this1;
  bool rc_l_addrinfo_first1 = 0;
  this1 = obj_type_LAddrInfo_check(L,1);
  rc_l_addrinfo_first1 = l_addrinfo_first(this1);
  lua_pushboolean(L, rc_l_addrinfo_first1);
  return 1;
}

/* method: get_addr */
static int LAddrInfo__get_addr__meth(lua_State *L) {
  LAddrInfo * this1;
  LSockAddr * addr2;
  int rc_l_addrinfo_get_addr1 = 0;
  this1 = obj_type_LAddrInfo_check(L,1);
  addr2 = obj_type_LSockAddr_check(L,2);
  rc_l_addrinfo_get_addr1 = l_addrinfo_get_addr(this1, addr2);
  lua_pushinteger(L, rc_l_addrinfo_get_addr1);
  return 1;
}

/* method: get_canonname */
static int LAddrInfo__get_canonname__meth(lua_State *L) {
  LAddrInfo * this1;
  const char * rc_l_addrinfo_get_canonname1 = NULL;
  this1 = obj_type_LAddrInfo_check(L,1);
  rc_l_addrinfo_get_canonname1 = l_addrinfo_get_canonname(this1);
  lua_pushstring(L, rc_l_addrinfo_get_canonname1);
  return 1;
}

/* method: get_family */
static int LAddrInfo__get_family__meth(lua_State *L) {
  LAddrInfo * this1;
  int rc_l_addrinfo_get_family1 = 0;
  this1 = obj_type_LAddrInfo_check(L,1);
  rc_l_addrinfo_get_family1 = l_addrinfo_get_family(this1);
  lua_pushinteger(L, rc_l_addrinfo_get_family1);
  return 1;
}

/* method: get_socktype */
static int LAddrInfo__get_socktype__meth(lua_State *L) {
  LAddrInfo * this1;
  int rc_l_addrinfo_get_socktype1 = 0;
  this1 = obj_type_LAddrInfo_check(L,1);
  rc_l_addrinfo_get_socktype1 = l_addrinfo_get_socktype(this1);
  lua_pushinteger(L, rc_l_addrinfo_get_socktype1);
  return 1;
}

/* method: get_protocol */
static int LAddrInfo__get_protocol__meth(lua_State *L) {
  LAddrInfo * this1;
  int rc_l_addrinfo_get_protocol1 = 0;
  this1 = obj_type_LAddrInfo_check(L,1);
  rc_l_addrinfo_get_protocol1 = l_addrinfo_get_protocol(this1);
  lua_pushinteger(L, rc_l_addrinfo_get_protocol1);
  return 1;
}

/* method: next */
static int LAddrInfo__next__meth(lua_State *L) {
  LAddrInfo * this1;
  bool rc_l_addrinfo_next1 = 0;
  this1 = obj_type_LAddrInfo_check(L,1);
  rc_l_addrinfo_next1 = l_addrinfo_next(this1);
  lua_pushboolean(L, rc_l_addrinfo_next1);
  return 1;
}

/* method: IP_RECVOPTS */
#if (IP_RECVOPTS)
static int SetSocketOption__IP_RECVOPTS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_RECVOPTS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_RECVOPTS1 = lsocket_opt_set_IP_RECVOPTS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_RECVOPTS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_RECVOPTS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_MTU */
#if (IP_MTU)
static int SetSocketOption__IP_MTU__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_MTU1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_MTU1 = lsocket_opt_set_IP_MTU(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_MTU1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_MTU1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_TOS */
#if (IP_TOS)
static int SetSocketOption__IP_TOS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_TOS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_TOS1 = lsocket_opt_set_IP_TOS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_TOS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_TOS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_OPTIONS */
#if (IP_OPTIONS)
static int SetSocketOption__IP_OPTIONS__func(lua_State *L) {
  LSocketFD sock1;
  size_t value_len2;
  const char * value2;
  errno_rc rc_lsocket_opt_set_IP_OPTIONS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lsocket_opt_set_IP_OPTIONS1 = lsocket_opt_set_IP_OPTIONS(sock1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_OPTIONS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_OPTIONS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_TTL */
#if (IP_TTL)
static int SetSocketOption__IP_TTL__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_TTL1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_TTL1 = lsocket_opt_set_IP_TTL(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_TTL1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_TTL1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_RECVTOS */
#if (IP_RECVTOS)
static int SetSocketOption__IP_RECVTOS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_RECVTOS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_RECVTOS1 = lsocket_opt_set_IP_RECVTOS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_RECVTOS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_RECVTOS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_MINTTL */
#if (IP_MINTTL)
static int SetSocketOption__IP_MINTTL__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_MINTTL1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_MINTTL1 = lsocket_opt_set_IP_MINTTL(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_MINTTL1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_MINTTL1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_MULTICAST_TTL */
#if (IP_MULTICAST_TTL)
static int SetSocketOption__IP_MULTICAST_TTL__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_MULTICAST_TTL1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_MULTICAST_TTL1 = lsocket_opt_set_IP_MULTICAST_TTL(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_MULTICAST_TTL1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_MULTICAST_TTL1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_RECVTTL */
#if (IP_RECVTTL)
static int SetSocketOption__IP_RECVTTL__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_RECVTTL1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_RECVTTL1 = lsocket_opt_set_IP_RECVTTL(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_RECVTTL1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_RECVTTL1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_FREEBIND */
#if (IP_FREEBIND)
static int SetSocketOption__IP_FREEBIND__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_FREEBIND1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_FREEBIND1 = lsocket_opt_set_IP_FREEBIND(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_FREEBIND1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_FREEBIND1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_PKTINFO */
#if (IP_PKTINFO)
static int SetSocketOption__IP_PKTINFO__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_PKTINFO1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_PKTINFO1 = lsocket_opt_set_IP_PKTINFO(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_PKTINFO1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_PKTINFO1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_HDRINCL */
#if (IP_HDRINCL)
static int SetSocketOption__IP_HDRINCL__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_HDRINCL1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_HDRINCL1 = lsocket_opt_set_IP_HDRINCL(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_HDRINCL1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_HDRINCL1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_MTU_DISCOVER */
#if (IP_MTU_DISCOVER)
static int SetSocketOption__IP_MTU_DISCOVER__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_MTU_DISCOVER1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_MTU_DISCOVER1 = lsocket_opt_set_IP_MTU_DISCOVER(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_MTU_DISCOVER1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_MTU_DISCOVER1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_RETOPTS */
#if (IP_RETOPTS)
static int SetSocketOption__IP_RETOPTS__func(lua_State *L) {
  LSocketFD sock1;
  size_t value_len2;
  const char * value2;
  errno_rc rc_lsocket_opt_set_IP_RETOPTS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lsocket_opt_set_IP_RETOPTS1 = lsocket_opt_set_IP_RETOPTS(sock1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_RETOPTS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_RETOPTS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_RECVERR */
#if (IP_RECVERR)
static int SetSocketOption__IP_RECVERR__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_RECVERR1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_RECVERR1 = lsocket_opt_set_IP_RECVERR(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_RECVERR1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_RECVERR1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_ROUTER_ALERT */
#if (IP_ROUTER_ALERT)
static int SetSocketOption__IP_ROUTER_ALERT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_ROUTER_ALERT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_ROUTER_ALERT1 = lsocket_opt_set_IP_ROUTER_ALERT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_ROUTER_ALERT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_ROUTER_ALERT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_RECVRETOPTS */
#if (IP_RECVRETOPTS)
static int SetSocketOption__IP_RECVRETOPTS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_RECVRETOPTS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_RECVRETOPTS1 = lsocket_opt_set_IP_RECVRETOPTS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_RECVRETOPTS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_RECVRETOPTS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_RECVORIGDSTADDR */
#if (IP_RECVORIGDSTADDR)
static int SetSocketOption__IP_RECVORIGDSTADDR__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_RECVORIGDSTADDR1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_RECVORIGDSTADDR1 = lsocket_opt_set_IP_RECVORIGDSTADDR(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_RECVORIGDSTADDR1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_RECVORIGDSTADDR1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_MULTICAST_LOOP */
#if (IP_MULTICAST_LOOP)
static int SetSocketOption__IP_MULTICAST_LOOP__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IP_MULTICAST_LOOP1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IP_MULTICAST_LOOP1 = lsocket_opt_set_IP_MULTICAST_LOOP(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IP_MULTICAST_LOOP1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IP_MULTICAST_LOOP1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_MULTICAST_HOPS */
#if (IPV6_MULTICAST_HOPS)
static int SetSocketOption__IPV6_MULTICAST_HOPS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_MULTICAST_HOPS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_MULTICAST_HOPS1 = lsocket_opt_set_IPV6_MULTICAST_HOPS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_MULTICAST_HOPS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_MULTICAST_HOPS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_RECVERR */
#if (IPV6_RECVERR)
static int SetSocketOption__IPV6_RECVERR__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_RECVERR1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_RECVERR1 = lsocket_opt_set_IPV6_RECVERR(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_RECVERR1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_RECVERR1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_CHECKSUM */
#if (IPV6_CHECKSUM)
static int SetSocketOption__IPV6_CHECKSUM__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_CHECKSUM1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_CHECKSUM1 = lsocket_opt_set_IPV6_CHECKSUM(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_CHECKSUM1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_CHECKSUM1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_MULTICAST_LOOP */
#if (IPV6_MULTICAST_LOOP)
static int SetSocketOption__IPV6_MULTICAST_LOOP__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_MULTICAST_LOOP1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_MULTICAST_LOOP1 = lsocket_opt_set_IPV6_MULTICAST_LOOP(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_MULTICAST_LOOP1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_MULTICAST_LOOP1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_MULTICAST_IF */
#if (IPV6_MULTICAST_IF)
static int SetSocketOption__IPV6_MULTICAST_IF__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_MULTICAST_IF1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_MULTICAST_IF1 = lsocket_opt_set_IPV6_MULTICAST_IF(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_MULTICAST_IF1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_MULTICAST_IF1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_RECVRTHDR */
#if (IPV6_RECVRTHDR)
static int SetSocketOption__IPV6_RECVRTHDR__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_RECVRTHDR1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_RECVRTHDR1 = lsocket_opt_set_IPV6_RECVRTHDR(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_RECVRTHDR1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_RECVRTHDR1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_MTU_DISCOVER */
#if (IPV6_MTU_DISCOVER)
static int SetSocketOption__IPV6_MTU_DISCOVER__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_MTU_DISCOVER1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_MTU_DISCOVER1 = lsocket_opt_set_IPV6_MTU_DISCOVER(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_MTU_DISCOVER1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_MTU_DISCOVER1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_UNICAST_HOPS */
#if (IPV6_UNICAST_HOPS)
static int SetSocketOption__IPV6_UNICAST_HOPS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_UNICAST_HOPS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_UNICAST_HOPS1 = lsocket_opt_set_IPV6_UNICAST_HOPS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_UNICAST_HOPS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_UNICAST_HOPS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_NEXTHOP */
#if (IPV6_NEXTHOP)
static int SetSocketOption__IPV6_NEXTHOP__func(lua_State *L) {
  LSocketFD sock1;
  LSockAddr * value2;
  errno_rc rc_lsocket_opt_set_IPV6_NEXTHOP1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = obj_type_LSockAddr_check(L,2);
  rc_lsocket_opt_set_IPV6_NEXTHOP1 = lsocket_opt_set_IPV6_NEXTHOP(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_NEXTHOP1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_NEXTHOP1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_RECVHOPLIMIT */
#if (IPV6_RECVHOPLIMIT)
static int SetSocketOption__IPV6_RECVHOPLIMIT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_RECVHOPLIMIT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_RECVHOPLIMIT1 = lsocket_opt_set_IPV6_RECVHOPLIMIT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_RECVHOPLIMIT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_RECVHOPLIMIT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_MTU */
#if (IPV6_MTU)
static int SetSocketOption__IPV6_MTU__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_MTU1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_MTU1 = lsocket_opt_set_IPV6_MTU(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_MTU1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_MTU1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_HOPLIMIT */
#if (IPV6_HOPLIMIT)
static int SetSocketOption__IPV6_HOPLIMIT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_HOPLIMIT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_HOPLIMIT1 = lsocket_opt_set_IPV6_HOPLIMIT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_HOPLIMIT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_HOPLIMIT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_ADDRFORM */
#if (IPV6_ADDRFORM)
static int SetSocketOption__IPV6_ADDRFORM__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_ADDRFORM1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_ADDRFORM1 = lsocket_opt_set_IPV6_ADDRFORM(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_ADDRFORM1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_ADDRFORM1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_V6ONLY */
#if (IPV6_V6ONLY)
static int SetSocketOption__IPV6_V6ONLY__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_V6ONLY1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_V6ONLY1 = lsocket_opt_set_IPV6_V6ONLY(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_V6ONLY1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_V6ONLY1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_RECVDSTOPTS */
#if (IPV6_RECVDSTOPTS)
static int SetSocketOption__IPV6_RECVDSTOPTS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_RECVDSTOPTS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_RECVDSTOPTS1 = lsocket_opt_set_IPV6_RECVDSTOPTS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_RECVDSTOPTS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_RECVDSTOPTS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_TCLASS */
#if (IPV6_TCLASS)
static int SetSocketOption__IPV6_TCLASS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_TCLASS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_TCLASS1 = lsocket_opt_set_IPV6_TCLASS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_TCLASS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_TCLASS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_RECVTCLASS */
#if (IPV6_RECVTCLASS)
static int SetSocketOption__IPV6_RECVTCLASS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_RECVTCLASS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_RECVTCLASS1 = lsocket_opt_set_IPV6_RECVTCLASS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_RECVTCLASS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_RECVTCLASS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_RECVHOPOPTS */
#if (IPV6_RECVHOPOPTS)
static int SetSocketOption__IPV6_RECVHOPOPTS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_RECVHOPOPTS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_RECVHOPOPTS1 = lsocket_opt_set_IPV6_RECVHOPOPTS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_RECVHOPOPTS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_RECVHOPOPTS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_RECVPKTINFO */
#if (IPV6_RECVPKTINFO)
static int SetSocketOption__IPV6_RECVPKTINFO__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_RECVPKTINFO1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_RECVPKTINFO1 = lsocket_opt_set_IPV6_RECVPKTINFO(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_RECVPKTINFO1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_RECVPKTINFO1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IPV6_ROUTER_ALERT */
#if (IPV6_ROUTER_ALERT)
static int SetSocketOption__IPV6_ROUTER_ALERT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_IPV6_ROUTER_ALERT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_IPV6_ROUTER_ALERT1 = lsocket_opt_set_IPV6_ROUTER_ALERT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_IPV6_ROUTER_ALERT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_IPV6_ROUTER_ALERT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_SNDBUFFORCE */
#if (SO_SNDBUFFORCE)
static int SetSocketOption__SO_SNDBUFFORCE__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_SNDBUFFORCE1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_SNDBUFFORCE1 = lsocket_opt_set_SO_SNDBUFFORCE(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_SNDBUFFORCE1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_SNDBUFFORCE1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_TIMESTAMP */
#if (SO_TIMESTAMP)
static int SetSocketOption__SO_TIMESTAMP__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_TIMESTAMP1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_TIMESTAMP1 = lsocket_opt_set_SO_TIMESTAMP(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_TIMESTAMP1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_TIMESTAMP1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_SNDBUF */
#if (SO_SNDBUF)
static int SetSocketOption__SO_SNDBUF__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_SNDBUF1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_SNDBUF1 = lsocket_opt_set_SO_SNDBUF(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_SNDBUF1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_SNDBUF1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_NO_CHECK */
#if (SO_NO_CHECK)
static int SetSocketOption__SO_NO_CHECK__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_NO_CHECK1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_NO_CHECK1 = lsocket_opt_set_SO_NO_CHECK(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_NO_CHECK1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_NO_CHECK1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_BSDCOMPAT */
#if (SO_BSDCOMPAT)
static int SetSocketOption__SO_BSDCOMPAT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_BSDCOMPAT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_BSDCOMPAT1 = lsocket_opt_set_SO_BSDCOMPAT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_BSDCOMPAT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_BSDCOMPAT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_REUSEADDR */
#if (SO_REUSEADDR)
static int SetSocketOption__SO_REUSEADDR__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_REUSEADDR1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_REUSEADDR1 = lsocket_opt_set_SO_REUSEADDR(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_REUSEADDR1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_REUSEADDR1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_MARK */
#if (SO_MARK)
static int SetSocketOption__SO_MARK__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_MARK1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_MARK1 = lsocket_opt_set_SO_MARK(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_MARK1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_MARK1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_TIMESTAMPING */
#if (SO_TIMESTAMPING)
static int SetSocketOption__SO_TIMESTAMPING__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_TIMESTAMPING1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_TIMESTAMPING1 = lsocket_opt_set_SO_TIMESTAMPING(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_TIMESTAMPING1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_TIMESTAMPING1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_TIMESTAMPNS */
#if (SO_TIMESTAMPNS)
static int SetSocketOption__SO_TIMESTAMPNS__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_TIMESTAMPNS1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_TIMESTAMPNS1 = lsocket_opt_set_SO_TIMESTAMPNS(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_TIMESTAMPNS1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_TIMESTAMPNS1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_BINDTODEVICE */
#if (SO_BINDTODEVICE)
static int SetSocketOption__SO_BINDTODEVICE__func(lua_State *L) {
  LSocketFD sock1;
  size_t value_len2;
  const char * value2;
  errno_rc rc_lsocket_opt_set_SO_BINDTODEVICE1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lsocket_opt_set_SO_BINDTODEVICE1 = lsocket_opt_set_SO_BINDTODEVICE(sock1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_BINDTODEVICE1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_BINDTODEVICE1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_DONTROUTE */
#if (SO_DONTROUTE)
static int SetSocketOption__SO_DONTROUTE__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_DONTROUTE1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_DONTROUTE1 = lsocket_opt_set_SO_DONTROUTE(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_DONTROUTE1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_DONTROUTE1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_RCVBUFFORCE */
#if (SO_RCVBUFFORCE)
static int SetSocketOption__SO_RCVBUFFORCE__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_RCVBUFFORCE1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_RCVBUFFORCE1 = lsocket_opt_set_SO_RCVBUFFORCE(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_RCVBUFFORCE1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_RCVBUFFORCE1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_SNDLOWAT */
#if (SO_SNDLOWAT)
static int SetSocketOption__SO_SNDLOWAT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_SNDLOWAT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_SNDLOWAT1 = lsocket_opt_set_SO_SNDLOWAT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_SNDLOWAT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_SNDLOWAT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_OOBINLINE */
#if (SO_OOBINLINE)
static int SetSocketOption__SO_OOBINLINE__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_OOBINLINE1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_OOBINLINE1 = lsocket_opt_set_SO_OOBINLINE(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_OOBINLINE1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_OOBINLINE1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_RCVLOWAT */
#if (SO_RCVLOWAT)
static int SetSocketOption__SO_RCVLOWAT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_RCVLOWAT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_RCVLOWAT1 = lsocket_opt_set_SO_RCVLOWAT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_RCVLOWAT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_RCVLOWAT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_PASSCRED */
#if (SO_PASSCRED)
static int SetSocketOption__SO_PASSCRED__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_PASSCRED1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_PASSCRED1 = lsocket_opt_set_SO_PASSCRED(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_PASSCRED1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_PASSCRED1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_RCVBUF */
#if (SO_RCVBUF)
static int SetSocketOption__SO_RCVBUF__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_RCVBUF1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_RCVBUF1 = lsocket_opt_set_SO_RCVBUF(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_RCVBUF1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_RCVBUF1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_PRIORITY */
#if (SO_PRIORITY)
static int SetSocketOption__SO_PRIORITY__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_PRIORITY1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_PRIORITY1 = lsocket_opt_set_SO_PRIORITY(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_PRIORITY1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_PRIORITY1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_BROADCAST */
#if (SO_BROADCAST)
static int SetSocketOption__SO_BROADCAST__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_BROADCAST1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_BROADCAST1 = lsocket_opt_set_SO_BROADCAST(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_BROADCAST1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_BROADCAST1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_KEEPALIVE */
#if (SO_KEEPALIVE)
static int SetSocketOption__SO_KEEPALIVE__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_KEEPALIVE1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_KEEPALIVE1 = lsocket_opt_set_SO_KEEPALIVE(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_KEEPALIVE1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_KEEPALIVE1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: SO_DEBUG */
#if (SO_DEBUG)
static int SetSocketOption__SO_DEBUG__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_SO_DEBUG1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_SO_DEBUG1 = lsocket_opt_set_SO_DEBUG(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_SO_DEBUG1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_SO_DEBUG1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_CORK */
#if (TCP_CORK)
static int SetSocketOption__TCP_CORK__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_CORK1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_CORK1 = lsocket_opt_set_TCP_CORK(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_CORK1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_CORK1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_KEEPIDLE */
#if (TCP_KEEPIDLE)
static int SetSocketOption__TCP_KEEPIDLE__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_KEEPIDLE1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_KEEPIDLE1 = lsocket_opt_set_TCP_KEEPIDLE(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_KEEPIDLE1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_KEEPIDLE1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_QUICKACK */
#if (TCP_QUICKACK)
static int SetSocketOption__TCP_QUICKACK__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_QUICKACK1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_QUICKACK1 = lsocket_opt_set_TCP_QUICKACK(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_QUICKACK1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_QUICKACK1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_CONGESTION */
#if (TCP_CONGESTION)
static int SetSocketOption__TCP_CONGESTION__func(lua_State *L) {
  LSocketFD sock1;
  size_t value_len2;
  const char * value2;
  errno_rc rc_lsocket_opt_set_TCP_CONGESTION1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lsocket_opt_set_TCP_CONGESTION1 = lsocket_opt_set_TCP_CONGESTION(sock1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_CONGESTION1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_CONGESTION1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_WINDOW_CLAMP */
#if (TCP_WINDOW_CLAMP)
static int SetSocketOption__TCP_WINDOW_CLAMP__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_WINDOW_CLAMP1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_WINDOW_CLAMP1 = lsocket_opt_set_TCP_WINDOW_CLAMP(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_WINDOW_CLAMP1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_WINDOW_CLAMP1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_DEFER_ACCEPT */
#if (TCP_DEFER_ACCEPT)
static int SetSocketOption__TCP_DEFER_ACCEPT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_DEFER_ACCEPT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_DEFER_ACCEPT1 = lsocket_opt_set_TCP_DEFER_ACCEPT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_DEFER_ACCEPT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_DEFER_ACCEPT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_MAXSEG */
#if (TCP_MAXSEG)
static int SetSocketOption__TCP_MAXSEG__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_MAXSEG1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_MAXSEG1 = lsocket_opt_set_TCP_MAXSEG(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_MAXSEG1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_MAXSEG1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_NODELAY */
#if (TCP_NODELAY)
static int SetSocketOption__TCP_NODELAY__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_NODELAY1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_NODELAY1 = lsocket_opt_set_TCP_NODELAY(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_NODELAY1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_NODELAY1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_KEEPCNT */
#if (TCP_KEEPCNT)
static int SetSocketOption__TCP_KEEPCNT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_KEEPCNT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_KEEPCNT1 = lsocket_opt_set_TCP_KEEPCNT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_KEEPCNT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_KEEPCNT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_LINGER2 */
#if (TCP_LINGER2)
static int SetSocketOption__TCP_LINGER2__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_LINGER21 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_LINGER21 = lsocket_opt_set_TCP_LINGER2(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_LINGER21)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_LINGER21);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_SYNCNT */
#if (TCP_SYNCNT)
static int SetSocketOption__TCP_SYNCNT__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_SYNCNT1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_SYNCNT1 = lsocket_opt_set_TCP_SYNCNT(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_SYNCNT1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_SYNCNT1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: TCP_KEEPINTVL */
#if (TCP_KEEPINTVL)
static int SetSocketOption__TCP_KEEPINTVL__func(lua_State *L) {
  LSocketFD sock1;
  int value2;
  errno_rc rc_lsocket_opt_set_TCP_KEEPINTVL1 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lsocket_opt_set_TCP_KEEPINTVL1 = lsocket_opt_set_TCP_KEEPINTVL(sock1, value2);
  /* check for error. */
  if((-1 == rc_lsocket_opt_set_TCP_KEEPINTVL1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_lsocket_opt_set_TCP_KEEPINTVL1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: IP_RECVOPTS */
#if (IP_RECVOPTS)
static int GetSocketOption__IP_RECVOPTS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_RECVOPTS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_RECVOPTS2 = lsocket_opt_get_IP_RECVOPTS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_RECVOPTS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_RECVOPTS2);
  return 2;
}
#endif

/* method: IP_MTU */
#if (IP_MTU)
static int GetSocketOption__IP_MTU__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_MTU2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_MTU2 = lsocket_opt_get_IP_MTU(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_MTU2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_MTU2);
  return 2;
}
#endif

/* method: IP_TOS */
#if (IP_TOS)
static int GetSocketOption__IP_TOS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_TOS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_TOS2 = lsocket_opt_get_IP_TOS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_TOS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_TOS2);
  return 2;
}
#endif

/* method: IP_OPTIONS */
#if (IP_OPTIONS)
static int GetSocketOption__IP_OPTIONS__func(lua_State *L) {
  LSocketFD sock1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  errno_rc rc_lsocket_opt_get_IP_OPTIONS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_OPTIONS2 = lsocket_opt_get_IP_OPTIONS(sock1, value1, &(value_len1));
  if(!(-1 == rc_lsocket_opt_get_IP_OPTIONS2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_OPTIONS2);
  return 2;
}
#endif

/* method: IP_TTL */
#if (IP_TTL)
static int GetSocketOption__IP_TTL__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_TTL2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_TTL2 = lsocket_opt_get_IP_TTL(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_TTL2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_TTL2);
  return 2;
}
#endif

/* method: IP_RECVTOS */
#if (IP_RECVTOS)
static int GetSocketOption__IP_RECVTOS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_RECVTOS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_RECVTOS2 = lsocket_opt_get_IP_RECVTOS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_RECVTOS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_RECVTOS2);
  return 2;
}
#endif

/* method: IP_MINTTL */
#if (IP_MINTTL)
static int GetSocketOption__IP_MINTTL__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_MINTTL2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_MINTTL2 = lsocket_opt_get_IP_MINTTL(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_MINTTL2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_MINTTL2);
  return 2;
}
#endif

/* method: IP_MULTICAST_TTL */
#if (IP_MULTICAST_TTL)
static int GetSocketOption__IP_MULTICAST_TTL__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_MULTICAST_TTL2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_MULTICAST_TTL2 = lsocket_opt_get_IP_MULTICAST_TTL(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_MULTICAST_TTL2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_MULTICAST_TTL2);
  return 2;
}
#endif

/* method: IP_RECVTTL */
#if (IP_RECVTTL)
static int GetSocketOption__IP_RECVTTL__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_RECVTTL2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_RECVTTL2 = lsocket_opt_get_IP_RECVTTL(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_RECVTTL2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_RECVTTL2);
  return 2;
}
#endif

/* method: IP_FREEBIND */
#if (IP_FREEBIND)
static int GetSocketOption__IP_FREEBIND__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_FREEBIND2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_FREEBIND2 = lsocket_opt_get_IP_FREEBIND(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_FREEBIND2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_FREEBIND2);
  return 2;
}
#endif

/* method: IP_PKTINFO */
#if (IP_PKTINFO)
static int GetSocketOption__IP_PKTINFO__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_PKTINFO2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_PKTINFO2 = lsocket_opt_get_IP_PKTINFO(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_PKTINFO2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_PKTINFO2);
  return 2;
}
#endif

/* method: IP_HDRINCL */
#if (IP_HDRINCL)
static int GetSocketOption__IP_HDRINCL__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_HDRINCL2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_HDRINCL2 = lsocket_opt_get_IP_HDRINCL(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_HDRINCL2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_HDRINCL2);
  return 2;
}
#endif

/* method: IP_MTU_DISCOVER */
#if (IP_MTU_DISCOVER)
static int GetSocketOption__IP_MTU_DISCOVER__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_MTU_DISCOVER2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_MTU_DISCOVER2 = lsocket_opt_get_IP_MTU_DISCOVER(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_MTU_DISCOVER2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_MTU_DISCOVER2);
  return 2;
}
#endif

/* method: IP_RETOPTS */
#if (IP_RETOPTS)
static int GetSocketOption__IP_RETOPTS__func(lua_State *L) {
  LSocketFD sock1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  errno_rc rc_lsocket_opt_get_IP_RETOPTS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_RETOPTS2 = lsocket_opt_get_IP_RETOPTS(sock1, value1, &(value_len1));
  if(!(-1 == rc_lsocket_opt_get_IP_RETOPTS2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_RETOPTS2);
  return 2;
}
#endif

/* method: IP_RECVERR */
#if (IP_RECVERR)
static int GetSocketOption__IP_RECVERR__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_RECVERR2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_RECVERR2 = lsocket_opt_get_IP_RECVERR(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_RECVERR2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_RECVERR2);
  return 2;
}
#endif

/* method: IP_ROUTER_ALERT */
#if (IP_ROUTER_ALERT)
static int GetSocketOption__IP_ROUTER_ALERT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_ROUTER_ALERT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_ROUTER_ALERT2 = lsocket_opt_get_IP_ROUTER_ALERT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_ROUTER_ALERT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_ROUTER_ALERT2);
  return 2;
}
#endif

/* method: IP_RECVRETOPTS */
#if (IP_RECVRETOPTS)
static int GetSocketOption__IP_RECVRETOPTS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_RECVRETOPTS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_RECVRETOPTS2 = lsocket_opt_get_IP_RECVRETOPTS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_RECVRETOPTS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_RECVRETOPTS2);
  return 2;
}
#endif

/* method: IP_RECVORIGDSTADDR */
#if (IP_RECVORIGDSTADDR)
static int GetSocketOption__IP_RECVORIGDSTADDR__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_RECVORIGDSTADDR2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_RECVORIGDSTADDR2 = lsocket_opt_get_IP_RECVORIGDSTADDR(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_RECVORIGDSTADDR2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_RECVORIGDSTADDR2);
  return 2;
}
#endif

/* method: IP_MULTICAST_LOOP */
#if (IP_MULTICAST_LOOP)
static int GetSocketOption__IP_MULTICAST_LOOP__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IP_MULTICAST_LOOP2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IP_MULTICAST_LOOP2 = lsocket_opt_get_IP_MULTICAST_LOOP(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IP_MULTICAST_LOOP2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IP_MULTICAST_LOOP2);
  return 2;
}
#endif

/* method: IPV6_MULTICAST_HOPS */
#if (IPV6_MULTICAST_HOPS)
static int GetSocketOption__IPV6_MULTICAST_HOPS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_MULTICAST_HOPS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_MULTICAST_HOPS2 = lsocket_opt_get_IPV6_MULTICAST_HOPS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_MULTICAST_HOPS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_MULTICAST_HOPS2);
  return 2;
}
#endif

/* method: IPV6_RECVERR */
#if (IPV6_RECVERR)
static int GetSocketOption__IPV6_RECVERR__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_RECVERR2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_RECVERR2 = lsocket_opt_get_IPV6_RECVERR(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_RECVERR2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_RECVERR2);
  return 2;
}
#endif

/* method: IPV6_CHECKSUM */
#if (IPV6_CHECKSUM)
static int GetSocketOption__IPV6_CHECKSUM__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_CHECKSUM2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_CHECKSUM2 = lsocket_opt_get_IPV6_CHECKSUM(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_CHECKSUM2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_CHECKSUM2);
  return 2;
}
#endif

/* method: IPV6_MULTICAST_LOOP */
#if (IPV6_MULTICAST_LOOP)
static int GetSocketOption__IPV6_MULTICAST_LOOP__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_MULTICAST_LOOP2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_MULTICAST_LOOP2 = lsocket_opt_get_IPV6_MULTICAST_LOOP(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_MULTICAST_LOOP2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_MULTICAST_LOOP2);
  return 2;
}
#endif

/* method: IPV6_MULTICAST_IF */
#if (IPV6_MULTICAST_IF)
static int GetSocketOption__IPV6_MULTICAST_IF__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_MULTICAST_IF2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_MULTICAST_IF2 = lsocket_opt_get_IPV6_MULTICAST_IF(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_MULTICAST_IF2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_MULTICAST_IF2);
  return 2;
}
#endif

/* method: IPV6_RECVRTHDR */
#if (IPV6_RECVRTHDR)
static int GetSocketOption__IPV6_RECVRTHDR__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_RECVRTHDR2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_RECVRTHDR2 = lsocket_opt_get_IPV6_RECVRTHDR(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_RECVRTHDR2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_RECVRTHDR2);
  return 2;
}
#endif

/* method: IPV6_MTU_DISCOVER */
#if (IPV6_MTU_DISCOVER)
static int GetSocketOption__IPV6_MTU_DISCOVER__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_MTU_DISCOVER2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_MTU_DISCOVER2 = lsocket_opt_get_IPV6_MTU_DISCOVER(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_MTU_DISCOVER2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_MTU_DISCOVER2);
  return 2;
}
#endif

/* method: IPV6_UNICAST_HOPS */
#if (IPV6_UNICAST_HOPS)
static int GetSocketOption__IPV6_UNICAST_HOPS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_UNICAST_HOPS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_UNICAST_HOPS2 = lsocket_opt_get_IPV6_UNICAST_HOPS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_UNICAST_HOPS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_UNICAST_HOPS2);
  return 2;
}
#endif

/* method: IPV6_NEXTHOP */
#if (IPV6_NEXTHOP)
static int GetSocketOption__IPV6_NEXTHOP__func(lua_State *L) {
  LSocketFD sock1;
  LSockAddr value1_store;
  LSockAddr * value1 = &(value1_store);
  errno_rc rc_lsocket_opt_get_IPV6_NEXTHOP2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_NEXTHOP2 = lsocket_opt_get_IPV6_NEXTHOP(sock1, value1);
  if(!(-1 == rc_lsocket_opt_get_IPV6_NEXTHOP2)) {
    obj_type_LSockAddr_push(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_NEXTHOP2);
  return 2;
}
#endif

/* method: IPV6_RECVHOPLIMIT */
#if (IPV6_RECVHOPLIMIT)
static int GetSocketOption__IPV6_RECVHOPLIMIT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_RECVHOPLIMIT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_RECVHOPLIMIT2 = lsocket_opt_get_IPV6_RECVHOPLIMIT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_RECVHOPLIMIT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_RECVHOPLIMIT2);
  return 2;
}
#endif

/* method: IPV6_MTU */
#if (IPV6_MTU)
static int GetSocketOption__IPV6_MTU__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_MTU2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_MTU2 = lsocket_opt_get_IPV6_MTU(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_MTU2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_MTU2);
  return 2;
}
#endif

/* method: IPV6_HOPLIMIT */
#if (IPV6_HOPLIMIT)
static int GetSocketOption__IPV6_HOPLIMIT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_HOPLIMIT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_HOPLIMIT2 = lsocket_opt_get_IPV6_HOPLIMIT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_HOPLIMIT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_HOPLIMIT2);
  return 2;
}
#endif

/* method: IPV6_V6ONLY */
#if (IPV6_V6ONLY)
static int GetSocketOption__IPV6_V6ONLY__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_V6ONLY2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_V6ONLY2 = lsocket_opt_get_IPV6_V6ONLY(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_V6ONLY2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_V6ONLY2);
  return 2;
}
#endif

/* method: IPV6_RECVDSTOPTS */
#if (IPV6_RECVDSTOPTS)
static int GetSocketOption__IPV6_RECVDSTOPTS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_RECVDSTOPTS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_RECVDSTOPTS2 = lsocket_opt_get_IPV6_RECVDSTOPTS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_RECVDSTOPTS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_RECVDSTOPTS2);
  return 2;
}
#endif

/* method: IPV6_TCLASS */
#if (IPV6_TCLASS)
static int GetSocketOption__IPV6_TCLASS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_TCLASS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_TCLASS2 = lsocket_opt_get_IPV6_TCLASS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_TCLASS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_TCLASS2);
  return 2;
}
#endif

/* method: IPV6_RECVTCLASS */
#if (IPV6_RECVTCLASS)
static int GetSocketOption__IPV6_RECVTCLASS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_RECVTCLASS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_RECVTCLASS2 = lsocket_opt_get_IPV6_RECVTCLASS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_RECVTCLASS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_RECVTCLASS2);
  return 2;
}
#endif

/* method: IPV6_RECVHOPOPTS */
#if (IPV6_RECVHOPOPTS)
static int GetSocketOption__IPV6_RECVHOPOPTS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_RECVHOPOPTS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_RECVHOPOPTS2 = lsocket_opt_get_IPV6_RECVHOPOPTS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_RECVHOPOPTS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_RECVHOPOPTS2);
  return 2;
}
#endif

/* method: IPV6_RECVPKTINFO */
#if (IPV6_RECVPKTINFO)
static int GetSocketOption__IPV6_RECVPKTINFO__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_RECVPKTINFO2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_RECVPKTINFO2 = lsocket_opt_get_IPV6_RECVPKTINFO(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_RECVPKTINFO2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_RECVPKTINFO2);
  return 2;
}
#endif

/* method: IPV6_ROUTER_ALERT */
#if (IPV6_ROUTER_ALERT)
static int GetSocketOption__IPV6_ROUTER_ALERT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_IPV6_ROUTER_ALERT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_IPV6_ROUTER_ALERT2 = lsocket_opt_get_IPV6_ROUTER_ALERT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_IPV6_ROUTER_ALERT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_IPV6_ROUTER_ALERT2);
  return 2;
}
#endif

/* method: SO_SNDBUFFORCE */
#if (SO_SNDBUFFORCE)
static int GetSocketOption__SO_SNDBUFFORCE__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_SNDBUFFORCE2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_SNDBUFFORCE2 = lsocket_opt_get_SO_SNDBUFFORCE(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_SNDBUFFORCE2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_SNDBUFFORCE2);
  return 2;
}
#endif

/* method: SO_ACCEPTCONN */
#if (SO_ACCEPTCONN)
static int GetSocketOption__SO_ACCEPTCONN__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_ACCEPTCONN2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_ACCEPTCONN2 = lsocket_opt_get_SO_ACCEPTCONN(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_ACCEPTCONN2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_ACCEPTCONN2);
  return 2;
}
#endif

/* method: SO_PROTOCOL */
#if (SO_PROTOCOL)
static int GetSocketOption__SO_PROTOCOL__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_PROTOCOL2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_PROTOCOL2 = lsocket_opt_get_SO_PROTOCOL(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_PROTOCOL2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_PROTOCOL2);
  return 2;
}
#endif

/* method: SO_TIMESTAMP */
#if (SO_TIMESTAMP)
static int GetSocketOption__SO_TIMESTAMP__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_TIMESTAMP2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_TIMESTAMP2 = lsocket_opt_get_SO_TIMESTAMP(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_TIMESTAMP2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_TIMESTAMP2);
  return 2;
}
#endif

/* method: SO_SNDBUF */
#if (SO_SNDBUF)
static int GetSocketOption__SO_SNDBUF__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_SNDBUF2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_SNDBUF2 = lsocket_opt_get_SO_SNDBUF(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_SNDBUF2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_SNDBUF2);
  return 2;
}
#endif

/* method: SO_NO_CHECK */
#if (SO_NO_CHECK)
static int GetSocketOption__SO_NO_CHECK__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_NO_CHECK2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_NO_CHECK2 = lsocket_opt_get_SO_NO_CHECK(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_NO_CHECK2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_NO_CHECK2);
  return 2;
}
#endif

/* method: SO_BSDCOMPAT */
#if (SO_BSDCOMPAT)
static int GetSocketOption__SO_BSDCOMPAT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_BSDCOMPAT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_BSDCOMPAT2 = lsocket_opt_get_SO_BSDCOMPAT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_BSDCOMPAT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_BSDCOMPAT2);
  return 2;
}
#endif

/* method: SO_REUSEADDR */
#if (SO_REUSEADDR)
static int GetSocketOption__SO_REUSEADDR__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_REUSEADDR2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_REUSEADDR2 = lsocket_opt_get_SO_REUSEADDR(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_REUSEADDR2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_REUSEADDR2);
  return 2;
}
#endif

/* method: SO_MARK */
#if (SO_MARK)
static int GetSocketOption__SO_MARK__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_MARK2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_MARK2 = lsocket_opt_get_SO_MARK(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_MARK2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_MARK2);
  return 2;
}
#endif

/* method: SO_TIMESTAMPING */
#if (SO_TIMESTAMPING)
static int GetSocketOption__SO_TIMESTAMPING__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_TIMESTAMPING2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_TIMESTAMPING2 = lsocket_opt_get_SO_TIMESTAMPING(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_TIMESTAMPING2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_TIMESTAMPING2);
  return 2;
}
#endif

/* method: SO_TIMESTAMPNS */
#if (SO_TIMESTAMPNS)
static int GetSocketOption__SO_TIMESTAMPNS__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_TIMESTAMPNS2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_TIMESTAMPNS2 = lsocket_opt_get_SO_TIMESTAMPNS(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_TIMESTAMPNS2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_TIMESTAMPNS2);
  return 2;
}
#endif

/* method: SO_BINDTODEVICE */
#if (SO_BINDTODEVICE)
static int GetSocketOption__SO_BINDTODEVICE__func(lua_State *L) {
  LSocketFD sock1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  errno_rc rc_lsocket_opt_get_SO_BINDTODEVICE2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_BINDTODEVICE2 = lsocket_opt_get_SO_BINDTODEVICE(sock1, value1, &(value_len1));
  if(!(-1 == rc_lsocket_opt_get_SO_BINDTODEVICE2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_BINDTODEVICE2);
  return 2;
}
#endif

/* method: SO_DONTROUTE */
#if (SO_DONTROUTE)
static int GetSocketOption__SO_DONTROUTE__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_DONTROUTE2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_DONTROUTE2 = lsocket_opt_get_SO_DONTROUTE(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_DONTROUTE2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_DONTROUTE2);
  return 2;
}
#endif

/* method: SO_RCVBUFFORCE */
#if (SO_RCVBUFFORCE)
static int GetSocketOption__SO_RCVBUFFORCE__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_RCVBUFFORCE2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_RCVBUFFORCE2 = lsocket_opt_get_SO_RCVBUFFORCE(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_RCVBUFFORCE2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_RCVBUFFORCE2);
  return 2;
}
#endif

/* method: SO_SNDLOWAT */
#if (SO_SNDLOWAT)
static int GetSocketOption__SO_SNDLOWAT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_SNDLOWAT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_SNDLOWAT2 = lsocket_opt_get_SO_SNDLOWAT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_SNDLOWAT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_SNDLOWAT2);
  return 2;
}
#endif

/* method: SO_OOBINLINE */
#if (SO_OOBINLINE)
static int GetSocketOption__SO_OOBINLINE__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_OOBINLINE2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_OOBINLINE2 = lsocket_opt_get_SO_OOBINLINE(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_OOBINLINE2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_OOBINLINE2);
  return 2;
}
#endif

/* method: SO_DOMAIN */
#if (SO_DOMAIN)
static int GetSocketOption__SO_DOMAIN__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_DOMAIN2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_DOMAIN2 = lsocket_opt_get_SO_DOMAIN(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_DOMAIN2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_DOMAIN2);
  return 2;
}
#endif

/* method: SO_RCVLOWAT */
#if (SO_RCVLOWAT)
static int GetSocketOption__SO_RCVLOWAT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_RCVLOWAT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_RCVLOWAT2 = lsocket_opt_get_SO_RCVLOWAT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_RCVLOWAT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_RCVLOWAT2);
  return 2;
}
#endif

/* method: SO_PASSCRED */
#if (SO_PASSCRED)
static int GetSocketOption__SO_PASSCRED__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_PASSCRED2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_PASSCRED2 = lsocket_opt_get_SO_PASSCRED(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_PASSCRED2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_PASSCRED2);
  return 2;
}
#endif

/* method: SO_RCVBUF */
#if (SO_RCVBUF)
static int GetSocketOption__SO_RCVBUF__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_RCVBUF2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_RCVBUF2 = lsocket_opt_get_SO_RCVBUF(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_RCVBUF2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_RCVBUF2);
  return 2;
}
#endif

/* method: SO_PRIORITY */
#if (SO_PRIORITY)
static int GetSocketOption__SO_PRIORITY__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_PRIORITY2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_PRIORITY2 = lsocket_opt_get_SO_PRIORITY(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_PRIORITY2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_PRIORITY2);
  return 2;
}
#endif

/* method: SO_TYPE */
#if (SO_TYPE)
static int GetSocketOption__SO_TYPE__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_TYPE2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_TYPE2 = lsocket_opt_get_SO_TYPE(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_TYPE2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_TYPE2);
  return 2;
}
#endif

/* method: SO_BROADCAST */
#if (SO_BROADCAST)
static int GetSocketOption__SO_BROADCAST__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_BROADCAST2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_BROADCAST2 = lsocket_opt_get_SO_BROADCAST(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_BROADCAST2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_BROADCAST2);
  return 2;
}
#endif

/* method: SO_KEEPALIVE */
#if (SO_KEEPALIVE)
static int GetSocketOption__SO_KEEPALIVE__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_KEEPALIVE2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_KEEPALIVE2 = lsocket_opt_get_SO_KEEPALIVE(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_KEEPALIVE2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_KEEPALIVE2);
  return 2;
}
#endif

/* method: SO_DEBUG */
#if (SO_DEBUG)
static int GetSocketOption__SO_DEBUG__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_DEBUG2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_DEBUG2 = lsocket_opt_get_SO_DEBUG(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_DEBUG2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_DEBUG2);
  return 2;
}
#endif

/* method: SO_ERROR */
#if (SO_ERROR)
static int GetSocketOption__SO_ERROR__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_SO_ERROR2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_SO_ERROR2 = lsocket_opt_get_SO_ERROR(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_SO_ERROR2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_SO_ERROR2);
  return 2;
}
#endif

/* method: TCP_CORK */
#if (TCP_CORK)
static int GetSocketOption__TCP_CORK__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_CORK2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_CORK2 = lsocket_opt_get_TCP_CORK(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_CORK2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_CORK2);
  return 2;
}
#endif

/* method: TCP_KEEPIDLE */
#if (TCP_KEEPIDLE)
static int GetSocketOption__TCP_KEEPIDLE__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_KEEPIDLE2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_KEEPIDLE2 = lsocket_opt_get_TCP_KEEPIDLE(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_KEEPIDLE2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_KEEPIDLE2);
  return 2;
}
#endif

/* method: TCP_QUICKACK */
#if (TCP_QUICKACK)
static int GetSocketOption__TCP_QUICKACK__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_QUICKACK2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_QUICKACK2 = lsocket_opt_get_TCP_QUICKACK(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_QUICKACK2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_QUICKACK2);
  return 2;
}
#endif

/* method: TCP_CONGESTION */
#if (TCP_CONGESTION)
static int GetSocketOption__TCP_CONGESTION__func(lua_State *L) {
  LSocketFD sock1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  errno_rc rc_lsocket_opt_get_TCP_CONGESTION2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_CONGESTION2 = lsocket_opt_get_TCP_CONGESTION(sock1, value1, &(value_len1));
  if(!(-1 == rc_lsocket_opt_get_TCP_CONGESTION2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_CONGESTION2);
  return 2;
}
#endif

/* method: TCP_WINDOW_CLAMP */
#if (TCP_WINDOW_CLAMP)
static int GetSocketOption__TCP_WINDOW_CLAMP__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_WINDOW_CLAMP2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_WINDOW_CLAMP2 = lsocket_opt_get_TCP_WINDOW_CLAMP(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_WINDOW_CLAMP2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_WINDOW_CLAMP2);
  return 2;
}
#endif

/* method: TCP_DEFER_ACCEPT */
#if (TCP_DEFER_ACCEPT)
static int GetSocketOption__TCP_DEFER_ACCEPT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_DEFER_ACCEPT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_DEFER_ACCEPT2 = lsocket_opt_get_TCP_DEFER_ACCEPT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_DEFER_ACCEPT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_DEFER_ACCEPT2);
  return 2;
}
#endif

/* method: TCP_MAXSEG */
#if (TCP_MAXSEG)
static int GetSocketOption__TCP_MAXSEG__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_MAXSEG2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_MAXSEG2 = lsocket_opt_get_TCP_MAXSEG(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_MAXSEG2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_MAXSEG2);
  return 2;
}
#endif

/* method: TCP_NODELAY */
#if (TCP_NODELAY)
static int GetSocketOption__TCP_NODELAY__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_NODELAY2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_NODELAY2 = lsocket_opt_get_TCP_NODELAY(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_NODELAY2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_NODELAY2);
  return 2;
}
#endif

/* method: TCP_KEEPCNT */
#if (TCP_KEEPCNT)
static int GetSocketOption__TCP_KEEPCNT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_KEEPCNT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_KEEPCNT2 = lsocket_opt_get_TCP_KEEPCNT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_KEEPCNT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_KEEPCNT2);
  return 2;
}
#endif

/* method: TCP_LINGER2 */
#if (TCP_LINGER2)
static int GetSocketOption__TCP_LINGER2__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_LINGER22 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_LINGER22 = lsocket_opt_get_TCP_LINGER2(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_LINGER22)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_LINGER22);
  return 2;
}
#endif

/* method: TCP_SYNCNT */
#if (TCP_SYNCNT)
static int GetSocketOption__TCP_SYNCNT__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_SYNCNT2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_SYNCNT2 = lsocket_opt_get_TCP_SYNCNT(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_SYNCNT2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_SYNCNT2);
  return 2;
}
#endif

/* method: TCP_KEEPINTVL */
#if (TCP_KEEPINTVL)
static int GetSocketOption__TCP_KEEPINTVL__func(lua_State *L) {
  LSocketFD sock1;
  int value1 = 0;
  errno_rc rc_lsocket_opt_get_TCP_KEEPINTVL2 = 0;
  sock1 = obj_type_LSocketFD_check(L,1);
  rc_lsocket_opt_get_TCP_KEEPINTVL2 = lsocket_opt_get_TCP_KEEPINTVL(sock1, &(value1));
  if(!(-1 == rc_lsocket_opt_get_TCP_KEEPINTVL2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_lsocket_opt_get_TCP_KEEPINTVL2);
  return 2;
}
#endif

/* method: new */
static int LSocketFD__new__meth(lua_State *L) {
  int domain1;
  int type2;
  int protocol3;
  int flags4;
  LSocketFD this1;
  domain1 = luaL_checkinteger(L,1);
  type2 = luaL_checkinteger(L,2);
  protocol3 = luaL_optinteger(L,3,0);
  flags4 = luaL_optinteger(L,4,0);
  this1 = l_socket_open(domain1, type2, protocol3, flags4);
  obj_type_LSocketFD_push(L, this1);
  return 1;
}

/* method: fd */
static int LSocketFD__fd__meth(lua_State *L) {
  int fd1;
  LSocketFD this1;
  fd1 = luaL_checkinteger(L,1);
  this1 = fd1;
  obj_type_LSocketFD_push(L, this1);
  return 1;
}

/* method: close */
static int LSocketFD__close__meth(lua_State *L) {
  LSocketFD this1;
  this1 = obj_type_LSocketFD_delete(L,1);
  l_socket_close_internal(this1);
  return 0;
}

/* method: __tostring */
static int LSocketFD____tostring__meth(lua_State *L) {
  LSocketFD this1;
  const char * str1 = NULL;
	char tmp[64];

  this1 = obj_type_LSocketFD_check(L,1);
	str1 = tmp;
	snprintf(tmp, 64, "LSocketFD: fd=%d", this1);

  lua_pushstring(L, str1);
  return 1;
}

/* method: shutdown */
static int LSocketFD__shutdown__meth(lua_State *L) {
  LSocketFD this1;
  int how2;
  int rc_l_socket_shutdown1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  how2 = luaL_checkinteger(L,2);
  rc_l_socket_shutdown1 = l_socket_shutdown(this1, how2);
  lua_pushinteger(L, rc_l_socket_shutdown1);
  return 1;
}

/* method: fileno */
static int LSocketFD__fileno__meth(lua_State *L) {
  LSocketFD this1;
  int fd1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  fd1 = this1;
  lua_pushinteger(L, fd1);
  return 1;
}

/* method: set_nonblock */
static int LSocketFD__set_nonblock__meth(lua_State *L) {
  LSocketFD this1;
  bool nonblock2;
  errno_rc rc_l_socket_set_nonblock1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  nonblock2 = lua_toboolean(L,2);
  rc_l_socket_set_nonblock1 = l_socket_set_nonblock(this1, nonblock2);
  /* check for error. */
  if((-1 == rc_l_socket_set_nonblock1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_l_socket_set_nonblock1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: connect */
static int LSocketFD__connect__meth(lua_State *L) {
  LSocketFD this1;
  LSockAddr * addr2;
  errno_rc rc_l_socket_connect1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  addr2 = obj_type_LSockAddr_check(L,2);
  rc_l_socket_connect1 = l_socket_connect(this1, addr2);
  /* check for error. */
  if((-1 == rc_l_socket_connect1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_l_socket_connect1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: bind */
static int LSocketFD__bind__meth(lua_State *L) {
  LSocketFD this1;
  LSockAddr * addr2;
  errno_rc rc_l_socket_bind1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  addr2 = obj_type_LSockAddr_check(L,2);
  rc_l_socket_bind1 = l_socket_bind(this1, addr2);
  /* check for error. */
  if((-1 == rc_l_socket_bind1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_l_socket_bind1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: listen */
static int LSocketFD__listen__meth(lua_State *L) {
  LSocketFD this1;
  int backlog2;
  errno_rc rc_l_socket_listen1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  backlog2 = luaL_checkinteger(L,2);
  rc_l_socket_listen1 = l_socket_listen(this1, backlog2);
  /* check for error. */
  if((-1 == rc_l_socket_listen1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_l_socket_listen1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_sockname */
static int LSocketFD__get_sockname__meth(lua_State *L) {
  LSocketFD this1;
  LSockAddr * addr2;
  errno_rc rc_l_socket_get_sockname1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  addr2 = obj_type_LSockAddr_check(L,2);
  rc_l_socket_get_sockname1 = l_socket_get_sockname(this1, addr2);
  /* check for error. */
  if((-1 == rc_l_socket_get_sockname1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_l_socket_get_sockname1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_peername */
static int LSocketFD__get_peername__meth(lua_State *L) {
  LSocketFD this1;
  LSockAddr * addr2;
  errno_rc rc_l_socket_get_peername1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  addr2 = obj_type_LSockAddr_check(L,2);
  rc_l_socket_get_peername1 = l_socket_get_peername(this1, addr2);
  /* check for error. */
  if((-1 == rc_l_socket_get_peername1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_l_socket_get_peername1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: accept */
static int LSocketFD__accept__meth(lua_State *L) {
  LSocketFD this1;
  LSockAddr * peer2;
  int flags3;
  LSocketFD client1;
  errno_rc rc2 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  peer2 = obj_type_LSockAddr_optional(L,2);
  flags3 = luaL_optinteger(L,3,0);
  rc2 = l_socket_accept(this1, peer2, flags3);
	client1 = rc2;

  if(!(-1 == rc2)) {
    obj_type_LSocketFD_push(L, client1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc2);
  return 2;
}

/* method: send */
static int LSocketFD__send__meth(lua_State *L) {
  LSocketFD this1;
  size_t data_len2;
  const char * data2;
  int flags3;
  errno_rc rc1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  data2 = luaL_checklstring(L,2,&(data_len2));
  flags3 = luaL_optinteger(L,3,0);
	rc1 = l_socket_send(this1, data2, data_len2, flags3);
	/* rc1 >= 0, then return number of bytes sent. */
	if(rc1 >= 0) {
		lua_pushinteger(L, rc1);
		return 1;
	}

  /* check for error. */
  if((-1 == rc1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: recv */
static int LSocketFD__recv__meth(lua_State *L) {
  LSocketFD this1;
  size_t len2;
  int flags3;
  size_t data_len1 = 0;
  char * data1 = NULL;
  errno_rc rc2 = 0;
#define BUF_LEN 8192
	char buf[BUF_LEN];
	size_t buf_len = BUF_LEN;

  this1 = obj_type_LSocketFD_check(L,1);
  len2 = luaL_checkinteger(L,2);
  flags3 = luaL_optinteger(L,3,0);
	if(buf_len > len2) { buf_len = len2; }
	rc2 = l_socket_recv(this1, buf, buf_len, flags3);
	/* rc2 == 0, then socket is closed. */
	if(rc2 == 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "CLOSED");
		return 2;
	}
	data1 = buf;
	data_len1 = rc2;

  if(!(-1 == rc2)) {
    if(data1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, data1,data_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc2);
  return 2;
}

/* method: send_buffer */
static int LSocketFD__send_buffer__meth(lua_State *L) {
  LSocketFD this1;
  BufferIF_VAR(buf2);
  size_t off3;
  size_t len4;
  int flags5;
  errno_rc rc1 = 0;
	size_t data_len;
	const uint8_t *data;

  this1 = obj_type_LSocketFD_check(L,1);
  BufferIF_LUA_CHECK(L,2, buf2);
  off3 = luaL_optinteger(L,3,0);
  len4 = luaL_optinteger(L,4,0);
  flags5 = luaL_optinteger(L,5,0);
	data_len = buf2_if->get_size(buf2);
	data = buf2_if->const_data(buf2);
	/* apply offset. */
	if(off3 > 0) {
		if(off3 >= data_len) {
			luaL_argerror(L, 3, "Offset out-of-bounds.");
		}
		data += off3;
		data_len -= off3;
	}
	/* apply length. */
	if(len4 > 0) {
		if(len4 > data_len) {
			luaL_argerror(L, 4, "Length out-of-bounds.");
		}
		data_len = len4;
	}
	rc1 = l_socket_send(this1, data, data_len, flags5);
	/* rc1 >= 0, then return number of bytes sent. */
	if(rc1 >= 0) {
		lua_pushinteger(L, rc1);
		return 1;
	}

  /* check for error. */
  if((-1 == rc1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: recv_buffer */
static int LSocketFD__recv_buffer__meth(lua_State *L) {
  LSocketFD this1;
  MutableBufferIF_VAR(buf2);
  size_t off3;
  size_t len4;
  int flags5;
  int read_len1 = 0;
  errno_rc rc2 = 0;
	size_t cap_len;
	uint8_t *data;

  this1 = obj_type_LSocketFD_check(L,1);
  MutableBufferIF_LUA_CHECK(L,2, buf2);
  off3 = luaL_optinteger(L,3,0);
  len4 = luaL_optinteger(L,4,4096);
  flags5 = luaL_optinteger(L,5,0);
	cap_len = buf2_if->get_size(buf2);
	data = buf2_if->data(buf2);
	/* apply offset. */
	if(off3 > 0) {
		if(off3 >= cap_len) {
			luaL_argerror(L, 3, "Offset out-of-bounds.");
		}
		data += off3;
		cap_len -= off3;
	}
	/* calculate read length. */
	if(len4 < cap_len) {
		cap_len = len4;
	}
	if(0 == cap_len) {
		lua_pushnil(L);
		lua_pushliteral(L, "ENOBUFS");
		return 2;
	}
	rc2 = l_socket_recv(this1, data, cap_len, flags5);
	/* rc2 == 0, then socket is closed. */
	if(rc2 == 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "CLOSED");
		return 2;
	}
	read_len1 = rc2;

  if(!(-1 == rc2)) {
    lua_pushinteger(L, read_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc2);
  return 2;
}

/* method: send_buf */
static int LSocketFD__send_buf__meth(lua_State *L) {
  LSocketFD this1;
  const uint8_t * data2;
  size_t off3;
  size_t len4;
  int flags5;
  errno_rc rc1 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  data2 = lua_touserdata(L,2);
  off3 = luaL_optinteger(L,3,0);
  len4 = luaL_optinteger(L,4,0);
  flags5 = luaL_optinteger(L,5,0);
	data2 += off3;
	rc1 = l_socket_send(this1, data2, len4, flags5);
	/* rc1 >= 0, then return number of bytes sent. */
	if(rc1 >= 0) {
		lua_pushinteger(L, rc1);
		return 1;
	}

  /* check for error. */
  if((-1 == rc1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: recv_buf */
static int LSocketFD__recv_buf__meth(lua_State *L) {
  LSocketFD this1;
  uint8_t * data2;
  size_t off3;
  size_t len4;
  int flags5;
  int data_len1 = 0;
  errno_rc rc2 = 0;
  this1 = obj_type_LSocketFD_check(L,1);
  data2 = lua_touserdata(L,2);
  off3 = luaL_optinteger(L,3,0);
  len4 = luaL_optinteger(L,4,4096);
  flags5 = luaL_optinteger(L,5,0);
	data2 += off3;
	rc2 = l_socket_recv(this1, data2, len4, flags5);
	/* rc2 == 0, then socket is closed. */
	if(rc2 == 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "CLOSED");
		return 2;
	}
	data_len1 = rc2;

  if(!(-1 == rc2)) {
    lua_pushinteger(L, data_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc2);
  return 2;
}

/* method: new */
static int LIOBuffer__new__meth(lua_State *L) {
  LIOBuffer this1_store;
  LIOBuffer * this1 = &(this1_store);
	const uint8_t *data = NULL;
	size_t len = 0;
	int ltype = lua_type(L, 1);

	if(ltype == LUA_TSTRING) {
		data = lua_tolstring(L, 1, &len);
		l_iobuffer_init(this1, data, len);
	} else if(ltype == LUA_TNUMBER) {
		len = lua_tointeger(L, 1);
		l_iobuffer_init_cap(this1, len);
	}

  obj_type_LIOBuffer_push(L, this1);
  return 1;
}

/* method: free */
static int LIOBuffer__free__meth(lua_State *L) {
  LIOBuffer * this1;
  this1 = obj_type_LIOBuffer_delete(L,1);
  l_iobuffer_cleanup(this1);
  return 0;
}

/* method: __tostring */
static int LIOBuffer____tostring__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t data_len1 = 0;
  const char * data1 = NULL;
  this1 = obj_type_LIOBuffer_check(L,1);
 data1 = this1->buf; data_len1 = this1->_size; 
  if(data1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, data1,data_len1);
  return 1;
}

/* method: tostring */
static int LIOBuffer__tostring__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t data_len1 = 0;
  const char * data1 = NULL;
  this1 = obj_type_LIOBuffer_check(L,1);
 data1 = this1->buf; data_len1 = this1->_size; 
  if(data1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, data1,data_len1);
  return 1;
}

/* method: get_byte */
static int LIOBuffer__get_byte__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t offset2;
  unsigned int val1 = 0;
  this1 = obj_type_LIOBuffer_check(L,1);
  offset2 = luaL_checkinteger(L,2);
	/* check offset. */
	if(offset2 >= this1->_size) {
		return luaL_argerror(L, 2, "Offset out-of-bounds.");
	}
	val1 = this1->buf[offset2];

  lua_pushinteger(L, val1);
  return 1;
}

/* method: set_byte */
static int LIOBuffer__set_byte__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t offset2;
  unsigned int val3;
  this1 = obj_type_LIOBuffer_check(L,1);
  offset2 = luaL_checkinteger(L,2);
  val3 = luaL_checkinteger(L,3);
	/* check offset. */
	if(offset2 >= this1->_size) {
		return luaL_argerror(L, 2, "Offset out-of-bounds.");
	}
	this1->buf[offset2] = val3;

  return 0;
}

/* method: get_bytes */
static int LIOBuffer__get_bytes__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t offset2;
  size_t length3;
  this1 = obj_type_LIOBuffer_check(L,1);
  offset2 = luaL_optinteger(L,2,0);
  length3 = luaL_optinteger(L,3,0);
	size_t i;
	size_t data_len;
	const uint8_t *data;

	data_len = this1->_size;
	data = this1->buf;
	/* apply offset. */
	if(offset2 > 0) {
		if(offset2 >= data_len) {
			return luaL_argerror(L, 2, "Offset out-of-bounds.");
		}
		data += offset2;
		data_len -= offset2;
	}
	/* apply length. */
	if(length3 > 0) {
		if(length3 > data_len) {
			return luaL_argerror(L, 3, "Length out-of-bounds.");
		}
		data_len = length3;
	}
	luaL_checkstack(L, data_len, "No enough space on Lua stack for bytes.");
	for(i=0; i < data_len; i++) {
		lua_pushinteger(L, data[i]);
	}
	return data_len;

  return 0;
}

/* method: get_data */
static int LIOBuffer__get_data__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t offset2;
  size_t length3;
  size_t data_len1 = 0;
  const char * data1 = NULL;
  this1 = obj_type_LIOBuffer_check(L,1);
  offset2 = luaL_optinteger(L,2,0);
  length3 = luaL_optinteger(L,3,0);
	data_len1 = this1->_size;
	data1 = this1->buf;
	/* apply offset. */
	if(offset2 > 0) {
		if(offset2 >= data_len1) {
			return luaL_argerror(L, 2, "Offset out-of-bounds.");
		}
		data1 += offset2;
		data_len1 -= offset2;
	}
	/* apply length. */
	if(length3 > 0) {
		if(length3 > data_len1) {
			return luaL_argerror(L, 3, "Length out-of-bounds.");
		}
		data_len1 = length3;
	}

  if(data1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, data1,data_len1);
  return 1;
}

/* method: set_data */
static int LIOBuffer__set_data__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t data_len2;
  const char * data2;
  this1 = obj_type_LIOBuffer_check(L,1);
  data2 = luaL_checklstring(L,2,&(data_len2));
	/* check capacity */
	if(data_len2 > this1->_capacity) {
		if(!l_iobuffer_set_capacity(this1, data_len2)) {
			return luaL_argerror(L, 2, "Can't grow buffer, not enough space.");
		}
	}
	memcpy(this1->buf, data2, data_len2);
	this1->_size = data_len2;

  return 0;
}

/* method: copy_buffer */
static int LIOBuffer__copy_buffer__meth(lua_State *L) {
  LIOBuffer * this1;
  BufferIF_VAR(src2);
  size_t offset3;
  size_t length4;
	size_t data_len;
	const uint8_t *data;

  this1 = obj_type_LIOBuffer_check(L,1);
  BufferIF_LUA_CHECK(L,2, src2);
  offset3 = luaL_optinteger(L,3,0);
  length4 = luaL_optinteger(L,4,0);
	data_len = src2_if->get_size(src2);
	data = src2_if->const_data(src2);
	/* apply offset. */
	if(offset3 > 0) {
		if(offset3 >= data_len) {
			luaL_argerror(L, 3, "Offset out-of-bounds.");
		}
		data += offset3;
		data_len -= offset3;
	}
	/* apply length. */
	if(length4 > 0) {
		if(length4 > data_len) {
			luaL_argerror(L, 4, "Length out-of-bounds.");
		}
		data_len = length4;
	}
	/* check capacity */
	if(data_len > this1->_capacity) {
		if(!l_iobuffer_set_capacity(this1, data_len)) {
			return luaL_argerror(L, 2, "Can't grow buffer, not enough space.");
		}
	}
	memcpy(this1->buf, data, data_len);
	this1->_size = data_len;

  return 0;
}

/* method: reset */
static int LIOBuffer__reset__meth(lua_State *L) {
  LIOBuffer * this1;
  this1 = obj_type_LIOBuffer_check(L,1);
  l_iobuffer_reset(this1);
  return 0;
}

/* method: __len */
static int LIOBuffer____len__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t size1 = 0;
  this1 = obj_type_LIOBuffer_check(L,1);
size1 = this1->_size; 
  lua_pushinteger(L, size1);
  return 1;
}

/* method: size */
static int LIOBuffer__size__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t size1 = 0;
  this1 = obj_type_LIOBuffer_check(L,1);
size1 = this1->_size; 
  lua_pushinteger(L, size1);
  return 1;
}

/* method: set_size */
static int LIOBuffer__set_size__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t size2;
  bool rc_l_iobuffer_set_size1 = 0;
  this1 = obj_type_LIOBuffer_check(L,1);
  size2 = luaL_checkinteger(L,2);
  rc_l_iobuffer_set_size1 = l_iobuffer_set_size(this1, size2);
  lua_pushboolean(L, rc_l_iobuffer_set_size1);
  return 1;
}

/* method: capacity */
static int LIOBuffer__capacity__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t capacity1 = 0;
  this1 = obj_type_LIOBuffer_check(L,1);
capacity1 = this1->_capacity; 
  lua_pushinteger(L, capacity1);
  return 1;
}

/* method: set_capacity */
static int LIOBuffer__set_capacity__meth(lua_State *L) {
  LIOBuffer * this1;
  size_t capacity2;
  bool rc_l_iobuffer_set_capacity1 = 0;
  this1 = obj_type_LIOBuffer_check(L,1);
  capacity2 = luaL_checkinteger(L,2);
  rc_l_iobuffer_set_capacity1 = l_iobuffer_set_capacity(this1, capacity2);
  lua_pushboolean(L, rc_l_iobuffer_set_capacity1);
  return 1;
}



static const luaL_Reg obj_Errors_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Errors_methods[] = {
  {"description", Errors__description__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Errors_metas[] = {
  {NULL, NULL}
};

static const obj_const obj_Errors_constants[] = {
#ifdef ELNRNG
  {"ELNRNG", NULL, ELNRNG, CONST_NUMBER},
#endif
#ifdef EPFNOSUPPORT
  {"EPFNOSUPPORT", NULL, EPFNOSUPPORT, CONST_NUMBER},
#endif
#ifdef EBADR
  {"EBADR", NULL, EBADR, CONST_NUMBER},
#endif
#ifdef ENOLINK
  {"ENOLINK", NULL, ENOLINK, CONST_NUMBER},
#endif
#ifdef ENOMSG
  {"ENOMSG", NULL, ENOMSG, CONST_NUMBER},
#endif
#ifdef ERESTART
  {"ERESTART", NULL, ERESTART, CONST_NUMBER},
#endif
#ifdef EUCLEAN
  {"EUCLEAN", NULL, EUCLEAN, CONST_NUMBER},
#endif
#ifdef ELIBSCN
  {"ELIBSCN", NULL, ELIBSCN, CONST_NUMBER},
#endif
#ifdef EROFS
  {"EROFS", NULL, EROFS, CONST_NUMBER},
#endif
#ifdef EBADE
  {"EBADE", NULL, EBADE, CONST_NUMBER},
#endif
#ifdef ENOTSOCK
  {"ENOTSOCK", NULL, ENOTSOCK, CONST_NUMBER},
#endif
#ifdef ENOTCONN
  {"ENOTCONN", NULL, ENOTCONN, CONST_NUMBER},
#endif
#ifdef EREMOTE
  {"EREMOTE", NULL, EREMOTE, CONST_NUMBER},
#endif
#ifdef ECOMM
  {"ECOMM", NULL, ECOMM, CONST_NUMBER},
#endif
#ifdef ENODATA
  {"ENODATA", NULL, ENODATA, CONST_NUMBER},
#endif
#ifdef EPERM
  {"EPERM", NULL, EPERM, CONST_NUMBER},
#endif
#ifdef EBADRQC
  {"EBADRQC", NULL, EBADRQC, CONST_NUMBER},
#endif
#ifdef ENOSPC
  {"ENOSPC", NULL, ENOSPC, CONST_NUMBER},
#endif
#ifdef ELIBMAX
  {"ELIBMAX", NULL, ELIBMAX, CONST_NUMBER},
#endif
#ifdef EDOTDOT
  {"EDOTDOT", NULL, EDOTDOT, CONST_NUMBER},
#endif
#ifdef ENOPROTOOPT
  {"ENOPROTOOPT", NULL, ENOPROTOOPT, CONST_NUMBER},
#endif
#ifdef EBFONT
  {"EBFONT", NULL, EBFONT, CONST_NUMBER},
#endif
#ifdef EKEYREVOKED
  {"EKEYREVOKED", NULL, EKEYREVOKED, CONST_NUMBER},
#endif
#ifdef ESRMNT
  {"ESRMNT", NULL, ESRMNT, CONST_NUMBER},
#endif
#ifdef EOVERFLOW
  {"EOVERFLOW", NULL, EOVERFLOW, CONST_NUMBER},
#endif
#ifdef EDQUOT
  {"EDQUOT", NULL, EDQUOT, CONST_NUMBER},
#endif
#ifdef EFBIG
  {"EFBIG", NULL, EFBIG, CONST_NUMBER},
#endif
#ifdef EIDRM
  {"EIDRM", NULL, EIDRM, CONST_NUMBER},
#endif
#ifdef EDOM
  {"EDOM", NULL, EDOM, CONST_NUMBER},
#endif
#ifdef EPROTO
  {"EPROTO", NULL, EPROTO, CONST_NUMBER},
#endif
#ifdef EMULTIHOP
  {"EMULTIHOP", NULL, EMULTIHOP, CONST_NUMBER},
#endif
#ifdef EBUSY
  {"EBUSY", NULL, EBUSY, CONST_NUMBER},
#endif
#ifdef EDEADLOCK
  {"EDEADLOCK", NULL, EDEADLOCK, CONST_NUMBER},
#endif
#ifdef ENOPKG
  {"ENOPKG", NULL, ENOPKG, CONST_NUMBER},
#endif
#ifdef EPIPE
  {"EPIPE", NULL, EPIPE, CONST_NUMBER},
#endif
#ifdef EADDRINUSE
  {"EADDRINUSE", NULL, EADDRINUSE, CONST_NUMBER},
#endif
#ifdef EFAULT
  {"EFAULT", NULL, EFAULT, CONST_NUMBER},
#endif
#ifdef EDEADLK
  {"EDEADLK", NULL, EDEADLK, CONST_NUMBER},
#endif
#ifdef ENFILE
  {"ENFILE", NULL, ENFILE, CONST_NUMBER},
#endif
#ifdef EAGAIN
  {"EAGAIN", NULL, EAGAIN, CONST_NUMBER},
#endif
#ifdef ECONNABORTED
  {"ECONNABORTED", NULL, ECONNABORTED, CONST_NUMBER},
#endif
#ifdef EMLINK
  {"EMLINK", NULL, EMLINK, CONST_NUMBER},
#endif
#ifdef EBADMSG
  {"EBADMSG", NULL, EBADMSG, CONST_NUMBER},
#endif
#ifdef ERFKILL
  {"ERFKILL", NULL, ERFKILL, CONST_NUMBER},
#endif
#ifdef ENOTTY
  {"ENOTTY", NULL, ENOTTY, CONST_NUMBER},
#endif
#ifdef ELIBACC
  {"ELIBACC", NULL, ELIBACC, CONST_NUMBER},
#endif
#ifdef ETIME
  {"ETIME", NULL, ETIME, CONST_NUMBER},
#endif
#ifdef ECHILD
  {"ECHILD", NULL, ECHILD, CONST_NUMBER},
#endif
#ifdef ENOTRECOVERABLE
  {"ENOTRECOVERABLE", NULL, ENOTRECOVERABLE, CONST_NUMBER},
#endif
#ifdef EISCONN
  {"EISCONN", NULL, EISCONN, CONST_NUMBER},
#endif
#ifdef ENAVAIL
  {"ENAVAIL", NULL, ENAVAIL, CONST_NUMBER},
#endif
#ifdef EDESTADDRREQ
  {"EDESTADDRREQ", NULL, EDESTADDRREQ, CONST_NUMBER},
#endif
#ifdef EREMOTEIO
  {"EREMOTEIO", NULL, EREMOTEIO, CONST_NUMBER},
#endif
#ifdef ESTALE
  {"ESTALE", NULL, ESTALE, CONST_NUMBER},
#endif
#ifdef ESTRPIPE
  {"ESTRPIPE", NULL, ESTRPIPE, CONST_NUMBER},
#endif
#ifdef EHOSTUNREACH
  {"EHOSTUNREACH", NULL, EHOSTUNREACH, CONST_NUMBER},
#endif
#ifdef ENOTBLK
  {"ENOTBLK", NULL, ENOTBLK, CONST_NUMBER},
#endif
#ifdef EEXIST
  {"EEXIST", NULL, EEXIST, CONST_NUMBER},
#endif
#ifdef ENOTDIR
  {"ENOTDIR", NULL, ENOTDIR, CONST_NUMBER},
#endif
#ifdef EWOULDBLOCK
  {"EWOULDBLOCK", NULL, EWOULDBLOCK, CONST_NUMBER},
#endif
#ifdef EREMCHG
  {"EREMCHG", NULL, EREMCHG, CONST_NUMBER},
#endif
#ifdef ELOOP
  {"ELOOP", NULL, ELOOP, CONST_NUMBER},
#endif
#ifdef ENOTUNIQ
  {"ENOTUNIQ", NULL, ENOTUNIQ, CONST_NUMBER},
#endif
#ifdef EMEDIUMTYPE
  {"EMEDIUMTYPE", NULL, EMEDIUMTYPE, CONST_NUMBER},
#endif
#ifdef ENOLCK
  {"ENOLCK", NULL, ENOLCK, CONST_NUMBER},
#endif
#ifdef EUNATCH
  {"EUNATCH", NULL, EUNATCH, CONST_NUMBER},
#endif
#ifdef EPROTONOSUPPORT
  {"EPROTONOSUPPORT", NULL, EPROTONOSUPPORT, CONST_NUMBER},
#endif
#ifdef EHOSTDOWN
  {"EHOSTDOWN", NULL, EHOSTDOWN, CONST_NUMBER},
#endif
#ifdef ENXIO
  {"ENXIO", NULL, ENXIO, CONST_NUMBER},
#endif
#ifdef ECONNRESET
  {"ECONNRESET", NULL, ECONNRESET, CONST_NUMBER},
#endif
#ifdef EOWNERDEAD
  {"EOWNERDEAD", NULL, EOWNERDEAD, CONST_NUMBER},
#endif
#ifdef EL2HLT
  {"EL2HLT", NULL, EL2HLT, CONST_NUMBER},
#endif
#ifdef EBADSLT
  {"EBADSLT", NULL, EBADSLT, CONST_NUMBER},
#endif
#ifdef ESHUTDOWN
  {"ESHUTDOWN", NULL, ESHUTDOWN, CONST_NUMBER},
#endif
#ifdef EIO
  {"EIO", NULL, EIO, CONST_NUMBER},
#endif
#ifdef ENOANO
  {"ENOANO", NULL, ENOANO, CONST_NUMBER},
#endif
#ifdef EACCES
  {"EACCES", NULL, EACCES, CONST_NUMBER},
#endif
#ifdef EOPNOTSUPP
  {"EOPNOTSUPP", NULL, EOPNOTSUPP, CONST_NUMBER},
#endif
#ifdef EKEYREJECTED
  {"EKEYREJECTED", NULL, EKEYREJECTED, CONST_NUMBER},
#endif
#ifdef EL3HLT
  {"EL3HLT", NULL, EL3HLT, CONST_NUMBER},
#endif
#ifdef ELIBBAD
  {"ELIBBAD", NULL, ELIBBAD, CONST_NUMBER},
#endif
#ifdef ENODEV
  {"ENODEV", NULL, ENODEV, CONST_NUMBER},
#endif
#ifdef ENOSR
  {"ENOSR", NULL, ENOSR, CONST_NUMBER},
#endif
#ifdef ENOBUFS
  {"ENOBUFS", NULL, ENOBUFS, CONST_NUMBER},
#endif
#ifdef ENETUNREACH
  {"ENETUNREACH", NULL, ENETUNREACH, CONST_NUMBER},
#endif
#ifdef ENOKEY
  {"ENOKEY", NULL, ENOKEY, CONST_NUMBER},
#endif
#ifdef ECANCELED
  {"ECANCELED", NULL, ECANCELED, CONST_NUMBER},
#endif
#ifdef ENETRESET
  {"ENETRESET", NULL, ENETRESET, CONST_NUMBER},
#endif
#ifdef ENOENT
  {"ENOENT", NULL, ENOENT, CONST_NUMBER},
#endif
#ifdef ENOSTR
  {"ENOSTR", NULL, ENOSTR, CONST_NUMBER},
#endif
#ifdef EL3RST
  {"EL3RST", NULL, EL3RST, CONST_NUMBER},
#endif
#ifdef EMFILE
  {"EMFILE", NULL, EMFILE, CONST_NUMBER},
#endif
#ifdef ENOEXEC
  {"ENOEXEC", NULL, ENOEXEC, CONST_NUMBER},
#endif
#ifdef ENOTEMPTY
  {"ENOTEMPTY", NULL, ENOTEMPTY, CONST_NUMBER},
#endif
#ifdef EINVAL
  {"EINVAL", NULL, EINVAL, CONST_NUMBER},
#endif
#ifdef ERANGE
  {"ERANGE", NULL, ERANGE, CONST_NUMBER},
#endif
#ifdef ENONET
  {"ENONET", NULL, ENONET, CONST_NUMBER},
#endif
#ifdef EISNAM
  {"EISNAM", NULL, EISNAM, CONST_NUMBER},
#endif
#ifdef E2BIG
  {"E2BIG", NULL, E2BIG, CONST_NUMBER},
#endif
#ifdef ENOTNAM
  {"ENOTNAM", NULL, ENOTNAM, CONST_NUMBER},
#endif
#ifdef ETOOMANYREFS
  {"ETOOMANYREFS", NULL, ETOOMANYREFS, CONST_NUMBER},
#endif
#ifdef EADDRNOTAVAIL
  {"EADDRNOTAVAIL", NULL, EADDRNOTAVAIL, CONST_NUMBER},
#endif
#ifdef ENOSYS
  {"ENOSYS", NULL, ENOSYS, CONST_NUMBER},
#endif
#ifdef EINPROGRESS
  {"EINPROGRESS", NULL, EINPROGRESS, CONST_NUMBER},
#endif
#ifdef ETIMEDOUT
  {"ETIMEDOUT", NULL, ETIMEDOUT, CONST_NUMBER},
#endif
#ifdef EBADFD
  {"EBADFD", NULL, EBADFD, CONST_NUMBER},
#endif
#ifdef EISDIR
  {"EISDIR", NULL, EISDIR, CONST_NUMBER},
#endif
#ifdef EADV
  {"EADV", NULL, EADV, CONST_NUMBER},
#endif
#ifdef EINTR
  {"EINTR", NULL, EINTR, CONST_NUMBER},
#endif
#ifdef ESRCH
  {"ESRCH", NULL, ESRCH, CONST_NUMBER},
#endif
#ifdef ESOCKTNOSUPPORT
  {"ESOCKTNOSUPPORT", NULL, ESOCKTNOSUPPORT, CONST_NUMBER},
#endif
#ifdef EXFULL
  {"EXFULL", NULL, EXFULL, CONST_NUMBER},
#endif
#ifdef EPROTOTYPE
  {"EPROTOTYPE", NULL, EPROTOTYPE, CONST_NUMBER},
#endif
#ifdef EUSERS
  {"EUSERS", NULL, EUSERS, CONST_NUMBER},
#endif
#ifdef ENETDOWN
  {"ENETDOWN", NULL, ENETDOWN, CONST_NUMBER},
#endif
#ifdef EAFNOSUPPORT
  {"EAFNOSUPPORT", NULL, EAFNOSUPPORT, CONST_NUMBER},
#endif
#ifdef ESPIPE
  {"ESPIPE", NULL, ESPIPE, CONST_NUMBER},
#endif
#ifdef ETXTBSY
  {"ETXTBSY", NULL, ETXTBSY, CONST_NUMBER},
#endif
#ifdef ECHRNG
  {"ECHRNG", NULL, ECHRNG, CONST_NUMBER},
#endif
#ifdef ENOMEM
  {"ENOMEM", NULL, ENOMEM, CONST_NUMBER},
#endif
#ifdef ECONNREFUSED
  {"ECONNREFUSED", NULL, ECONNREFUSED, CONST_NUMBER},
#endif
#ifdef EMSGSIZE
  {"EMSGSIZE", NULL, EMSGSIZE, CONST_NUMBER},
#endif
#ifdef EKEYEXPIRED
  {"EKEYEXPIRED", NULL, EKEYEXPIRED, CONST_NUMBER},
#endif
#ifdef ENOMEDIUM
  {"ENOMEDIUM", NULL, ENOMEDIUM, CONST_NUMBER},
#endif
#ifdef EILSEQ
  {"EILSEQ", NULL, EILSEQ, CONST_NUMBER},
#endif
#ifdef ELIBEXEC
  {"ELIBEXEC", NULL, ELIBEXEC, CONST_NUMBER},
#endif
#ifdef ENOCSI
  {"ENOCSI", NULL, ENOCSI, CONST_NUMBER},
#endif
#ifdef EALREADY
  {"EALREADY", NULL, EALREADY, CONST_NUMBER},
#endif
#ifdef ENAMETOOLONG
  {"ENAMETOOLONG", NULL, ENAMETOOLONG, CONST_NUMBER},
#endif
#ifdef EBADF
  {"EBADF", NULL, EBADF, CONST_NUMBER},
#endif
#ifdef EXDEV
  {"EXDEV", NULL, EXDEV, CONST_NUMBER},
#endif
#ifdef EL2NSYNC
  {"EL2NSYNC", NULL, EL2NSYNC, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Errors_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_EAI_Errors_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_EAI_Errors_methods[] = {
  {"description", EAI_Errors__description__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_EAI_Errors_metas[] = {
  {NULL, NULL}
};

static const obj_const obj_EAI_Errors_constants[] = {
#ifdef EAI_NODATA
  {"EAI_NODATA", NULL, EAI_NODATA, CONST_NUMBER},
#endif
#ifdef EAI_SOCKTYPE
  {"EAI_SOCKTYPE", NULL, EAI_SOCKTYPE, CONST_NUMBER},
#endif
#ifdef EAI_AGAIN
  {"EAI_AGAIN", NULL, EAI_AGAIN, CONST_NUMBER},
#endif
#ifdef EAI_SERVICE
  {"EAI_SERVICE", NULL, EAI_SERVICE, CONST_NUMBER},
#endif
#ifdef EAI_CANCELED
  {"EAI_CANCELED", NULL, EAI_CANCELED, CONST_NUMBER},
#endif
#ifdef EAI_OVERFLOW
  {"EAI_OVERFLOW", NULL, EAI_OVERFLOW, CONST_NUMBER},
#endif
#ifdef EAI_BADFLAGS
  {"EAI_BADFLAGS", NULL, EAI_BADFLAGS, CONST_NUMBER},
#endif
#ifdef EAI_NOTCANCELED
  {"EAI_NOTCANCELED", NULL, EAI_NOTCANCELED, CONST_NUMBER},
#endif
#ifdef EAI_IDN_ENCODE
  {"EAI_IDN_ENCODE", NULL, EAI_IDN_ENCODE, CONST_NUMBER},
#endif
#ifdef EAI_FAMILY
  {"EAI_FAMILY", NULL, EAI_FAMILY, CONST_NUMBER},
#endif
#ifdef EAI_FAIL
  {"EAI_FAIL", NULL, EAI_FAIL, CONST_NUMBER},
#endif
#ifdef EAI_INPROGRESS
  {"EAI_INPROGRESS", NULL, EAI_INPROGRESS, CONST_NUMBER},
#endif
#ifdef EAI_MEMORY
  {"EAI_MEMORY", NULL, EAI_MEMORY, CONST_NUMBER},
#endif
#ifdef EAI_SYSTEM
  {"EAI_SYSTEM", NULL, EAI_SYSTEM, CONST_NUMBER},
#endif
#ifdef EAI_NONAME
  {"EAI_NONAME", NULL, EAI_NONAME, CONST_NUMBER},
#endif
#ifdef EAI_ALLDONE
  {"EAI_ALLDONE", NULL, EAI_ALLDONE, CONST_NUMBER},
#endif
#ifdef EAI_ADDRFAMILY
  {"EAI_ADDRFAMILY", NULL, EAI_ADDRFAMILY, CONST_NUMBER},
#endif
#ifdef EAI_INTR
  {"EAI_INTR", NULL, EAI_INTR, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_EAI_Errors_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Protocols_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Protocols_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Protocols_metas[] = {
  {"__index", Protocols____index__meth},
  {NULL, NULL}
};

static const obj_const obj_Protocols_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Protocols_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Services_pub_funcs[] = {
  {"byname", Services__byname__func},
  {"byport", Services__byport__func},
  {NULL, NULL}
};

static const luaL_Reg obj_Services_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Services_metas[] = {
  {"__index", Services____index__meth},
  {NULL, NULL}
};

static const obj_const obj_Services_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Services_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_LSockAddr_pub_funcs[] = {
  {"new", LSockAddr__new__meth},
  {"ip_port", LSockAddr__ip_port__meth},
  {"unix", LSockAddr__unix__meth},
  {"family", LSockAddr__family__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_LSockAddr_methods[] = {
  {"set_ip_port", LSockAddr__set_ip_port__meth},
  {"set_unix", LSockAddr__set_unix__meth},
  {"resize", LSockAddr__resize__meth},
  {"get_family", LSockAddr__get_family__meth},
  {"set_port", LSockAddr__set_port__meth},
  {"get_port", LSockAddr__get_port__meth},
  {"addr", LSockAddr__addr__meth},
  {"addrlen", LSockAddr__addrlen__meth},
  {"lookup_full", LSockAddr__lookup_full__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_LSockAddr_metas[] = {
  {"__gc", LSockAddr__delete__meth},
  {"__tostring", LSockAddr____tostring__meth},
  {"__eq", obj_simple_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_LSockAddr_bases[] = {
  {-1, NULL}
};

static const obj_field obj_LSockAddr_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_LSockAddr_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_LSockAddr_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_LAddrInfo_pub_funcs[] = {
  {"new", LAddrInfo__new__meth},
  {"ipv4", LAddrInfo__ipv4__meth},
  {"ipv6", LAddrInfo__ipv6__meth},
  {"full", LAddrInfo__full__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_LAddrInfo_methods[] = {
  {"first", LAddrInfo__first__meth},
  {"get_addr", LAddrInfo__get_addr__meth},
  {"get_canonname", LAddrInfo__get_canonname__meth},
  {"get_family", LAddrInfo__get_family__meth},
  {"get_socktype", LAddrInfo__get_socktype__meth},
  {"get_protocol", LAddrInfo__get_protocol__meth},
  {"next", LAddrInfo__next__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_LAddrInfo_metas[] = {
  {"__gc", LAddrInfo__delete__meth},
  {"__tostring", obj_simple_udata_default_tostring},
  {"__eq", obj_simple_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_LAddrInfo_bases[] = {
  {-1, NULL}
};

static const obj_field obj_LAddrInfo_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_LAddrInfo_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_LAddrInfo_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Options_pub_funcs[] = {
  {NULL, NULL}
};

static const obj_const obj_Options_constants[] = {
#ifdef IPV6_ROUTER_ALERT
  {"IPV6_ROUTER_ALERT", NULL, IPV6_ROUTER_ALERT, CONST_NUMBER},
#endif
#ifdef TCP_CONGESTION
  {"TCP_CONGESTION", NULL, TCP_CONGESTION, CONST_NUMBER},
#endif
#ifdef SO_SNDBUF
  {"SO_SNDBUF", NULL, SO_SNDBUF, CONST_NUMBER},
#endif
#ifdef SO_NO_CHECK
  {"SO_NO_CHECK", NULL, SO_NO_CHECK, CONST_NUMBER},
#endif
#ifdef SO_MARK
  {"SO_MARK", NULL, SO_MARK, CONST_NUMBER},
#endif
#ifdef IP_MINTTL
  {"IP_MINTTL", NULL, IP_MINTTL, CONST_NUMBER},
#endif
#ifdef TCP_SYNCNT
  {"TCP_SYNCNT", NULL, TCP_SYNCNT, CONST_NUMBER},
#endif
#ifdef IPV6_RECVERR
  {"IPV6_RECVERR", NULL, IPV6_RECVERR, CONST_NUMBER},
#endif
#ifdef SO_RCVBUFFORCE
  {"SO_RCVBUFFORCE", NULL, SO_RCVBUFFORCE, CONST_NUMBER},
#endif
#ifdef SO_SNDLOWAT
  {"SO_SNDLOWAT", NULL, SO_SNDLOWAT, CONST_NUMBER},
#endif
#ifdef IP_HDRINCL
  {"IP_HDRINCL", NULL, IP_HDRINCL, CONST_NUMBER},
#endif
#ifdef IP_MTU_DISCOVER
  {"IP_MTU_DISCOVER", NULL, IP_MTU_DISCOVER, CONST_NUMBER},
#endif
#ifdef IP_FREEBIND
  {"IP_FREEBIND", NULL, IP_FREEBIND, CONST_NUMBER},
#endif
#ifdef IPV6_MULTICAST_IF
  {"IPV6_MULTICAST_IF", NULL, IPV6_MULTICAST_IF, CONST_NUMBER},
#endif
#ifdef SOL_TCP
  {"SOL_TCP", NULL, SOL_TCP, CONST_NUMBER},
#endif
#ifdef IPV6_MTU_DISCOVER
  {"IPV6_MTU_DISCOVER", NULL, IPV6_MTU_DISCOVER, CONST_NUMBER},
#endif
#ifdef IPV6_UNICAST_HOPS
  {"IPV6_UNICAST_HOPS", NULL, IPV6_UNICAST_HOPS, CONST_NUMBER},
#endif
#ifdef SO_SNDBUFFORCE
  {"SO_SNDBUFFORCE", NULL, SO_SNDBUFFORCE, CONST_NUMBER},
#endif
#ifdef IP_RECVOPTS
  {"IP_RECVOPTS", NULL, IP_RECVOPTS, CONST_NUMBER},
#endif
#ifdef SO_PROTOCOL
  {"SO_PROTOCOL", NULL, SO_PROTOCOL, CONST_NUMBER},
#endif
#ifdef IPV6_MTU
  {"IPV6_MTU", NULL, IPV6_MTU, CONST_NUMBER},
#endif
#ifdef IP_TTL
  {"IP_TTL", NULL, IP_TTL, CONST_NUMBER},
#endif
#ifdef IPV6_RECVPKTINFO
  {"IPV6_RECVPKTINFO", NULL, IPV6_RECVPKTINFO, CONST_NUMBER},
#endif
#ifdef IPV6_HOPLIMIT
  {"IPV6_HOPLIMIT", NULL, IPV6_HOPLIMIT, CONST_NUMBER},
#endif
#ifdef IP_MULTICAST_TTL
  {"IP_MULTICAST_TTL", NULL, IP_MULTICAST_TTL, CONST_NUMBER},
#endif
#ifdef IPV6_ADDRFORM
  {"IPV6_ADDRFORM", NULL, IPV6_ADDRFORM, CONST_NUMBER},
#endif
#ifdef IPV6_V6ONLY
  {"IPV6_V6ONLY", NULL, IPV6_V6ONLY, CONST_NUMBER},
#endif
#ifdef IPV6_RECVDSTOPTS
  {"IPV6_RECVDSTOPTS", NULL, IPV6_RECVDSTOPTS, CONST_NUMBER},
#endif
#ifdef SOL_SOCKET
  {"SOL_SOCKET", NULL, SOL_SOCKET, CONST_NUMBER},
#endif
#ifdef TCP_KEEPIDLE
  {"TCP_KEEPIDLE", NULL, TCP_KEEPIDLE, CONST_NUMBER},
#endif
#ifdef SO_RCVBUF
  {"SO_RCVBUF", NULL, SO_RCVBUF, CONST_NUMBER},
#endif
#ifdef IP_RECVRETOPTS
  {"IP_RECVRETOPTS", NULL, IP_RECVRETOPTS, CONST_NUMBER},
#endif
#ifdef SOL_UDP
  {"SOL_UDP", NULL, SOL_UDP, CONST_NUMBER},
#endif
#ifdef IP_RECVORIGDSTADDR
  {"IP_RECVORIGDSTADDR", NULL, IP_RECVORIGDSTADDR, CONST_NUMBER},
#endif
#ifdef IPV6_MULTICAST_HOPS
  {"IPV6_MULTICAST_HOPS", NULL, IPV6_MULTICAST_HOPS, CONST_NUMBER},
#endif
#ifdef SO_TIMESTAMPING
  {"SO_TIMESTAMPING", NULL, SO_TIMESTAMPING, CONST_NUMBER},
#endif
#ifdef SO_TIMESTAMP
  {"SO_TIMESTAMP", NULL, SO_TIMESTAMP, CONST_NUMBER},
#endif
#ifdef IP_OPTIONS
  {"IP_OPTIONS", NULL, IP_OPTIONS, CONST_NUMBER},
#endif
#ifdef SO_BSDCOMPAT
  {"SO_BSDCOMPAT", NULL, SO_BSDCOMPAT, CONST_NUMBER},
#endif
#ifdef SO_REUSEADDR
  {"SO_REUSEADDR", NULL, SO_REUSEADDR, CONST_NUMBER},
#endif
#ifdef IP_RECVTOS
  {"IP_RECVTOS", NULL, IP_RECVTOS, CONST_NUMBER},
#endif
#ifdef SO_DONTROUTE
  {"SO_DONTROUTE", NULL, SO_DONTROUTE, CONST_NUMBER},
#endif
#ifdef SOL_IP
  {"SOL_IP", NULL, SOL_IP, CONST_NUMBER},
#endif
#ifdef SO_DOMAIN
  {"SO_DOMAIN", NULL, SO_DOMAIN, CONST_NUMBER},
#endif
#ifdef IPV6_CHECKSUM
  {"IPV6_CHECKSUM", NULL, IPV6_CHECKSUM, CONST_NUMBER},
#endif
#ifdef IPV6_MULTICAST_LOOP
  {"IPV6_MULTICAST_LOOP", NULL, IPV6_MULTICAST_LOOP, CONST_NUMBER},
#endif
#ifdef IP_RECVERR
  {"IP_RECVERR", NULL, IP_RECVERR, CONST_NUMBER},
#endif
#ifdef TCP_WINDOW_CLAMP
  {"TCP_WINDOW_CLAMP", NULL, TCP_WINDOW_CLAMP, CONST_NUMBER},
#endif
#ifdef IPV6_RECVHOPOPTS
  {"IPV6_RECVHOPOPTS", NULL, IPV6_RECVHOPOPTS, CONST_NUMBER},
#endif
#ifdef IPV6_RECVRTHDR
  {"IPV6_RECVRTHDR", NULL, IPV6_RECVRTHDR, CONST_NUMBER},
#endif
#ifdef SO_PRIORITY
  {"SO_PRIORITY", NULL, SO_PRIORITY, CONST_NUMBER},
#endif
#ifdef TCP_KEEPINTVL
  {"TCP_KEEPINTVL", NULL, TCP_KEEPINTVL, CONST_NUMBER},
#endif
#ifdef IPV6_RECVTCLASS
  {"IPV6_RECVTCLASS", NULL, IPV6_RECVTCLASS, CONST_NUMBER},
#endif
#ifdef TCP_QUICKACK
  {"TCP_QUICKACK", NULL, TCP_QUICKACK, CONST_NUMBER},
#endif
#ifdef TCP_KEEPCNT
  {"TCP_KEEPCNT", NULL, TCP_KEEPCNT, CONST_NUMBER},
#endif
#ifdef TCP_NODELAY
  {"TCP_NODELAY", NULL, TCP_NODELAY, CONST_NUMBER},
#endif
#ifdef TCP_MAXSEG
  {"TCP_MAXSEG", NULL, TCP_MAXSEG, CONST_NUMBER},
#endif
#ifdef SOL_UNIX
  {"SOL_UNIX", NULL, SOL_UNIX, CONST_NUMBER},
#endif
#ifdef IPV6_RECVHOPLIMIT
  {"IPV6_RECVHOPLIMIT", NULL, IPV6_RECVHOPLIMIT, CONST_NUMBER},
#endif
#ifdef IP_MULTICAST_LOOP
  {"IP_MULTICAST_LOOP", NULL, IP_MULTICAST_LOOP, CONST_NUMBER},
#endif
#ifdef TCP_DEFER_ACCEPT
  {"TCP_DEFER_ACCEPT", NULL, TCP_DEFER_ACCEPT, CONST_NUMBER},
#endif
#ifdef TCP_LINGER2
  {"TCP_LINGER2", NULL, TCP_LINGER2, CONST_NUMBER},
#endif
#ifdef SO_ERROR
  {"SO_ERROR", NULL, SO_ERROR, CONST_NUMBER},
#endif
#ifdef SO_TYPE
  {"SO_TYPE", NULL, SO_TYPE, CONST_NUMBER},
#endif
#ifdef IPV6_NEXTHOP
  {"IPV6_NEXTHOP", NULL, IPV6_NEXTHOP, CONST_NUMBER},
#endif
#ifdef SOL_IPV6
  {"SOL_IPV6", NULL, SOL_IPV6, CONST_NUMBER},
#endif
#ifdef IP_RECVTTL
  {"IP_RECVTTL", NULL, IP_RECVTTL, CONST_NUMBER},
#endif
#ifdef SO_KEEPALIVE
  {"SO_KEEPALIVE", NULL, SO_KEEPALIVE, CONST_NUMBER},
#endif
#ifdef SO_BROADCAST
  {"SO_BROADCAST", NULL, SO_BROADCAST, CONST_NUMBER},
#endif
#ifdef TCP_CORK
  {"TCP_CORK", NULL, TCP_CORK, CONST_NUMBER},
#endif
#ifdef SO_PASSCRED
  {"SO_PASSCRED", NULL, SO_PASSCRED, CONST_NUMBER},
#endif
#ifdef SO_ACCEPTCONN
  {"SO_ACCEPTCONN", NULL, SO_ACCEPTCONN, CONST_NUMBER},
#endif
#ifdef SO_RCVLOWAT
  {"SO_RCVLOWAT", NULL, SO_RCVLOWAT, CONST_NUMBER},
#endif
#ifdef IP_MTU
  {"IP_MTU", NULL, IP_MTU, CONST_NUMBER},
#endif
#ifdef IP_RETOPTS
  {"IP_RETOPTS", NULL, IP_RETOPTS, CONST_NUMBER},
#endif
#ifdef SO_OOBINLINE
  {"SO_OOBINLINE", NULL, SO_OOBINLINE, CONST_NUMBER},
#endif
#ifdef IP_TOS
  {"IP_TOS", NULL, IP_TOS, CONST_NUMBER},
#endif
#ifdef IP_ROUTER_ALERT
  {"IP_ROUTER_ALERT", NULL, IP_ROUTER_ALERT, CONST_NUMBER},
#endif
#ifdef IP_PKTINFO
  {"IP_PKTINFO", NULL, IP_PKTINFO, CONST_NUMBER},
#endif
#ifdef SO_TIMESTAMPNS
  {"SO_TIMESTAMPNS", NULL, SO_TIMESTAMPNS, CONST_NUMBER},
#endif
#ifdef IPV6_TCLASS
  {"IPV6_TCLASS", NULL, IPV6_TCLASS, CONST_NUMBER},
#endif
#ifdef SO_BINDTODEVICE
  {"SO_BINDTODEVICE", NULL, SO_BINDTODEVICE, CONST_NUMBER},
#endif
#ifdef SO_DEBUG
  {"SO_DEBUG", NULL, SO_DEBUG, CONST_NUMBER},
#endif
#ifdef SOL_ICMPV6
  {"SOL_ICMPV6", NULL, SOL_ICMPV6, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Options_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_SetSocketOption_pub_funcs[] = {
#if (IP_RECVOPTS)
  {"IP_RECVOPTS", SetSocketOption__IP_RECVOPTS__func},
#endif
#if (IP_MTU)
  {"IP_MTU", SetSocketOption__IP_MTU__func},
#endif
#if (IP_TOS)
  {"IP_TOS", SetSocketOption__IP_TOS__func},
#endif
#if (IP_OPTIONS)
  {"IP_OPTIONS", SetSocketOption__IP_OPTIONS__func},
#endif
#if (IP_TTL)
  {"IP_TTL", SetSocketOption__IP_TTL__func},
#endif
#if (IP_RECVTOS)
  {"IP_RECVTOS", SetSocketOption__IP_RECVTOS__func},
#endif
#if (IP_MINTTL)
  {"IP_MINTTL", SetSocketOption__IP_MINTTL__func},
#endif
#if (IP_MULTICAST_TTL)
  {"IP_MULTICAST_TTL", SetSocketOption__IP_MULTICAST_TTL__func},
#endif
#if (IP_RECVTTL)
  {"IP_RECVTTL", SetSocketOption__IP_RECVTTL__func},
#endif
#if (IP_FREEBIND)
  {"IP_FREEBIND", SetSocketOption__IP_FREEBIND__func},
#endif
#if (IP_PKTINFO)
  {"IP_PKTINFO", SetSocketOption__IP_PKTINFO__func},
#endif
#if (IP_HDRINCL)
  {"IP_HDRINCL", SetSocketOption__IP_HDRINCL__func},
#endif
#if (IP_MTU_DISCOVER)
  {"IP_MTU_DISCOVER", SetSocketOption__IP_MTU_DISCOVER__func},
#endif
#if (IP_RETOPTS)
  {"IP_RETOPTS", SetSocketOption__IP_RETOPTS__func},
#endif
#if (IP_RECVERR)
  {"IP_RECVERR", SetSocketOption__IP_RECVERR__func},
#endif
#if (IP_ROUTER_ALERT)
  {"IP_ROUTER_ALERT", SetSocketOption__IP_ROUTER_ALERT__func},
#endif
#if (IP_RECVRETOPTS)
  {"IP_RECVRETOPTS", SetSocketOption__IP_RECVRETOPTS__func},
#endif
#if (IP_RECVORIGDSTADDR)
  {"IP_RECVORIGDSTADDR", SetSocketOption__IP_RECVORIGDSTADDR__func},
#endif
#if (IP_MULTICAST_LOOP)
  {"IP_MULTICAST_LOOP", SetSocketOption__IP_MULTICAST_LOOP__func},
#endif
#if (IPV6_MULTICAST_HOPS)
  {"IPV6_MULTICAST_HOPS", SetSocketOption__IPV6_MULTICAST_HOPS__func},
#endif
#if (IPV6_RECVERR)
  {"IPV6_RECVERR", SetSocketOption__IPV6_RECVERR__func},
#endif
#if (IPV6_CHECKSUM)
  {"IPV6_CHECKSUM", SetSocketOption__IPV6_CHECKSUM__func},
#endif
#if (IPV6_MULTICAST_LOOP)
  {"IPV6_MULTICAST_LOOP", SetSocketOption__IPV6_MULTICAST_LOOP__func},
#endif
#if (IPV6_MULTICAST_IF)
  {"IPV6_MULTICAST_IF", SetSocketOption__IPV6_MULTICAST_IF__func},
#endif
#if (IPV6_RECVRTHDR)
  {"IPV6_RECVRTHDR", SetSocketOption__IPV6_RECVRTHDR__func},
#endif
#if (IPV6_MTU_DISCOVER)
  {"IPV6_MTU_DISCOVER", SetSocketOption__IPV6_MTU_DISCOVER__func},
#endif
#if (IPV6_UNICAST_HOPS)
  {"IPV6_UNICAST_HOPS", SetSocketOption__IPV6_UNICAST_HOPS__func},
#endif
#if (IPV6_NEXTHOP)
  {"IPV6_NEXTHOP", SetSocketOption__IPV6_NEXTHOP__func},
#endif
#if (IPV6_RECVHOPLIMIT)
  {"IPV6_RECVHOPLIMIT", SetSocketOption__IPV6_RECVHOPLIMIT__func},
#endif
#if (IPV6_MTU)
  {"IPV6_MTU", SetSocketOption__IPV6_MTU__func},
#endif
#if (IPV6_HOPLIMIT)
  {"IPV6_HOPLIMIT", SetSocketOption__IPV6_HOPLIMIT__func},
#endif
#if (IPV6_ADDRFORM)
  {"IPV6_ADDRFORM", SetSocketOption__IPV6_ADDRFORM__func},
#endif
#if (IPV6_V6ONLY)
  {"IPV6_V6ONLY", SetSocketOption__IPV6_V6ONLY__func},
#endif
#if (IPV6_RECVDSTOPTS)
  {"IPV6_RECVDSTOPTS", SetSocketOption__IPV6_RECVDSTOPTS__func},
#endif
#if (IPV6_TCLASS)
  {"IPV6_TCLASS", SetSocketOption__IPV6_TCLASS__func},
#endif
#if (IPV6_RECVTCLASS)
  {"IPV6_RECVTCLASS", SetSocketOption__IPV6_RECVTCLASS__func},
#endif
#if (IPV6_RECVHOPOPTS)
  {"IPV6_RECVHOPOPTS", SetSocketOption__IPV6_RECVHOPOPTS__func},
#endif
#if (IPV6_RECVPKTINFO)
  {"IPV6_RECVPKTINFO", SetSocketOption__IPV6_RECVPKTINFO__func},
#endif
#if (IPV6_ROUTER_ALERT)
  {"IPV6_ROUTER_ALERT", SetSocketOption__IPV6_ROUTER_ALERT__func},
#endif
#if (SO_SNDBUFFORCE)
  {"SO_SNDBUFFORCE", SetSocketOption__SO_SNDBUFFORCE__func},
#endif
#if (SO_TIMESTAMP)
  {"SO_TIMESTAMP", SetSocketOption__SO_TIMESTAMP__func},
#endif
#if (SO_SNDBUF)
  {"SO_SNDBUF", SetSocketOption__SO_SNDBUF__func},
#endif
#if (SO_NO_CHECK)
  {"SO_NO_CHECK", SetSocketOption__SO_NO_CHECK__func},
#endif
#if (SO_BSDCOMPAT)
  {"SO_BSDCOMPAT", SetSocketOption__SO_BSDCOMPAT__func},
#endif
#if (SO_REUSEADDR)
  {"SO_REUSEADDR", SetSocketOption__SO_REUSEADDR__func},
#endif
#if (SO_MARK)
  {"SO_MARK", SetSocketOption__SO_MARK__func},
#endif
#if (SO_TIMESTAMPING)
  {"SO_TIMESTAMPING", SetSocketOption__SO_TIMESTAMPING__func},
#endif
#if (SO_TIMESTAMPNS)
  {"SO_TIMESTAMPNS", SetSocketOption__SO_TIMESTAMPNS__func},
#endif
#if (SO_BINDTODEVICE)
  {"SO_BINDTODEVICE", SetSocketOption__SO_BINDTODEVICE__func},
#endif
#if (SO_DONTROUTE)
  {"SO_DONTROUTE", SetSocketOption__SO_DONTROUTE__func},
#endif
#if (SO_RCVBUFFORCE)
  {"SO_RCVBUFFORCE", SetSocketOption__SO_RCVBUFFORCE__func},
#endif
#if (SO_SNDLOWAT)
  {"SO_SNDLOWAT", SetSocketOption__SO_SNDLOWAT__func},
#endif
#if (SO_OOBINLINE)
  {"SO_OOBINLINE", SetSocketOption__SO_OOBINLINE__func},
#endif
#if (SO_RCVLOWAT)
  {"SO_RCVLOWAT", SetSocketOption__SO_RCVLOWAT__func},
#endif
#if (SO_PASSCRED)
  {"SO_PASSCRED", SetSocketOption__SO_PASSCRED__func},
#endif
#if (SO_RCVBUF)
  {"SO_RCVBUF", SetSocketOption__SO_RCVBUF__func},
#endif
#if (SO_PRIORITY)
  {"SO_PRIORITY", SetSocketOption__SO_PRIORITY__func},
#endif
#if (SO_BROADCAST)
  {"SO_BROADCAST", SetSocketOption__SO_BROADCAST__func},
#endif
#if (SO_KEEPALIVE)
  {"SO_KEEPALIVE", SetSocketOption__SO_KEEPALIVE__func},
#endif
#if (SO_DEBUG)
  {"SO_DEBUG", SetSocketOption__SO_DEBUG__func},
#endif
#if (TCP_CORK)
  {"TCP_CORK", SetSocketOption__TCP_CORK__func},
#endif
#if (TCP_KEEPIDLE)
  {"TCP_KEEPIDLE", SetSocketOption__TCP_KEEPIDLE__func},
#endif
#if (TCP_QUICKACK)
  {"TCP_QUICKACK", SetSocketOption__TCP_QUICKACK__func},
#endif
#if (TCP_CONGESTION)
  {"TCP_CONGESTION", SetSocketOption__TCP_CONGESTION__func},
#endif
#if (TCP_WINDOW_CLAMP)
  {"TCP_WINDOW_CLAMP", SetSocketOption__TCP_WINDOW_CLAMP__func},
#endif
#if (TCP_DEFER_ACCEPT)
  {"TCP_DEFER_ACCEPT", SetSocketOption__TCP_DEFER_ACCEPT__func},
#endif
#if (TCP_MAXSEG)
  {"TCP_MAXSEG", SetSocketOption__TCP_MAXSEG__func},
#endif
#if (TCP_NODELAY)
  {"TCP_NODELAY", SetSocketOption__TCP_NODELAY__func},
#endif
#if (TCP_KEEPCNT)
  {"TCP_KEEPCNT", SetSocketOption__TCP_KEEPCNT__func},
#endif
#if (TCP_LINGER2)
  {"TCP_LINGER2", SetSocketOption__TCP_LINGER2__func},
#endif
#if (TCP_SYNCNT)
  {"TCP_SYNCNT", SetSocketOption__TCP_SYNCNT__func},
#endif
#if (TCP_KEEPINTVL)
  {"TCP_KEEPINTVL", SetSocketOption__TCP_KEEPINTVL__func},
#endif
  {NULL, NULL}
};

static const obj_const obj_SetSocketOption_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_SetSocketOption_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_GetSocketOption_pub_funcs[] = {
#if (IP_RECVOPTS)
  {"IP_RECVOPTS", GetSocketOption__IP_RECVOPTS__func},
#endif
#if (IP_MTU)
  {"IP_MTU", GetSocketOption__IP_MTU__func},
#endif
#if (IP_TOS)
  {"IP_TOS", GetSocketOption__IP_TOS__func},
#endif
#if (IP_OPTIONS)
  {"IP_OPTIONS", GetSocketOption__IP_OPTIONS__func},
#endif
#if (IP_TTL)
  {"IP_TTL", GetSocketOption__IP_TTL__func},
#endif
#if (IP_RECVTOS)
  {"IP_RECVTOS", GetSocketOption__IP_RECVTOS__func},
#endif
#if (IP_MINTTL)
  {"IP_MINTTL", GetSocketOption__IP_MINTTL__func},
#endif
#if (IP_MULTICAST_TTL)
  {"IP_MULTICAST_TTL", GetSocketOption__IP_MULTICAST_TTL__func},
#endif
#if (IP_RECVTTL)
  {"IP_RECVTTL", GetSocketOption__IP_RECVTTL__func},
#endif
#if (IP_FREEBIND)
  {"IP_FREEBIND", GetSocketOption__IP_FREEBIND__func},
#endif
#if (IP_PKTINFO)
  {"IP_PKTINFO", GetSocketOption__IP_PKTINFO__func},
#endif
#if (IP_HDRINCL)
  {"IP_HDRINCL", GetSocketOption__IP_HDRINCL__func},
#endif
#if (IP_MTU_DISCOVER)
  {"IP_MTU_DISCOVER", GetSocketOption__IP_MTU_DISCOVER__func},
#endif
#if (IP_RETOPTS)
  {"IP_RETOPTS", GetSocketOption__IP_RETOPTS__func},
#endif
#if (IP_RECVERR)
  {"IP_RECVERR", GetSocketOption__IP_RECVERR__func},
#endif
#if (IP_ROUTER_ALERT)
  {"IP_ROUTER_ALERT", GetSocketOption__IP_ROUTER_ALERT__func},
#endif
#if (IP_RECVRETOPTS)
  {"IP_RECVRETOPTS", GetSocketOption__IP_RECVRETOPTS__func},
#endif
#if (IP_RECVORIGDSTADDR)
  {"IP_RECVORIGDSTADDR", GetSocketOption__IP_RECVORIGDSTADDR__func},
#endif
#if (IP_MULTICAST_LOOP)
  {"IP_MULTICAST_LOOP", GetSocketOption__IP_MULTICAST_LOOP__func},
#endif
#if (IPV6_MULTICAST_HOPS)
  {"IPV6_MULTICAST_HOPS", GetSocketOption__IPV6_MULTICAST_HOPS__func},
#endif
#if (IPV6_RECVERR)
  {"IPV6_RECVERR", GetSocketOption__IPV6_RECVERR__func},
#endif
#if (IPV6_CHECKSUM)
  {"IPV6_CHECKSUM", GetSocketOption__IPV6_CHECKSUM__func},
#endif
#if (IPV6_MULTICAST_LOOP)
  {"IPV6_MULTICAST_LOOP", GetSocketOption__IPV6_MULTICAST_LOOP__func},
#endif
#if (IPV6_MULTICAST_IF)
  {"IPV6_MULTICAST_IF", GetSocketOption__IPV6_MULTICAST_IF__func},
#endif
#if (IPV6_RECVRTHDR)
  {"IPV6_RECVRTHDR", GetSocketOption__IPV6_RECVRTHDR__func},
#endif
#if (IPV6_MTU_DISCOVER)
  {"IPV6_MTU_DISCOVER", GetSocketOption__IPV6_MTU_DISCOVER__func},
#endif
#if (IPV6_UNICAST_HOPS)
  {"IPV6_UNICAST_HOPS", GetSocketOption__IPV6_UNICAST_HOPS__func},
#endif
#if (IPV6_NEXTHOP)
  {"IPV6_NEXTHOP", GetSocketOption__IPV6_NEXTHOP__func},
#endif
#if (IPV6_RECVHOPLIMIT)
  {"IPV6_RECVHOPLIMIT", GetSocketOption__IPV6_RECVHOPLIMIT__func},
#endif
#if (IPV6_MTU)
  {"IPV6_MTU", GetSocketOption__IPV6_MTU__func},
#endif
#if (IPV6_HOPLIMIT)
  {"IPV6_HOPLIMIT", GetSocketOption__IPV6_HOPLIMIT__func},
#endif
#if (IPV6_V6ONLY)
  {"IPV6_V6ONLY", GetSocketOption__IPV6_V6ONLY__func},
#endif
#if (IPV6_RECVDSTOPTS)
  {"IPV6_RECVDSTOPTS", GetSocketOption__IPV6_RECVDSTOPTS__func},
#endif
#if (IPV6_TCLASS)
  {"IPV6_TCLASS", GetSocketOption__IPV6_TCLASS__func},
#endif
#if (IPV6_RECVTCLASS)
  {"IPV6_RECVTCLASS", GetSocketOption__IPV6_RECVTCLASS__func},
#endif
#if (IPV6_RECVHOPOPTS)
  {"IPV6_RECVHOPOPTS", GetSocketOption__IPV6_RECVHOPOPTS__func},
#endif
#if (IPV6_RECVPKTINFO)
  {"IPV6_RECVPKTINFO", GetSocketOption__IPV6_RECVPKTINFO__func},
#endif
#if (IPV6_ROUTER_ALERT)
  {"IPV6_ROUTER_ALERT", GetSocketOption__IPV6_ROUTER_ALERT__func},
#endif
#if (SO_SNDBUFFORCE)
  {"SO_SNDBUFFORCE", GetSocketOption__SO_SNDBUFFORCE__func},
#endif
#if (SO_ACCEPTCONN)
  {"SO_ACCEPTCONN", GetSocketOption__SO_ACCEPTCONN__func},
#endif
#if (SO_PROTOCOL)
  {"SO_PROTOCOL", GetSocketOption__SO_PROTOCOL__func},
#endif
#if (SO_TIMESTAMP)
  {"SO_TIMESTAMP", GetSocketOption__SO_TIMESTAMP__func},
#endif
#if (SO_SNDBUF)
  {"SO_SNDBUF", GetSocketOption__SO_SNDBUF__func},
#endif
#if (SO_NO_CHECK)
  {"SO_NO_CHECK", GetSocketOption__SO_NO_CHECK__func},
#endif
#if (SO_BSDCOMPAT)
  {"SO_BSDCOMPAT", GetSocketOption__SO_BSDCOMPAT__func},
#endif
#if (SO_REUSEADDR)
  {"SO_REUSEADDR", GetSocketOption__SO_REUSEADDR__func},
#endif
#if (SO_MARK)
  {"SO_MARK", GetSocketOption__SO_MARK__func},
#endif
#if (SO_TIMESTAMPING)
  {"SO_TIMESTAMPING", GetSocketOption__SO_TIMESTAMPING__func},
#endif
#if (SO_TIMESTAMPNS)
  {"SO_TIMESTAMPNS", GetSocketOption__SO_TIMESTAMPNS__func},
#endif
#if (SO_BINDTODEVICE)
  {"SO_BINDTODEVICE", GetSocketOption__SO_BINDTODEVICE__func},
#endif
#if (SO_DONTROUTE)
  {"SO_DONTROUTE", GetSocketOption__SO_DONTROUTE__func},
#endif
#if (SO_RCVBUFFORCE)
  {"SO_RCVBUFFORCE", GetSocketOption__SO_RCVBUFFORCE__func},
#endif
#if (SO_SNDLOWAT)
  {"SO_SNDLOWAT", GetSocketOption__SO_SNDLOWAT__func},
#endif
#if (SO_OOBINLINE)
  {"SO_OOBINLINE", GetSocketOption__SO_OOBINLINE__func},
#endif
#if (SO_DOMAIN)
  {"SO_DOMAIN", GetSocketOption__SO_DOMAIN__func},
#endif
#if (SO_RCVLOWAT)
  {"SO_RCVLOWAT", GetSocketOption__SO_RCVLOWAT__func},
#endif
#if (SO_PASSCRED)
  {"SO_PASSCRED", GetSocketOption__SO_PASSCRED__func},
#endif
#if (SO_RCVBUF)
  {"SO_RCVBUF", GetSocketOption__SO_RCVBUF__func},
#endif
#if (SO_PRIORITY)
  {"SO_PRIORITY", GetSocketOption__SO_PRIORITY__func},
#endif
#if (SO_TYPE)
  {"SO_TYPE", GetSocketOption__SO_TYPE__func},
#endif
#if (SO_BROADCAST)
  {"SO_BROADCAST", GetSocketOption__SO_BROADCAST__func},
#endif
#if (SO_KEEPALIVE)
  {"SO_KEEPALIVE", GetSocketOption__SO_KEEPALIVE__func},
#endif
#if (SO_DEBUG)
  {"SO_DEBUG", GetSocketOption__SO_DEBUG__func},
#endif
#if (SO_ERROR)
  {"SO_ERROR", GetSocketOption__SO_ERROR__func},
#endif
#if (TCP_CORK)
  {"TCP_CORK", GetSocketOption__TCP_CORK__func},
#endif
#if (TCP_KEEPIDLE)
  {"TCP_KEEPIDLE", GetSocketOption__TCP_KEEPIDLE__func},
#endif
#if (TCP_QUICKACK)
  {"TCP_QUICKACK", GetSocketOption__TCP_QUICKACK__func},
#endif
#if (TCP_CONGESTION)
  {"TCP_CONGESTION", GetSocketOption__TCP_CONGESTION__func},
#endif
#if (TCP_WINDOW_CLAMP)
  {"TCP_WINDOW_CLAMP", GetSocketOption__TCP_WINDOW_CLAMP__func},
#endif
#if (TCP_DEFER_ACCEPT)
  {"TCP_DEFER_ACCEPT", GetSocketOption__TCP_DEFER_ACCEPT__func},
#endif
#if (TCP_MAXSEG)
  {"TCP_MAXSEG", GetSocketOption__TCP_MAXSEG__func},
#endif
#if (TCP_NODELAY)
  {"TCP_NODELAY", GetSocketOption__TCP_NODELAY__func},
#endif
#if (TCP_KEEPCNT)
  {"TCP_KEEPCNT", GetSocketOption__TCP_KEEPCNT__func},
#endif
#if (TCP_LINGER2)
  {"TCP_LINGER2", GetSocketOption__TCP_LINGER2__func},
#endif
#if (TCP_SYNCNT)
  {"TCP_SYNCNT", GetSocketOption__TCP_SYNCNT__func},
#endif
#if (TCP_KEEPINTVL)
  {"TCP_KEEPINTVL", GetSocketOption__TCP_KEEPINTVL__func},
#endif
  {NULL, NULL}
};

static const obj_const obj_GetSocketOption_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_GetSocketOption_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_LSocketFD_pub_funcs[] = {
  {"new", LSocketFD__new__meth},
  {"fd", LSocketFD__fd__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_LSocketFD_methods[] = {
  {"close", LSocketFD__close__meth},
  {"shutdown", LSocketFD__shutdown__meth},
  {"fileno", LSocketFD__fileno__meth},
  {"set_nonblock", LSocketFD__set_nonblock__meth},
  {"connect", LSocketFD__connect__meth},
  {"bind", LSocketFD__bind__meth},
  {"listen", LSocketFD__listen__meth},
  {"get_sockname", LSocketFD__get_sockname__meth},
  {"get_peername", LSocketFD__get_peername__meth},
  {"accept", LSocketFD__accept__meth},
  {"send", LSocketFD__send__meth},
  {"recv", LSocketFD__recv__meth},
  {"send_buffer", LSocketFD__send_buffer__meth},
  {"recv_buffer", LSocketFD__recv_buffer__meth},
  {"send_buf", LSocketFD__send_buf__meth},
  {"recv_buf", LSocketFD__recv_buf__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_LSocketFD_metas[] = {
  {"__gc", LSocketFD__close__meth},
  {"__tostring", LSocketFD____tostring__meth},
  {"__eq", obj_simple_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_LSocketFD_bases[] = {
  {-1, NULL}
};

static const obj_field obj_LSocketFD_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_LSocketFD_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_LSocketFD_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_LIOBuffer_pub_funcs[] = {
  {"new", LIOBuffer__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_LIOBuffer_methods[] = {
  {"free", LIOBuffer__free__meth},
  {"tostring", LIOBuffer__tostring__meth},
  {"get_byte", LIOBuffer__get_byte__meth},
  {"set_byte", LIOBuffer__set_byte__meth},
  {"get_bytes", LIOBuffer__get_bytes__meth},
  {"get_data", LIOBuffer__get_data__meth},
  {"set_data", LIOBuffer__set_data__meth},
  {"copy_buffer", LIOBuffer__copy_buffer__meth},
  {"reset", LIOBuffer__reset__meth},
  {"size", LIOBuffer__size__meth},
  {"set_size", LIOBuffer__set_size__meth},
  {"capacity", LIOBuffer__capacity__meth},
  {"set_capacity", LIOBuffer__set_capacity__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_LIOBuffer_metas[] = {
  {"__gc", LIOBuffer__free__meth},
  {"__tostring", LIOBuffer____tostring__meth},
  {"__len", LIOBuffer____len__meth},
  {"__eq", obj_simple_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_LIOBuffer_bases[] = {
  {-1, NULL}
};

static const obj_field obj_LIOBuffer_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_LIOBuffer_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_LIOBuffer_implements[] = {
  { "BufferIF", &(LIOBuffer_Buffer) },

  { "MutableBufferIF", &(LIOBuffer_MutableBuffer) },

  {NULL, NULL}
};

static const luaL_Reg llnet_function[] = {
  {"socketpair", llnet__socketpair__func},
  {NULL, NULL}
};

static const obj_const llnet_constants[] = {
#ifdef IP_PMTUDISC_DONT
  {"IP_PMTUDISC_DONT", NULL, IP_PMTUDISC_DONT, CONST_NUMBER},
#endif
#ifdef IP_PMTUDISC_DO
  {"IP_PMTUDISC_DO", NULL, IP_PMTUDISC_DO, CONST_NUMBER},
#endif
#ifdef IPV6_RTHDR_TYPE_0
  {"IPV6_RTHDR_TYPE_0", NULL, IPV6_RTHDR_TYPE_0, CONST_NUMBER},
#endif
#ifdef SOCK_DGRAM
  {"SOCK_DGRAM", NULL, SOCK_DGRAM, CONST_NUMBER},
#endif
#ifdef AF_UNSPEC
  {"AF_UNSPEC", NULL, AF_UNSPEC, CONST_NUMBER},
#endif
#ifdef IPV6_RTHDR_STRICT
  {"IPV6_RTHDR_STRICT", NULL, IPV6_RTHDR_STRICT, CONST_NUMBER},
#endif
#ifdef SOCK_RAW
  {"SOCK_RAW", NULL, SOCK_RAW, CONST_NUMBER},
#endif
#ifdef SOCK_SEQPACKET
  {"SOCK_SEQPACKET", NULL, SOCK_SEQPACKET, CONST_NUMBER},
#endif
#ifdef SOCK_RDM
  {"SOCK_RDM", NULL, SOCK_RDM, CONST_NUMBER},
#endif
#ifdef IPV6_RTHDR_LOOSE
  {"IPV6_RTHDR_LOOSE", NULL, IPV6_RTHDR_LOOSE, CONST_NUMBER},
#endif
#ifdef SOCK_STREAM
  {"SOCK_STREAM", NULL, SOCK_STREAM, CONST_NUMBER},
#endif
#ifdef AF_UNIX
  {"AF_UNIX", NULL, AF_UNIX, CONST_NUMBER},
#endif
#ifdef SHUT_RDWR
  {"SHUT_RDWR", NULL, SHUT_RDWR, CONST_NUMBER},
#endif
#ifdef IPV6_PMTUDISC_DO
  {"IPV6_PMTUDISC_DO", NULL, IPV6_PMTUDISC_DO, CONST_NUMBER},
#endif
#ifdef IPV6_PMTUDISC_WANT
  {"IPV6_PMTUDISC_WANT", NULL, IPV6_PMTUDISC_WANT, CONST_NUMBER},
#endif
#ifdef AF_INET6
  {"AF_INET6", NULL, AF_INET6, CONST_NUMBER},
#endif
#ifdef IPV6_PMTUDISC_DONT
  {"IPV6_PMTUDISC_DONT", NULL, IPV6_PMTUDISC_DONT, CONST_NUMBER},
#endif
#ifdef IP_PMTUDISC_PROBE
  {"IP_PMTUDISC_PROBE", NULL, IP_PMTUDISC_PROBE, CONST_NUMBER},
#endif
#ifdef AF_IPX
  {"AF_IPX", NULL, AF_IPX, CONST_NUMBER},
#endif
#ifdef IP_PMTUDISC_WANT
  {"IP_PMTUDISC_WANT", NULL, IP_PMTUDISC_WANT, CONST_NUMBER},
#endif
#ifdef SOCK_NONBLOCK
  {"SOCK_NONBLOCK", NULL, SOCK_NONBLOCK, CONST_NUMBER},
#endif
#ifdef AF_PACKET
  {"AF_PACKET", NULL, AF_PACKET, CONST_NUMBER},
#endif
#ifdef SHUT_WR
  {"SHUT_WR", NULL, SHUT_WR, CONST_NUMBER},
#endif
#ifdef AF_INET
  {"AF_INET", NULL, AF_INET, CONST_NUMBER},
#endif
#ifdef IPV6_PMTUDISC_PROBE
  {"IPV6_PMTUDISC_PROBE", NULL, IPV6_PMTUDISC_PROBE, CONST_NUMBER},
#endif
#ifdef SOCK_CLOEXEC
  {"SOCK_CLOEXEC", NULL, SOCK_CLOEXEC, CONST_NUMBER},
#endif
#ifdef SHUT_RD
  {"SHUT_RD", NULL, SHUT_RD, CONST_NUMBER},
#endif
#ifdef AF_NETLINK
  {"AF_NETLINK", NULL, AF_NETLINK, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_Errors), REG_META, obj_Errors_pub_funcs, obj_Errors_methods, obj_Errors_metas, NULL, NULL, obj_Errors_constants, NULL, 1},
  { &(obj_type_EAI_Errors), REG_META, obj_EAI_Errors_pub_funcs, obj_EAI_Errors_methods, obj_EAI_Errors_metas, NULL, NULL, obj_EAI_Errors_constants, NULL, 1},
  { &(obj_type_Protocols), REG_META, obj_Protocols_pub_funcs, obj_Protocols_methods, obj_Protocols_metas, NULL, NULL, obj_Protocols_constants, NULL, 0},
  { &(obj_type_Services), REG_META, obj_Services_pub_funcs, obj_Services_methods, obj_Services_metas, NULL, NULL, obj_Services_constants, NULL, 0},
  { &(obj_type_LSockAddr), REG_OBJECT, obj_LSockAddr_pub_funcs, obj_LSockAddr_methods, obj_LSockAddr_metas, obj_LSockAddr_bases, obj_LSockAddr_fields, obj_LSockAddr_constants, obj_LSockAddr_implements, 0},
  { &(obj_type_LAddrInfo), REG_OBJECT, obj_LAddrInfo_pub_funcs, obj_LAddrInfo_methods, obj_LAddrInfo_metas, obj_LAddrInfo_bases, obj_LAddrInfo_fields, obj_LAddrInfo_constants, obj_LAddrInfo_implements, 0},
  { &(obj_type_Options), REG_PACKAGE, obj_Options_pub_funcs, NULL, NULL, NULL, NULL, obj_Options_constants, NULL, 1},
  { &(obj_type_SetSocketOption), REG_PACKAGE, obj_SetSocketOption_pub_funcs, NULL, NULL, NULL, NULL, obj_SetSocketOption_constants, NULL, 0},
  { &(obj_type_GetSocketOption), REG_PACKAGE, obj_GetSocketOption_pub_funcs, NULL, NULL, NULL, NULL, obj_GetSocketOption_constants, NULL, 0},
  { &(obj_type_LSocketFD), REG_OBJECT, obj_LSocketFD_pub_funcs, obj_LSocketFD_methods, obj_LSocketFD_metas, obj_LSocketFD_bases, obj_LSocketFD_fields, obj_LSocketFD_constants, obj_LSocketFD_implements, 0},
  { &(obj_type_LIOBuffer), REG_OBJECT, obj_LIOBuffer_pub_funcs, obj_LIOBuffer_methods, obj_LIOBuffer_metas, obj_LIOBuffer_bases, obj_LIOBuffer_fields, obj_LIOBuffer_constants, obj_LIOBuffer_implements, 0},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0}
};





#if LUAJIT_FFI
static const ffi_export_symbol llnet_ffi_export[] = {
  {NULL, { NULL } }
};
#endif




static const luaL_Reg submodule_libs[] = {
  {NULL, NULL}
};



static void create_object_instance_cache(lua_State *L) {
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(!lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop weak table. */
		return;
	}
	lua_pop(L, 1); /* pop nil. */
	/* create weak table for object instance references. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_newtable(L);               /* weak table. */
	lua_newtable(L);               /* metatable for weak table. */
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3);             /* metatable.__mode = 'v'  weak values. */
	lua_setmetatable(L, -2);       /* add metatable to weak table. */
	lua_rawset(L, LUA_REGISTRYINDEX);  /* create reference to weak table. */
}

LUA_NOBJ_API int luaopen_llnet(lua_State *L) {
	const reg_sub_module *reg = reg_sub_modules;
	const luaL_Reg *submodules = submodule_libs;
	int priv_table = -1;

	/* register interfaces */
	obj_register_interfaces(L, obj_interfaces);

	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
	lua_pushlightuserdata(L, obj_udata_private_key);
	lua_pushvalue(L, priv_table);
	lua_rawset(L, LUA_REGISTRYINDEX);  /* store private table in registry. */

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
#if REG_MODULES_AS_GLOBALS
	luaL_register(L, "llnet", llnet_function);
#else
	lua_newtable(L);
	luaL_setfuncs(L, llnet_function, 0);
#endif

	/* register module constants. */
	obj_type_register_constants(L, llnet_constants, -1, 0);

	for(; submodules->func != NULL ; submodules++) {
		lua_pushcfunction(L, submodules->func);
		lua_pushstring(L, submodules->name);
		lua_call(L, 1, 0);
	}

	/* register objects */
	for(; reg->type != NULL ; reg++) {
		lua_newtable(L); /* create public API table for object. */
		lua_pushvalue(L, -1); /* dup. object's public API table. */
		lua_setfield(L, -3, reg->type->name); /* module["<object_name>"] = <object public API> */
#if REG_OBJECTS_AS_GLOBALS
		lua_pushvalue(L, -1);                 /* dup value. */
		lua_setglobal(L, reg->type->name);    /* global: <object_name> = <object public API> */
#endif
		obj_type_register(L, reg, priv_table);
	}

#if LUAJIT_FFI
	if(nobj_check_ffi_support(L)) {
		nobj_try_loading_ffi(L, "llnet.nobj.ffi.lua", llnet_ffi_lua_code,
			llnet_ffi_export, priv_table);
	}
#endif

	lsock_init_winsock2(L);

	/* Cache reference to llnet.Errors table for errno->string convertion. */
	lua_pushlightuserdata(L, llnet_Errors_key);
	lua_getfield(L, -2, "Errors");
	lua_rawset(L, LUA_REGISTRYINDEX);

	/* Cache reference to llnet.EAI_Errors table for errno->string convertion. */
	lua_pushlightuserdata(L, llnet_EAI_Errors_key);
	lua_getfield(L, -2, "EAI_Errors");
	lua_rawset(L, LUA_REGISTRYINDEX);



	return 1;
}



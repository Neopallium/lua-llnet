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

#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 0
#define USE_FIELD_GET_SET_METHODS 0
#define LUAJIT_FFI 1
#define _GNU_SOURCE 1
#define L_PROTO_MAX_BUF 4096
#define _GNU_SOURCE 1
#define L_SERV_MAX_BUF 4096


#include <string.h>
#include <netdb.h>
#include "lsockaddr.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include "lsocket.h"



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
#define false 1
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

#ifndef obj_type_free
#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	free((obj)); \
} while(0)
#endif

#ifndef obj_type_new
#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = malloc(sizeof(type)); \
} while(0)
#endif

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
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

typedef struct reg_sub_module {
	obj_type        *type;
	module_reg_type req_type;
	const luaL_reg  *pub_funcs;
	const luaL_reg  *methods;
	const luaL_reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
	bool            bidirectional_consts;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char *obj_udata_weak_ref_key = "obj_udata_weak_ref_key";

#if LUAJIT_FFI
typedef struct ffi_export_symbol {
	const char *name;
	void       *sym;
} ffi_export_symbol;
#endif



typedef int errno_rc;

static void error_code__errno_rc__push(lua_State *L, errno_rc err);


static obj_type obj_types[] = {
#define obj_type_id_Errors 0
#define obj_type_Errors (obj_types[obj_type_id_Errors])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "Errors" },
#define obj_type_id_Protocols 1
#define obj_type_Protocols (obj_types[obj_type_id_Protocols])
  { NULL, 1, OBJ_TYPE_FLAG_WEAK_REF, "Protocols" },
#define obj_type_id_Services 2
#define obj_type_Services (obj_types[obj_type_id_Services])
  { NULL, 2, OBJ_TYPE_FLAG_WEAK_REF, "Services" },
#define obj_type_id_LSockAddr 3
#define obj_type_LSockAddr (obj_types[obj_type_id_LSockAddr])
  { NULL, 3, OBJ_TYPE_SIMPLE, "LSockAddr" },
#define obj_type_id_Options 4
#define obj_type_Options (obj_types[obj_type_id_Options])
  { NULL, 4, OBJ_TYPE_FLAG_WEAK_REF, "Options" },
#define obj_type_id_LSocketFD 5
#define obj_type_LSocketFD (obj_types[obj_type_id_LSocketFD])
  { NULL, 5, OBJ_TYPE_SIMPLE, "LSocketFD" },
  {NULL, -1, 0, NULL},
};


#if LUAJIT_FFI
static int nobj_udata_new_ffi(lua_State *L) {
	size_t size = luaL_checkinteger(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_settop(L, 2);
	/* create userdata. */
	lua_newuserdata(L, size);
	lua_replace(L, 1);
	/* set userdata's metatable. */
	lua_setmetatable(L, 1);
	return 1;
}

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
		return rc; /* return results of previous check. */
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
	return rc;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code, const ffi_export_symbol *ffi_exports, int priv_table)
{
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = luaL_loadbuffer(L, ffi_init_code, strlen(ffi_init_code), ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushcfunction(L, nobj_udata_new_ffi);
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

#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_objlen(L, _index);
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
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
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
	if(lua_isnil(L, _index)) {
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
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
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
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
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

	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
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
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	if(lua_isnil(L, _index)) {
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
	/* create new userdata. */
	void *ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_objlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_objlen(L, 2);

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
		bool bidirectional) {
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
	const luaL_reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_register(L, NULL, reg_list);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register_meta(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_reg *reg_list;

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_register(L, NULL, reg_list);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	/* register methods. */
	luaL_register(L, NULL, type_reg->methods);

	/* create metatable table. */
	lua_newtable(L);
	luaL_register(L, NULL, type_reg->metas); /* fill metatable */
	/* setmetatable on meta-object. */
	lua_setmetatable(L, -2);

	lua_pop(L, 1);  /* drop meta-object */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_reg *reg_list;
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
		luaL_register(L, NULL, reg_list); /* fill public API table. */

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

	luaL_register(L, NULL, type_reg->methods); /* fill methods table. */

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

	luaL_register(L, NULL, type_reg->metas); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2, type_reg->bidirectional_consts);

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



#define obj_type_LSockAddr_check(L, _index) \
	(LSockAddr *)obj_simple_udata_luacheck(L, _index, &(obj_type_LSockAddr))
#define obj_type_LSockAddr_optional(L, _index) \
	(LSockAddr *)obj_simple_udata_luaoptional(L, _index, &(obj_type_LSockAddr))
#define obj_type_LSockAddr_delete(L, _index) \
	(LSockAddr *)obj_simple_udata_luadelete(L, _index, &(obj_type_LSockAddr))
#define obj_type_LSockAddr_push(L, obj) \
	obj_simple_udata_luapush(L, obj, sizeof(LSockAddr), &(obj_type_LSockAddr))

#define obj_type_LSocketFD_check(L, _index) \
	*((LSocketFD *)obj_simple_udata_luacheck(L, _index, &(obj_type_LSocketFD)))
#define obj_type_LSocketFD_optional(L, _index) \
	*((LSocketFD *)obj_simple_udata_luaoptional(L, _index, &(obj_type_LSocketFD)))
#define obj_type_LSocketFD_delete(L, _index) \
	*((LSocketFD *)obj_simple_udata_luadelete(L, _index, &(obj_type_LSocketFD)))
#define obj_type_LSocketFD_push(L, obj) \
	obj_simple_udata_luapush(L, &(obj), sizeof(LSocketFD), &(obj_type_LSocketFD))




static const char llnet_ffi_lua_code[] = "local ffi=require\"ffi\"\n"
"local function ffi_safe_load(name, global)\n"
"	local stat, C = pcall(ffi.load, name, global)\n"
"	if not stat then return nil, C end\n"
"	return C\n"
"end\n"
"\n"
"local error = error\n"
"local type = type\n"
"local tonumber = tonumber\n"
"local tostring = tostring\n"
"local rawset = rawset\n"
"local p_config = package.config\n"
"local p_cpath = package.cpath\n"
"\n"
"local function ffi_load_cmodule(name)\n"
"	local dir_sep = p_config:sub(1,1)\n"
"	local path_sep = p_config:sub(3,3)\n"
"	local path_mark = p_config:sub(5,5)\n"
"	local path_match = \"([^\" .. path_sep .. \"]*)\" .. path_sep\n"
"	-- convert dotted name to directory path.\n"
"	name = name:gsub('%.', dir_sep)\n"
"	-- try each path in search path.\n"
"	for path in p_cpath:gmatch(path_match) do\n"
"		local fname = path:gsub(path_mark, name)\n"
"		local C, err = ffi_safe_load(fname)\n"
"		-- return opened library\n"
"		if C then return C end\n"
"	end\n"
"	return nil, \"Failed to find: \" .. name\n"
"end\n"
"\n"
"local _M, _priv, udata_new = ...\n"
"\n"
"local band = bit.band\n"
"local d_getmetatable = debug.getmetatable\n"
"local d_setmetatable = debug.setmetatable\n"
"\n"
"local OBJ_UDATA_FLAG_OWN		= 1\n"
"local OBJ_UDATA_FLAG_LOOKUP	= 2\n"
"local OBJ_UDATA_LAST_FLAG		= OBJ_UDATA_FLAG_LOOKUP\n"
"\n"
"local OBJ_TYPE_FLAG_WEAK_REF	= 1\n"
"local OBJ_TYPE_SIMPLE					= 2\n"
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
"-- cache mapping of cdata to userdata\n"
"local weak_objects = setmetatable({}, { __mode = \"v\" })\n"
"\n"
"local function obj_udata_luacheck_internal(obj, type_mt, not_delete)\n"
"	local obj_mt = d_getmetatable(obj)\n"
"	if obj_mt == type_mt then\n"
"		-- convert userdata to cdata.\n"
"		return ffi.cast(\"obj_udata *\", obj)\n"
"	end\n"
"	if not_delete then\n"
"		error(\"(expected `\" .. type_mt['.name'] .. \"`, got \" .. type(obj) .. \")\", 3)\n"
"	end\n"
"end\n"
"\n"
"local function obj_udata_luacheck(obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(obj, type_mt, true)\n"
"	return ud.obj\n"
"end\n"
"\n"
"local function obj_udata_to_cdata(objects, ud_obj, c_type, ud_mt)\n"
"	-- convert userdata to cdata.\n"
"	local c_obj = ffi.cast(c_type, obj_udata_luacheck(ud_obj, ud_mt))\n"
"	-- cache converted cdata\n"
"	rawset(objects, ud_obj, c_obj)\n"
"	return c_obj\n"
"end\n"
"\n"
"local function obj_udata_luadelete(ud_obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(ud_obj, type_mt, false)\n"
"	if not ud then return nil, 0 end\n"
"	local obj, flags = ud.obj, ud.flags\n"
"	-- null userdata.\n"
"	ud.obj = nil\n"
"	ud.flags = 0\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"	return obj, flags\n"
"end\n"
"\n"
"local function obj_udata_luapush(obj, type_mt, obj_type, flags)\n"
"	if obj == nil then return end\n"
"\n"
"	-- apply type's dynamic caster.\n"
"	if obj_type.dcaster ~= nil then\n"
"		local obj_ptr = ffi.new(\"void *[1]\", obj)\n"
"		local type_ptr = ffi.new(\"obj_type *[1]\", obj_type)\n"
"		obj_type.dcaster(obj_ptr, type_ptr)\n"
"		obj = obj_ptr[1]\n"
"		type = type_ptr[1]\n"
"	end\n"
"\n"
"	-- create new userdata\n"
"	local ud_obj = udata_new(ffi.sizeof\"obj_udata\", type_mt)\n"
"	local ud = ffi.cast(\"obj_udata *\", ud_obj)\n"
"	-- init. object\n"
"	ud.obj = obj\n"
"	ud.flags = flags\n"
"\n"
"	return ud_obj\n"
"end\n"
"\n"
"local function obj_udata_luadelete_weak(ud_obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(ud_obj, type_mt, false)\n"
"	if not ud then return nil, 0 end\n"
"	local obj, flags = ud.obj, ud.flags\n"
"	-- null userdata.\n"
"	ud.obj = nil\n"
"	ud.flags = 0\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"	-- remove object from weak ref. table.\n"
"	local obj_key = tonumber(ffi.cast('uintptr_t', obj))\n"
"	weak_objects[obj_key] = nil\n"
"	return obj, flags\n"
"end\n"
"\n"
"local function obj_udata_luapush_weak(obj, type_mt, obj_type, flags)\n"
"	if obj == nil then return end\n"
"\n"
"	-- apply type's dynamic caster.\n"
"	if obj_type.dcaster ~= nil then\n"
"		local obj_ptr = ffi.new(\"void *[1]\", obj)\n"
"		local type_ptr = ffi.new(\"obj_type *[1]\", obj_type)\n"
"		obj_type.dcaster(obj_ptr, type_ptr)\n"
"		obj = obj_ptr[1]\n"
"		type = type_ptr[1]\n"
"	end\n"
"\n"
"	-- lookup object in weak ref. table.\n"
"	local obj_key = tonumber(ffi.cast('uintptr_t', obj))\n"
"	local ud_obj = weak_objects[obj_key]\n"
"	if ud_obj ~= nil then return ud_obj end\n"
"\n"
"	-- create new userdata\n"
"	ud_obj = udata_new(ffi.sizeof\"obj_udata\", type_mt)\n"
"	local ud = ffi.cast(\"obj_udata *\", ud_obj)\n"
"	-- init. object\n"
"	ud.obj = obj\n"
"	ud.flags = flags\n"
"\n"
"	-- cache weak reference to object.\n"
"	weak_objects[obj_key] = ud_obj\n"
"\n"
"	return ud_obj\n"
"end\n"
"\n"
"local function obj_simple_udata_luacheck(ud_obj, type_mt)\n"
"	local obj_mt = d_getmetatable(ud_obj)\n"
"	if obj_mt == type_mt then\n"
"		-- convert userdata to cdata.\n"
"		return ffi.cast(\"void *\", ud_obj)\n"
"	end\n"
"	error(\"(expected `\" .. type_mt['.name'] .. \"`, got \" .. type(ud_obj) .. \")\", 3)\n"
"end\n"
"\n"
"local function obj_simple_udata_to_cdata(objects, ud_obj, c_type, ud_mt)\n"
"	-- convert userdata to cdata.\n"
"	local c_obj = ffi.cast(c_type, obj_simple_udata_luacheck(ud_obj, ud_mt))[0]\n"
"	-- cache converted cdata\n"
"	rawset(objects, ud_obj, c_obj)\n"
"	return c_obj\n"
"end\n"
"\n"
"local function obj_embed_udata_to_cdata(objects, ud_obj, c_type, ud_mt)\n"
"	-- convert userdata to cdata.\n"
"	local c_obj = ffi.cast(c_type, obj_simple_udata_luacheck(ud_obj, ud_mt))\n"
"	-- cache converted cdata\n"
"	rawset(objects, ud_obj, c_obj)\n"
"	return c_obj\n"
"end\n"
"\n"
"local function obj_simple_udata_luadelete(ud_obj, type_mt)\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"end\n"
"\n"
"local function obj_simple_udata_luapush(c_obj, size, type_mt)\n"
"	if c_obj == nil then return end\n"
"\n"
"	-- create new userdata\n"
"	local ud_obj = udata_new(size, type_mt)\n"
"	local cdata = ffi.cast(\"void *\", ud_obj)\n"
"	-- init. object\n"
"	ffi.copy(cdata, c_obj, size)\n"
"\n"
"	return ud_obj, cdata\n"
"end\n"
"\n"
"local function register_default_constructor(_pub, obj_name, constructor)\n"
"	local pub_constructor = _pub[obj_name]\n"
"	if type(pub_constructor) == 'table' then\n"
"		d_setmetatable(pub_constructor, { __call = function(t,...)\n"
"			return constructor(...)\n"
"		end,\n"
"		__metatable = false,\n"
"		})\n"
"	else\n"
"		_pub[obj_name] = constructor\n"
"	end\n"
"end\n"
"-- Load C module\n"
"local C = assert(ffi_load_cmodule(\"llnet\"))\n"
"\n"
"ffi.cdef[[\n"
"typedef int errno_rc;\n"
"\n"
"typedef struct LSockAddr LSockAddr;\n"
"typedef int LSocketFD;\n"
"\n"
"]]\n"
"\n"
"ffi.cdef[[\n"
"typedef struct sockaddr sockaddr;\n"
"typedef int socklen_t;\n"
"typedef int sa_family_t;\n"
"\n"
"struct LSockAddr {\n"
"	struct sockaddr  *addr;\n"
"	socklen_t        addrlen;\n"
"};\n"
"\n"
"typedef struct LSockAddr LSockAddr;\n"
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
"sockaddr * l_sockaddr_get_addr(LSockAddr *);\n"
"\n"
"socklen_t l_sockaddr_get_addrlen(LSockAddr *);\n"
"\n"
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
"errno_rc l_socket_set_int_option(LSocketFD, int, int, int);\n"
"\n"
"errno_rc l_socket_get_int_option(LSocketFD, int, int, int*);\n"
"\n"
"errno_rc l_socket_connect(LSocketFD, LSockAddr *);\n"
"\n"
"errno_rc l_socket_bind(LSocketFD, LSockAddr *);\n"
"\n"
"errno_rc l_socket_listen(LSocketFD, int);\n"
"\n"
"errno_rc l_socket_accept(LSocketFD, LSockAddr *, int);\n"
"\n"
"errno_rc l_socket_send(LSocketFD, const char *, size_t, int);\n"
"\n"
"\n"
"]]\n"
"\n"
"local _pub = {}\n"
"local _meth = {}\n"
"for obj_name,mt in pairs(_priv) do\n"
"	if type(mt) == 'table' and mt.__index then\n"
"		_meth[obj_name] = mt.__index\n"
"	end\n"
"end\n"
"_pub.llnet = _M\n"
"for obj_name,pub in pairs(_M) do\n"
"	_pub[obj_name] = pub\n"
"end\n"
"\n"
"\n"
"local obj_type_LSockAddr_check\n"
"local obj_type_LSockAddr_delete\n"
"local obj_type_LSockAddr_push\n"
"\n"
"do\n"
"	local LSockAddr_mt = _priv.LSockAddr\n"
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
"	function LSockAddr_mt:__tostring()\n"
"		return \"LSockAddr: \" .. tostring(ffi.cast('void *', self))\n"
"	end\n"
"\n"
"	function LSockAddr_mt.__eq(val1, val2)\n"
"		if not ffi.istype(\"LSockAddr\", val2) then return false end\n"
"		return (C.memcmp(val1, val2, LSockAddr_sizeof) == 0)\n"
"	end\n"
"end\n"
"\n"
"\n"
"local obj_type_LSocketFD_check\n"
"local obj_type_LSocketFD_delete\n"
"local obj_type_LSocketFD_push\n"
"\n"
"do\n"
"	local LSocketFD_mt = _priv.LSocketFD\n"
"	ffi_safe_cdef(\"LSocketFD_simple_wrapper\", [=[\n"
"		struct LSocketFD_t {\n"
"			LSocketFD _wrapped_val;\n"
"		};\n"
"		typedef struct LSocketFD_t LSocketFD_t;\n"
"	]=])\n"
"	local LSocketFD_sizeof = ffi.sizeof\"LSocketFD_t\"\n"
"\n"
"	function obj_type_LSocketFD_check(wrap_obj)\n"
"		return wrap_obj._wrapped_val\n"
"	end\n"
"\n"
"	function obj_type_LSocketFD_delete(wrap_obj)\n"
"		local this = wrap_obj._wrapped_val\n"
"		ffi.fill(wrap_obj, LSocketFD_sizeof, 0)\n"
"		return this\n"
"	end\n"
"\n"
"	function obj_type_LSocketFD_push(this)\n"
"		local wrap_obj = ffi.new(\"LSocketFD_t\")\n"
"		wrap_obj._wrapped_val = this\n"
"		return wrap_obj\n"
"	end\n"
"\n"
"	function LSocketFD_mt:__tostring()\n"
"		return \"LSocketFD: \" .. tostring(self._wrapped_val)\n"
"	end\n"
"\n"
"	function LSocketFD_mt.__eq(val1, val2)\n"
"		if not ffi.istype(\"LSocketFD_t\", val2) then return false end\n"
"		return (val1._wrapped_val == val2._wrapped_val)\n"
"	end\n"
"end\n"
"\n"
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
"  local this1\n"
"  local rc_l_sockaddr_init2\n"
"	this1 = ffi.new(\"LSockAddr\");\n"
"\n"
"  rc_l_sockaddr_init2 = C.l_sockaddr_init(this1)\n"
"  this1 =   obj_type_LSockAddr_push(this1)\n"
"  rc_l_sockaddr_init2 = rc_l_sockaddr_init2\n"
"  return this1, rc_l_sockaddr_init2\n"
"end\n"
"\n"
"register_default_constructor(_pub,\"LSockAddr\",_pub.LSockAddr.new)\n"
"-- method: ip_port\n"
"function _pub.LSockAddr.ip_port(ip1, port2)\n"
"  local ip_len1 = #ip1\n"
"  \n"
"  local this1\n"
"  local rc_l_sockaddr_init2\n"
"  local rc_l_sockaddr_set_ip_port3\n"
"	this1 = ffi.new(\"LSockAddr\");\n"
"\n"
"  rc_l_sockaddr_init2 = C.l_sockaddr_init(this1)\n"
"  rc_l_sockaddr_set_ip_port3 = C.l_sockaddr_set_ip_port(this1, ip1, port2)\n"
"  this1 =   obj_type_LSockAddr_push(this1)\n"
"  rc_l_sockaddr_init2 = rc_l_sockaddr_init2\n"
"  rc_l_sockaddr_set_ip_port3 = rc_l_sockaddr_set_ip_port3\n"
"  return this1, rc_l_sockaddr_init2, rc_l_sockaddr_set_ip_port3\n"
"end\n"
"\n"
"-- method: unix\n"
"function _pub.LSockAddr.unix(unix1)\n"
"  local unix_len1 = #unix1\n"
"  local this1\n"
"  local rc_l_sockaddr_init2\n"
"  local rc_l_sockaddr_set_unix3\n"
"	this1 = ffi.new(\"LSockAddr\");\n"
"\n"
"  rc_l_sockaddr_init2 = C.l_sockaddr_init(this1)\n"
"  rc_l_sockaddr_set_unix3 = C.l_sockaddr_set_unix(this1, unix1)\n"
"  this1 =   obj_type_LSockAddr_push(this1)\n"
"  rc_l_sockaddr_init2 = rc_l_sockaddr_init2\n"
"  rc_l_sockaddr_set_unix3 = rc_l_sockaddr_set_unix3\n"
"  return this1, rc_l_sockaddr_init2, rc_l_sockaddr_set_unix3\n"
"end\n"
"\n"
"-- method: family\n"
"function _pub.LSockAddr.family(family1)\n"
"  \n"
"  local this1\n"
"  local rc_l_sockaddr_init2\n"
"  local rc_l_sockaddr_set_family3\n"
"	this1 = ffi.new(\"LSockAddr\");\n"
"\n"
"  rc_l_sockaddr_init2 = C.l_sockaddr_init(this1)\n"
"  rc_l_sockaddr_set_family3 = C.l_sockaddr_set_family(this1, family1)\n"
"  this1 =   obj_type_LSockAddr_push(this1)\n"
"  rc_l_sockaddr_init2 = rc_l_sockaddr_init2\n"
"  rc_l_sockaddr_set_family3 = rc_l_sockaddr_set_family3\n"
"  return this1, rc_l_sockaddr_init2, rc_l_sockaddr_set_family3\n"
"end\n"
"\n"
"-- method: __gc\n"
"function _priv.LSockAddr.__gc(self)\n"
"  local this1 = obj_type_LSockAddr_delete(self)\n"
"  C.l_sockaddr_cleanup(this1)\n"
"  return \n"
"end\n"
"\n"
"-- method: set_ip_port\n"
"function _meth.LSockAddr.set_ip_port(self, ip2, port3)\n"
"  local this1 = obj_type_LSockAddr_check(self)\n"
"  local ip_len2 = #ip2\n"
"  \n"
"  local rc_l_sockaddr_set_ip_port1\n"
"  rc_l_sockaddr_set_ip_port1 = C.l_sockaddr_set_ip_port(this1, ip2, port3)\n"
"  rc_l_sockaddr_set_ip_port1 = rc_l_sockaddr_set_ip_port1\n"
"  return rc_l_sockaddr_set_ip_port1\n"
"end\n"
"\n"
"-- method: set_unix\n"
"function _meth.LSockAddr.set_unix(self, unix2)\n"
"  local this1 = obj_type_LSockAddr_check(self)\n"
"  local unix_len2 = #unix2\n"
"  local rc_l_sockaddr_set_unix1\n"
"  rc_l_sockaddr_set_unix1 = C.l_sockaddr_set_unix(this1, unix2)\n"
"  rc_l_sockaddr_set_unix1 = rc_l_sockaddr_set_unix1\n"
"  return rc_l_sockaddr_set_unix1\n"
"end\n"
"\n"
"-- method: resize\n"
"function _meth.LSockAddr.resize(self, addrlen2)\n"
"  local this1 = obj_type_LSockAddr_check(self)\n"
"  \n"
"  C.l_sockaddr_resize(this1, addrlen2)\n"
"  return \n"
"end\n"
"\n"
"-- method: get_family\n"
"function _meth.LSockAddr.get_family(self)\n"
"  local this1 = obj_type_LSockAddr_check(self)\n"
"  local rc_l_sockaddr_get_family1\n"
"  rc_l_sockaddr_get_family1 = C.l_sockaddr_get_family(this1)\n"
"  rc_l_sockaddr_get_family1 = rc_l_sockaddr_get_family1\n"
"  return rc_l_sockaddr_get_family1\n"
"end\n"
"\n"
"-- method: addr\n"
"function _meth.LSockAddr.addr(self)\n"
"  local this1 = obj_type_LSockAddr_check(self)\n"
"  local rc_l_sockaddr_get_addr1\n"
"  rc_l_sockaddr_get_addr1 = C.l_sockaddr_get_addr(this1)\n"
"  rc_l_sockaddr_get_addr1 = rc_l_sockaddr_get_addr1\n"
"  return rc_l_sockaddr_get_addr1\n"
"end\n"
"\n"
"-- method: addrlen\n"
"function _meth.LSockAddr.addrlen(self)\n"
"  local this1 = obj_type_LSockAddr_check(self)\n"
"  local rc_l_sockaddr_get_addrlen1\n"
"  rc_l_sockaddr_get_addrlen1 = C.l_sockaddr_get_addrlen(this1)\n"
"  rc_l_sockaddr_get_addrlen1 = rc_l_sockaddr_get_addrlen1\n"
"  return rc_l_sockaddr_get_addrlen1\n"
"end\n"
"\n"
"ffi.metatype(\"LSockAddr\", _priv.LSockAddr)\n"
"-- End \"LSockAddr\" FFI interface\n"
"\n"
"\n"
"-- Start \"Options\" FFI interface\n"
"-- End \"Options\" FFI interface\n"
"\n"
"\n"
"-- Start \"LSocketFD\" FFI interface\n"
"-- method: new\n"
"function _pub.LSocketFD.new(domain1, type2, protocol3, flags4)\n"
"  \n"
"  \n"
"  \n"
"  \n"
"  local this1\n"
"  this1 = C.l_socket_open(domain1, type2, protocol3, flags4)\n"
"  this1 =   obj_type_LSocketFD_push(this1)\n"
"  return this1\n"
"end\n"
"\n"
"register_default_constructor(_pub,\"LSocketFD\",_pub.LSocketFD.new)\n"
"-- method: fd\n"
"function _pub.LSocketFD.fd(fd1)\n"
"  \n"
"  local this1\n"
"	this1 = fd1\n"
"\n"
"  this1 =   obj_type_LSocketFD_push(this1)\n"
"  return this1\n"
"end\n"
"\n"
"-- method: close\n"
"function _meth.LSocketFD.close(self)\n"
"  local this1 = obj_type_LSocketFD_delete(self)\n"
"  C.l_socket_close_internal(this1)\n"
"  return \n"
"end\n"
"\n"
"-- method: __tostring\n"
"function _priv.LSocketFD.__tostring(self)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  local str1\n"
"	str1 = string.format(\"LSocketFD: fd=%i\\n\", this1)\n"
"\n"
"  str1 = ((nil ~= str1) and ffi.string(str1))\n"
"  return str1\n"
"end\n"
"\n"
"-- method: shutdown\n"
"function _meth.LSocketFD.shutdown(self, how2)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  \n"
"  local rc_l_socket_shutdown1\n"
"  rc_l_socket_shutdown1 = C.l_socket_shutdown(this1, how2)\n"
"  rc_l_socket_shutdown1 = rc_l_socket_shutdown1\n"
"  return rc_l_socket_shutdown1\n"
"end\n"
"\n"
"-- method: fileno\n"
"function _meth.LSocketFD.fileno(self)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  local fd1\n"
"	fd1 = this1\n"
"\n"
"  fd1 = fd1\n"
"  return fd1\n"
"end\n"
"\n"
"-- method: set_nonblock\n"
"function _meth.LSocketFD.set_nonblock(self, nonblock2)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  \n"
"  local rc_l_socket_set_nonblock1\n"
"  rc_l_socket_set_nonblock1 = C.l_socket_set_nonblock(this1, nonblock2)\n"
"  -- check for error.\n"
"  local rc_l_socket_set_nonblock1_err\n"
"  if (-1 == rc_l_socket_set_nonblock1) then\n"
"    rc_l_socket_set_nonblock1_err =   error_code__errno_rc__push(rc_l_socket_set_nonblock1)\n"
"    rc_l_socket_set_nonblock1 = nil\n"
"  else\n"
"    rc_l_socket_set_nonblock1 = true\n"
"  end\n"
"  return rc_l_socket_set_nonblock1, rc_l_socket_set_nonblock1_err\n"
"end\n"
"\n"
"-- method: set_int_option\n"
"function _meth.LSocketFD.set_int_option(self, level2, opt3, val4)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  \n"
"  \n"
"  \n"
"  local rc_l_socket_set_int_option1\n"
"  rc_l_socket_set_int_option1 = C.l_socket_set_int_option(this1, level2, opt3, val4)\n"
"  -- check for error.\n"
"  local rc_l_socket_set_int_option1_err\n"
"  if (-1 == rc_l_socket_set_int_option1) then\n"
"    rc_l_socket_set_int_option1_err =   error_code__errno_rc__push(rc_l_socket_set_int_option1)\n"
"    rc_l_socket_set_int_option1 = nil\n"
"  else\n"
"    rc_l_socket_set_int_option1 = true\n"
"  end\n"
"  return rc_l_socket_set_int_option1, rc_l_socket_set_int_option1_err\n"
"end\n"
"\n"
"  local get_int_option_val_tmp = ffi.new(\"int[1]\")\n"
"-- method: get_int_option\n"
"function _meth.LSocketFD.get_int_option(self, level2, opt3)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  \n"
"  \n"
"  local val1 = get_int_option_val_tmp\n"
"  local rc_l_socket_get_int_option2\n"
"  rc_l_socket_get_int_option2 = C.l_socket_get_int_option(this1, level2, opt3, val1)\n"
"  if not (-1 == rc_l_socket_get_int_option2) then\n"
"    val1 = val1\n"
"[0]  else\n"
"    val1 = nil\n"
"  end\n"
"  rc_l_socket_get_int_option2 =   error_code__errno_rc__push(rc_l_socket_get_int_option2)\n"
"  return val1, rc_l_socket_get_int_option2\n"
"end\n"
"\n"
"-- method: connect\n"
"function _meth.LSocketFD.connect(self, addr2)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  addr2 = obj_type_LSockAddr_check(addr2)\n"
"  local rc_l_socket_connect1\n"
"  rc_l_socket_connect1 = C.l_socket_connect(this1, addr2)\n"
"  -- check for error.\n"
"  local rc_l_socket_connect1_err\n"
"  if (-1 == rc_l_socket_connect1) then\n"
"    rc_l_socket_connect1_err =   error_code__errno_rc__push(rc_l_socket_connect1)\n"
"    rc_l_socket_connect1 = nil\n"
"  else\n"
"    rc_l_socket_connect1 = true\n"
"  end\n"
"  return rc_l_socket_connect1, rc_l_socket_connect1_err\n"
"end\n"
"\n"
"-- method: bind\n"
"function _meth.LSocketFD.bind(self, addr2)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  addr2 = obj_type_LSockAddr_check(addr2)\n"
"  local rc_l_socket_bind1\n"
"  rc_l_socket_bind1 = C.l_socket_bind(this1, addr2)\n"
"  -- check for error.\n"
"  local rc_l_socket_bind1_err\n"
"  if (-1 == rc_l_socket_bind1) then\n"
"    rc_l_socket_bind1_err =   error_code__errno_rc__push(rc_l_socket_bind1)\n"
"    rc_l_socket_bind1 = nil\n"
"  else\n"
"    rc_l_socket_bind1 = true\n"
"  end\n"
"  return rc_l_socket_bind1, rc_l_socket_bind1_err\n"
"end\n"
"\n"
"-- method: listen\n"
"function _meth.LSocketFD.listen(self, backlog2)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  \n"
"  local rc_l_socket_listen1\n"
"  rc_l_socket_listen1 = C.l_socket_listen(this1, backlog2)\n"
"  -- check for error.\n"
"  local rc_l_socket_listen1_err\n"
"  if (-1 == rc_l_socket_listen1) then\n"
"    rc_l_socket_listen1_err =   error_code__errno_rc__push(rc_l_socket_listen1)\n"
"    rc_l_socket_listen1 = nil\n"
"  else\n"
"    rc_l_socket_listen1 = true\n"
"  end\n"
"  return rc_l_socket_listen1, rc_l_socket_listen1_err\n"
"end\n"
"\n"
"-- method: accept\n"
"function _meth.LSocketFD.accept(self, peer2, flags3)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  peer2 = peer2 and obj_type_LSockAddr_check(peer2) or nil\n"
"    flags3 = flags3 or 0\n"
"  local client1\n"
"  local rc2\n"
"  rc2 = C.l_socket_accept(this1, peer2, flags3)\n"
"	client1 = rc2;\n"
"\n"
"  if not (-1 == rc2) then\n"
"    client1 =   obj_type_LSocketFD_push(client1)\n"
"  else\n"
"    client1 = nil\n"
"  end\n"
"  rc2 =   error_code__errno_rc__push(rc2)\n"
"  return client1, rc2\n"
"end\n"
"\n"
"-- method: send\n"
"function _meth.LSocketFD.send(self, data2, flags3)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  local data_len2 = #data2\n"
"    flags3 = flags3 or 0\n"
"  local rc_l_socket_send1\n"
"  rc_l_socket_send1 = C.l_socket_send(this1, data2, data_len2, flags3)\n"
"  -- check for error.\n"
"  local rc_l_socket_send1_err\n"
"  if (-1 == rc_l_socket_send1) then\n"
"    rc_l_socket_send1_err =   error_code__errno_rc__push(rc_l_socket_send1)\n"
"    rc_l_socket_send1 = nil\n"
"  else\n"
"    rc_l_socket_send1 = true\n"
"  end\n"
"  return rc_l_socket_send1, rc_l_socket_send1_err\n"
"end\n"
"\n"
"local tmp_buf_len = 8192\n"
"local tmp_buf = ffi.new(\"char[?]\", tmp_buf_len)\n"
"\n"
"-- method: recv\n"
"function _meth.LSocketFD.recv(self, len2, flags3)\n"
"  local this1 = obj_type_LSocketFD_check(self)\n"
"  \n"
"    flags3 = flags3 or 0\n"
"  local data_len1 = 0\n"
"  local data1\n"
"  local rc2\n"
"	local buf_len = (tmp_buf_len < len2) and tmp_buf_len or len2\n"
"	rc2 = C.l_socket_recv(this1, tmp_buf, buf_len, flags3)\n"
"	-- rc2 == 0, then socket is closed.\n"
"	if rc2 == 0 then return nil, \"CLOSED\" end\n"
"	data1 = tmp_buf;\n"
"	data_len1 = rc2;\n"
"\n"
"  if not (-1 == rc2) then\n"
"    data1 = ((nil ~= data1) and ffi.string(data1,data_len1))\n"
"  else\n"
"    data1 = nil\n"
"  end\n"
"  rc2 =   error_code__errno_rc__push(rc2)\n"
"  return data1, rc2\n"
"end\n"
"\n"
"ffi.metatype(\"LSocketFD_t\", _priv.LSocketFD)\n"
"-- End \"LSocketFD\" FFI interface\n"
"\n"
"_priv.LSocketFD.__gc = _meth.LSocketFD.LSocketFD__close__meth\n"
"";
static char *llnet_Errors_key = "llnet_Errors_key";

typedef struct sockaddr sockaddr;



/* method: description */
static int Errors__description__meth(lua_State *L) {
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
	if(err_num < 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "UNKNOWN ERROR");
		return 2;
	}
	msg1 = strerror(err_num);

  lua_pushstring(L, msg1);
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
  const char * name1 = luaL_checklstring(L,1,&(name_len1));
  size_t proto_len2;
  const char * proto2 = luaL_optlstring(L,2,"tcp",&(proto_len2));
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
  int port1 = luaL_checkinteger(L,1);
  size_t proto_len2;
  const char * proto2 = luaL_optlstring(L,2,"tcp",&(proto_len2));
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
  LSockAddr * this1;
  int rc_l_sockaddr_init2 = 0;
	LSockAddr addr;
	this1 = &addr;

  rc_l_sockaddr_init2 = l_sockaddr_init(this1);
  obj_type_LSockAddr_push(L, this1);
  lua_pushinteger(L, rc_l_sockaddr_init2);
  return 2;
}

/* method: ip_port */
static int LSockAddr__ip_port__meth(lua_State *L) {
  size_t ip_len1;
  const char * ip1 = luaL_checklstring(L,1,&(ip_len1));
  int port2 = luaL_checkinteger(L,2);
  LSockAddr * this1;
  int rc_l_sockaddr_init2 = 0;
  int rc_l_sockaddr_set_ip_port3 = 0;
	LSockAddr addr;
	this1 = &addr;

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
  const char * unix1 = luaL_checklstring(L,1,&(unix_len1));
  LSockAddr * this1;
  int rc_l_sockaddr_init2 = 0;
  int rc_l_sockaddr_set_unix3 = 0;
	LSockAddr addr;
	this1 = &addr;

  rc_l_sockaddr_init2 = l_sockaddr_init(this1);
  rc_l_sockaddr_set_unix3 = l_sockaddr_set_unix(this1, unix1);
  obj_type_LSockAddr_push(L, this1);
  lua_pushinteger(L, rc_l_sockaddr_init2);
  lua_pushinteger(L, rc_l_sockaddr_set_unix3);
  return 3;
}

/* method: family */
static int LSockAddr__family__meth(lua_State *L) {
  sa_family_t family1 = luaL_checkinteger(L,1);
  LSockAddr * this1;
  int rc_l_sockaddr_init2 = 0;
  int rc_l_sockaddr_set_family3 = 0;
	LSockAddr addr;
	this1 = &addr;

  rc_l_sockaddr_init2 = l_sockaddr_init(this1);
  rc_l_sockaddr_set_family3 = l_sockaddr_set_family(this1, family1);
  obj_type_LSockAddr_push(L, this1);
  lua_pushinteger(L, rc_l_sockaddr_init2);
  lua_pushinteger(L, rc_l_sockaddr_set_family3);
  return 3;
}

/* method: delete */
static int LSockAddr__delete__meth(lua_State *L) {
  LSockAddr * this1 = obj_type_LSockAddr_delete(L,1);
  l_sockaddr_cleanup(this1);
  return 0;
}

/* method: set_ip_port */
static int LSockAddr__set_ip_port__meth(lua_State *L) {
  LSockAddr * this1 = obj_type_LSockAddr_check(L,1);
  size_t ip_len2;
  const char * ip2 = luaL_checklstring(L,2,&(ip_len2));
  int port3 = luaL_checkinteger(L,3);
  int rc_l_sockaddr_set_ip_port1 = 0;
  rc_l_sockaddr_set_ip_port1 = l_sockaddr_set_ip_port(this1, ip2, port3);
  lua_pushinteger(L, rc_l_sockaddr_set_ip_port1);
  return 1;
}

/* method: set_unix */
static int LSockAddr__set_unix__meth(lua_State *L) {
  LSockAddr * this1 = obj_type_LSockAddr_check(L,1);
  size_t unix_len2;
  const char * unix2 = luaL_checklstring(L,2,&(unix_len2));
  int rc_l_sockaddr_set_unix1 = 0;
  rc_l_sockaddr_set_unix1 = l_sockaddr_set_unix(this1, unix2);
  lua_pushinteger(L, rc_l_sockaddr_set_unix1);
  return 1;
}

/* method: resize */
static int LSockAddr__resize__meth(lua_State *L) {
  LSockAddr * this1 = obj_type_LSockAddr_check(L,1);
  socklen_t addrlen2 = luaL_checkinteger(L,2);
  l_sockaddr_resize(this1, addrlen2);
  return 0;
}

/* method: get_family */
static int LSockAddr__get_family__meth(lua_State *L) {
  LSockAddr * this1 = obj_type_LSockAddr_check(L,1);
  sa_family_t rc_l_sockaddr_get_family1 = 0;
  rc_l_sockaddr_get_family1 = l_sockaddr_get_family(this1);
  lua_pushinteger(L, rc_l_sockaddr_get_family1);
  return 1;
}

/* method: addr */
static int LSockAddr__addr__meth(lua_State *L) {
  LSockAddr * this1 = obj_type_LSockAddr_check(L,1);
  sockaddr * rc_l_sockaddr_get_addr1 = NULL;
  rc_l_sockaddr_get_addr1 = l_sockaddr_get_addr(this1);
  lua_pushlightuserdata(L, rc_l_sockaddr_get_addr1);
  return 1;
}

/* method: addrlen */
static int LSockAddr__addrlen__meth(lua_State *L) {
  LSockAddr * this1 = obj_type_LSockAddr_check(L,1);
  socklen_t rc_l_sockaddr_get_addrlen1 = 0;
  rc_l_sockaddr_get_addrlen1 = l_sockaddr_get_addrlen(this1);
  lua_pushinteger(L, rc_l_sockaddr_get_addrlen1);
  return 1;
}

/* method: new */
static int LSocketFD__new__meth(lua_State *L) {
  int domain1 = luaL_checkinteger(L,1);
  int type2 = luaL_checkinteger(L,2);
  int protocol3 = luaL_checkinteger(L,3);
  int flags4 = luaL_checkinteger(L,4);
  LSocketFD this1;
  this1 = l_socket_open(domain1, type2, protocol3, flags4);
  obj_type_LSocketFD_push(L, this1);
  return 1;
}

/* method: fd */
static int LSocketFD__fd__meth(lua_State *L) {
  int fd1 = luaL_checkinteger(L,1);
  LSocketFD this1;
	this1 = fd1;

  obj_type_LSocketFD_push(L, this1);
  return 1;
}

/* method: close */
static int LSocketFD__close__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_delete(L,1);
  l_socket_close_internal(this1);
  return 0;
}

/* method: __tostring */
static int LSocketFD____tostring__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  const char * str1 = NULL;
	char tmp[1024];

	str1 = tmp;
	snprintf(tmp, 1024, "LSocketFD: fd=%d\n", this1);

  lua_pushstring(L, str1);
  return 1;
}

/* method: shutdown */
static int LSocketFD__shutdown__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  int how2 = luaL_checkinteger(L,2);
  int rc_l_socket_shutdown1 = 0;
  rc_l_socket_shutdown1 = l_socket_shutdown(this1, how2);
  lua_pushinteger(L, rc_l_socket_shutdown1);
  return 1;
}

/* method: fileno */
static int LSocketFD__fileno__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  int fd1 = 0;
	fd1 = this1;

  lua_pushinteger(L, fd1);
  return 1;
}

/* method: set_nonblock */
static int LSocketFD__set_nonblock__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  bool nonblock2 = lua_toboolean(L,2);
  errno_rc rc_l_socket_set_nonblock1 = 0;
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

/* method: set_option */
static int LSocketFD__set_option__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  int level2 = luaL_checkinteger(L,2);
  int opt3 = luaL_checkinteger(L,3);
  errno_rc rc1 = 0;
	int val_type;

	val_type = lua_type(L, 4);
	if(val_type == LUA_TSTRING) {
		size_t len;
		const char *val = lua_tolstring(L, 4, &len);
		rc1 = l_socket_set_option(this1, level2, opt3, val, len);
	} else if(val_type == LUA_TNUMBER) {
		int val = lua_tointeger(L, 4);
		rc1 = l_socket_set_option(this1, level2, opt3, &val, sizeof(val));
	} else {
		return luaL_argerror(L, 4, "expected string/number");
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

/* method: set_int_option */
static int LSocketFD__set_int_option__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  int level2 = luaL_checkinteger(L,2);
  int opt3 = luaL_checkinteger(L,3);
  int val4 = luaL_checkinteger(L,4);
  errno_rc rc_l_socket_set_int_option1 = 0;
  rc_l_socket_set_int_option1 = l_socket_set_int_option(this1, level2, opt3, val4);
  /* check for error. */
  if((-1 == rc_l_socket_set_int_option1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_l_socket_set_int_option1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_int_option */
static int LSocketFD__get_int_option__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  int level2 = luaL_checkinteger(L,2);
  int opt3 = luaL_checkinteger(L,3);
  int val1 = 0;
  errno_rc rc_l_socket_get_int_option2 = 0;
  rc_l_socket_get_int_option2 = l_socket_get_int_option(this1, level2, opt3, &(val1));
  if(!(-1 == rc_l_socket_get_int_option2)) {
    lua_pushinteger(L, val1);
  } else {
    lua_pushnil(L);
  }
  error_code__errno_rc__push(L, rc_l_socket_get_int_option2);
  return 2;
}

/* method: connect */
static int LSocketFD__connect__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  LSockAddr * addr2 = obj_type_LSockAddr_check(L,2);
  errno_rc rc_l_socket_connect1 = 0;
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
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  LSockAddr * addr2 = obj_type_LSockAddr_check(L,2);
  errno_rc rc_l_socket_bind1 = 0;
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
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  int backlog2 = luaL_checkinteger(L,2);
  errno_rc rc_l_socket_listen1 = 0;
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

/* method: accept */
static int LSocketFD__accept__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  LSockAddr * peer2 = obj_type_LSockAddr_optional(L,2);
  int flags3 = luaL_optinteger(L,3,0);
  LSocketFD client1;
  errno_rc rc2 = 0;
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
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  size_t data_len2;
  const char * data2 = luaL_checklstring(L,2,&(data_len2));
  int flags3 = luaL_optinteger(L,3,0);
  errno_rc rc_l_socket_send1 = 0;
  rc_l_socket_send1 = l_socket_send(this1, data2, data_len2, flags3);
  /* check for error. */
  if((-1 == rc_l_socket_send1)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_l_socket_send1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: recv */
static int LSocketFD__recv__meth(lua_State *L) {
  LSocketFD this1 = obj_type_LSocketFD_check(L,1);
  size_t len2 = luaL_checkinteger(L,2);
  int flags3 = luaL_optinteger(L,3,0);
  size_t data_len1 = 0;
  char * data1 = NULL;
  errno_rc rc2 = 0;
#define BUF_LEN 8192
	char buf[BUF_LEN];
	int buf_len = BUF_LEN;

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



static const luaL_reg obj_Errors_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_reg obj_Errors_methods[] = {
  {"description", Errors__description__meth},
  {NULL, NULL}
};

static const luaL_reg obj_Errors_metas[] = {
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

static const luaL_reg obj_Protocols_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_reg obj_Protocols_methods[] = {
  {NULL, NULL}
};

static const luaL_reg obj_Protocols_metas[] = {
  {"__index", Protocols____index__meth},
  {NULL, NULL}
};

static const obj_const obj_Protocols_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const luaL_reg obj_Services_pub_funcs[] = {
  {"byname", Services__byname__func},
  {"byport", Services__byport__func},
  {NULL, NULL}
};

static const luaL_reg obj_Services_methods[] = {
  {NULL, NULL}
};

static const luaL_reg obj_Services_metas[] = {
  {"__index", Services____index__meth},
  {NULL, NULL}
};

static const obj_const obj_Services_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const luaL_reg obj_LSockAddr_pub_funcs[] = {
  {"new", LSockAddr__new__meth},
  {"ip_port", LSockAddr__ip_port__meth},
  {"unix", LSockAddr__unix__meth},
  {"family", LSockAddr__family__meth},
  {NULL, NULL}
};

static const luaL_reg obj_LSockAddr_methods[] = {
  {"set_ip_port", LSockAddr__set_ip_port__meth},
  {"set_unix", LSockAddr__set_unix__meth},
  {"resize", LSockAddr__resize__meth},
  {"get_family", LSockAddr__get_family__meth},
  {"addr", LSockAddr__addr__meth},
  {"addrlen", LSockAddr__addrlen__meth},
  {NULL, NULL}
};

static const luaL_reg obj_LSockAddr_metas[] = {
  {"__gc", LSockAddr__delete__meth},
  {"__tostring", obj_simple_udata_default_tostring},
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

static const luaL_reg obj_Options_pub_funcs[] = {
  {NULL, NULL}
};

static const obj_const obj_Options_constants[] = {
#ifdef IPV6_IPSEC_POLICY
  {"IPV6_IPSEC_POLICY", NULL, IPV6_IPSEC_POLICY, CONST_NUMBER},
#endif
#ifdef IPV6_2292PKTINFO
  {"IPV6_2292PKTINFO", NULL, IPV6_2292PKTINFO, CONST_NUMBER},
#endif
#ifdef IP_TOS
  {"IP_TOS", NULL, IP_TOS, CONST_NUMBER},
#endif
#ifdef IPV6_DSTOPTS
  {"IPV6_DSTOPTS", NULL, IPV6_DSTOPTS, CONST_NUMBER},
#endif
#ifdef TCP_SYNCNT
  {"TCP_SYNCNT", NULL, TCP_SYNCNT, CONST_NUMBER},
#endif
#ifdef IPV6_2292RTHDR
  {"IPV6_2292RTHDR", NULL, IPV6_2292RTHDR, CONST_NUMBER},
#endif
#ifdef IP_HDRINCL
  {"IP_HDRINCL", NULL, IP_HDRINCL, CONST_NUMBER},
#endif
#ifdef IP_MULTICAST_IF
  {"IP_MULTICAST_IF", NULL, IP_MULTICAST_IF, CONST_NUMBER},
#endif
#ifdef IP_MTU_DISCOVER
  {"IP_MTU_DISCOVER", NULL, IP_MTU_DISCOVER, CONST_NUMBER},
#endif
#ifdef IPV6_MULTICAST_IF
  {"IPV6_MULTICAST_IF", NULL, IPV6_MULTICAST_IF, CONST_NUMBER},
#endif
#ifdef SOL_TCP
  {"SOL_TCP", NULL, SOL_TCP, CONST_NUMBER},
#endif
#ifdef IPV6_XFRM_POLICY
  {"IPV6_XFRM_POLICY", NULL, IPV6_XFRM_POLICY, CONST_NUMBER},
#endif
#ifdef IPV6_UNICAST_HOPS
  {"IPV6_UNICAST_HOPS", NULL, IPV6_UNICAST_HOPS, CONST_NUMBER},
#endif
#ifdef IP_DROP_SOURCE_MEMBERSHIP
  {"IP_DROP_SOURCE_MEMBERSHIP", NULL, IP_DROP_SOURCE_MEMBERSHIP, CONST_NUMBER},
#endif
#ifdef IP_BLOCK_SOURCE
  {"IP_BLOCK_SOURCE", NULL, IP_BLOCK_SOURCE, CONST_NUMBER},
#endif
#ifdef IP_PMTUDISC_DO
  {"IP_PMTUDISC_DO", NULL, IP_PMTUDISC_DO, CONST_NUMBER},
#endif
#ifdef IPV6_RECVPKTINFO
  {"IPV6_RECVPKTINFO", NULL, IPV6_RECVPKTINFO, CONST_NUMBER},
#endif
#ifdef IPV6_HOPLIMIT
  {"IPV6_HOPLIMIT", NULL, IPV6_HOPLIMIT, CONST_NUMBER},
#endif
#ifdef IPV6_ADDRFORM
  {"IPV6_ADDRFORM", NULL, IPV6_ADDRFORM, CONST_NUMBER},
#endif
#ifdef IPV6_V6ONLY
  {"IPV6_V6ONLY", NULL, IPV6_V6ONLY, CONST_NUMBER},
#endif
#ifdef SO_RCVTIMEO
  {"SO_RCVTIMEO", NULL, SO_RCVTIMEO, CONST_NUMBER},
#endif
#ifdef SO_TIMESTAMPNS
  {"SO_TIMESTAMPNS", NULL, SO_TIMESTAMPNS, CONST_NUMBER},
#endif
#ifdef SO_ATTACH
  {"SO_ATTACH", NULL, SO_ATTACH, CONST_NUMBER},
#endif
#ifdef IP_PKTOPTIONS
  {"IP_PKTOPTIONS", NULL, IP_PKTOPTIONS, CONST_NUMBER},
#endif
#ifdef IPV6_DROP_MEMBERSHIP
  {"IPV6_DROP_MEMBERSHIP", NULL, IPV6_DROP_MEMBERSHIP, CONST_NUMBER},
#endif
#ifdef SO_TYPE
  {"SO_TYPE", NULL, SO_TYPE, CONST_NUMBER},
#endif
#ifdef SO_SNDTIMEO
  {"SO_SNDTIMEO", NULL, SO_SNDTIMEO, CONST_NUMBER},
#endif
#ifdef SOL_UDP
  {"SOL_UDP", NULL, SOL_UDP, CONST_NUMBER},
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
#ifdef IPV6_HOPOPTS
  {"IPV6_HOPOPTS", NULL, IPV6_HOPOPTS, CONST_NUMBER},
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
#ifdef IP_XFRM_POLICY
  {"IP_XFRM_POLICY", NULL, IP_XFRM_POLICY, CONST_NUMBER},
#endif
#ifdef IPV6_ADD_MEMBERSHIP
  {"IPV6_ADD_MEMBERSHIP", NULL, IPV6_ADD_MEMBERSHIP, CONST_NUMBER},
#endif
#ifdef SO_PASSSEC
  {"SO_PASSSEC", NULL, SO_PASSSEC, CONST_NUMBER},
#endif
#ifdef TCP_WINDOW_CLAMP
  {"TCP_WINDOW_CLAMP", NULL, TCP_WINDOW_CLAMP, CONST_NUMBER},
#endif
#ifdef IP_IPSEC_POLICY
  {"IP_IPSEC_POLICY", NULL, IP_IPSEC_POLICY, CONST_NUMBER},
#endif
#ifdef IPV6_RECVHOPOPTS
  {"IPV6_RECVHOPOPTS", NULL, IPV6_RECVHOPOPTS, CONST_NUMBER},
#endif
#ifdef IPV6_RECVRTHDR
  {"IPV6_RECVRTHDR", NULL, IPV6_RECVRTHDR, CONST_NUMBER},
#endif
#ifdef IPV6_RECVTCLASS
  {"IPV6_RECVTCLASS", NULL, IPV6_RECVTCLASS, CONST_NUMBER},
#endif
#ifdef IP_MAX_MEMBERSHIPS
  {"IP_MAX_MEMBERSHIPS", NULL, IP_MAX_MEMBERSHIPS, CONST_NUMBER},
#endif
#ifdef SOL_ICMPV6
  {"SOL_ICMPV6", NULL, SOL_ICMPV6, CONST_NUMBER},
#endif
#ifdef SO_SECURITY_ENCRYPTION_NETWORK
  {"SO_SECURITY_ENCRYPTION_NETWORK", NULL, SO_SECURITY_ENCRYPTION_NETWORK, CONST_NUMBER},
#endif
#ifdef IPV6_2292HOPLIMIT
  {"IPV6_2292HOPLIMIT", NULL, IPV6_2292HOPLIMIT, CONST_NUMBER},
#endif
#ifdef IP_ADD_MEMBERSHIP
  {"IP_ADD_MEMBERSHIP", NULL, IP_ADD_MEMBERSHIP, CONST_NUMBER},
#endif
#ifdef SOL_IPV6
  {"SOL_IPV6", NULL, SOL_IPV6, CONST_NUMBER},
#endif
#ifdef IP_DEFAULT_MULTICAST_LOOP
  {"IP_DEFAULT_MULTICAST_LOOP", NULL, IP_DEFAULT_MULTICAST_LOOP, CONST_NUMBER},
#endif
#ifdef TCP_INFO
  {"TCP_INFO", NULL, TCP_INFO, CONST_NUMBER},
#endif
#ifdef IPV6_2292PKTOPTIONS
  {"IPV6_2292PKTOPTIONS", NULL, IPV6_2292PKTOPTIONS, CONST_NUMBER},
#endif
#ifdef IP_PMTUDISC_PROBE
  {"IP_PMTUDISC_PROBE", NULL, IP_PMTUDISC_PROBE, CONST_NUMBER},
#endif
#ifdef IP_ORIGDSTADDR
  {"IP_ORIGDSTADDR", NULL, IP_ORIGDSTADDR, CONST_NUMBER},
#endif
#ifdef SO_BROADCAST
  {"SO_BROADCAST", NULL, SO_BROADCAST, CONST_NUMBER},
#endif
#ifdef SO_DEBUG
  {"SO_DEBUG", NULL, SO_DEBUG, CONST_NUMBER},
#endif
#ifdef IPV6_ROUTER_ALERT
  {"IPV6_ROUTER_ALERT", NULL, IPV6_ROUTER_ALERT, CONST_NUMBER},
#endif
#ifdef IP_PMTUDISC
  {"IP_PMTUDISC", NULL, IP_PMTUDISC, CONST_NUMBER},
#endif
#ifdef IPV6_PKTINFO
  {"IPV6_PKTINFO", NULL, IPV6_PKTINFO, CONST_NUMBER},
#endif
#ifdef SO_SNDBUF
  {"SO_SNDBUF", NULL, SO_SNDBUF, CONST_NUMBER},
#endif
#ifdef IP_PMTUDISC_WANT
  {"IP_PMTUDISC_WANT", NULL, IP_PMTUDISC_WANT, CONST_NUMBER},
#endif
#ifdef SO_MARK
  {"SO_MARK", NULL, SO_MARK, CONST_NUMBER},
#endif
#ifdef SO_SECURITY_ENCRYPTION_TRANSPORT
  {"SO_SECURITY_ENCRYPTION_TRANSPORT", NULL, SO_SECURITY_ENCRYPTION_TRANSPORT, CONST_NUMBER},
#endif
#ifdef IP_MINTTL
  {"IP_MINTTL", NULL, IP_MINTTL, CONST_NUMBER},
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
#ifdef IP_FREEBIND
  {"IP_FREEBIND", NULL, IP_FREEBIND, CONST_NUMBER},
#endif
#ifdef IPV6_MTU_DISCOVER
  {"IPV6_MTU_DISCOVER", NULL, IPV6_MTU_DISCOVER, CONST_NUMBER},
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
#ifdef IP_MSFILTER
  {"IP_MSFILTER", NULL, IP_MSFILTER, CONST_NUMBER},
#endif
#ifdef IPV6_MTU
  {"IPV6_MTU", NULL, IPV6_MTU, CONST_NUMBER},
#endif
#ifdef IP_TTL
  {"IP_TTL", NULL, IP_TTL, CONST_NUMBER},
#endif
#ifdef IPV6_2292HOPOPTS
  {"IPV6_2292HOPOPTS", NULL, IPV6_2292HOPOPTS, CONST_NUMBER},
#endif
#ifdef IPV6_2292DSTOPTS
  {"IPV6_2292DSTOPTS", NULL, IPV6_2292DSTOPTS, CONST_NUMBER},
#endif
#ifdef SOL_SOCKET
  {"SOL_SOCKET", NULL, SOL_SOCKET, CONST_NUMBER},
#endif
#ifdef TCP_KEEPIDLE
  {"TCP_KEEPIDLE", NULL, TCP_KEEPIDLE, CONST_NUMBER},
#endif
#ifdef IP_PASSSEC
  {"IP_PASSSEC", NULL, IP_PASSSEC, CONST_NUMBER},
#endif
#ifdef IPV6_RTHDRDSTOPTS
  {"IPV6_RTHDRDSTOPTS", NULL, IPV6_RTHDRDSTOPTS, CONST_NUMBER},
#endif
#ifdef SO_OOBINLINE
  {"SO_OOBINLINE", NULL, SO_OOBINLINE, CONST_NUMBER},
#endif
#ifdef IPV6_JOIN_GROUP
  {"IPV6_JOIN_GROUP", NULL, IPV6_JOIN_GROUP, CONST_NUMBER},
#endif
#ifdef IP_RECVORIGDSTADDR
  {"IP_RECVORIGDSTADDR", NULL, IP_RECVORIGDSTADDR, CONST_NUMBER},
#endif
#ifdef TCP_MD5SIG
  {"TCP_MD5SIG", NULL, TCP_MD5SIG, CONST_NUMBER},
#endif
#ifdef TCP_CONGESTION
  {"TCP_CONGESTION", NULL, TCP_CONGESTION, CONST_NUMBER},
#endif
#ifdef SO_ACCEPTCONN
  {"SO_ACCEPTCONN", NULL, SO_ACCEPTCONN, CONST_NUMBER},
#endif
#ifdef TCP_QUICKACK
  {"TCP_QUICKACK", NULL, TCP_QUICKACK, CONST_NUMBER},
#endif
#ifdef TCP_DEFER_ACCEPT
  {"TCP_DEFER_ACCEPT", NULL, TCP_DEFER_ACCEPT, CONST_NUMBER},
#endif
#ifdef TCP_LINGER2
  {"TCP_LINGER2", NULL, TCP_LINGER2, CONST_NUMBER},
#endif
#ifdef TCP_KEEPCNT
  {"TCP_KEEPCNT", NULL, TCP_KEEPCNT, CONST_NUMBER},
#endif
#ifdef TCP_KEEPINTVL
  {"TCP_KEEPINTVL", NULL, TCP_KEEPINTVL, CONST_NUMBER},
#endif
#ifdef SO_BINDTODEVICE
  {"SO_BINDTODEVICE", NULL, SO_BINDTODEVICE, CONST_NUMBER},
#endif
#ifdef IP_PKTINFO
  {"IP_PKTINFO", NULL, IP_PKTINFO, CONST_NUMBER},
#endif
#ifdef TCP_MAXSEG
  {"TCP_MAXSEG", NULL, TCP_MAXSEG, CONST_NUMBER},
#endif
#ifdef IP_DEFAULT_MULTICAST_TTL
  {"IP_DEFAULT_MULTICAST_TTL", NULL, IP_DEFAULT_MULTICAST_TTL, CONST_NUMBER},
#endif
#ifdef TCP_NODELAY
  {"TCP_NODELAY", NULL, TCP_NODELAY, CONST_NUMBER},
#endif
#ifdef IPV6_RTHDR_TYPE_0
  {"IPV6_RTHDR_TYPE_0", NULL, IPV6_RTHDR_TYPE_0, CONST_NUMBER},
#endif
#ifdef IPV6_RTHDR_STRICT
  {"IPV6_RTHDR_STRICT", NULL, IPV6_RTHDR_STRICT, CONST_NUMBER},
#endif
#ifdef IPV6_RTHDR_LOOSE
  {"IPV6_RTHDR_LOOSE", NULL, IPV6_RTHDR_LOOSE, CONST_NUMBER},
#endif
#ifdef SO_DONTROUTE
  {"SO_DONTROUTE", NULL, SO_DONTROUTE, CONST_NUMBER},
#endif
#ifdef SO_RCVLOWAT
  {"SO_RCVLOWAT", NULL, SO_RCVLOWAT, CONST_NUMBER},
#endif
#ifdef IPV6_PMTUDISC_PROBE
  {"IPV6_PMTUDISC_PROBE", NULL, IPV6_PMTUDISC_PROBE, CONST_NUMBER},
#endif
#ifdef IPV6_PMTUDISC_DO
  {"IPV6_PMTUDISC_DO", NULL, IPV6_PMTUDISC_DO, CONST_NUMBER},
#endif
#ifdef IPV6_PMTUDISC_WANT
  {"IPV6_PMTUDISC_WANT", NULL, IPV6_PMTUDISC_WANT, CONST_NUMBER},
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
#ifdef IPV6_PMTUDISC_DONT
  {"IPV6_PMTUDISC_DONT", NULL, IPV6_PMTUDISC_DONT, CONST_NUMBER},
#endif
#ifdef IP_RECVERR
  {"IP_RECVERR", NULL, IP_RECVERR, CONST_NUMBER},
#endif
#ifdef IPV6_LEAVE_GROUP
  {"IPV6_LEAVE_GROUP", NULL, IPV6_LEAVE_GROUP, CONST_NUMBER},
#endif
#ifdef SO_LINGER
  {"SO_LINGER", NULL, SO_LINGER, CONST_NUMBER},
#endif
#ifdef IPV6_JOIN_ANYCAST
  {"IPV6_JOIN_ANYCAST", NULL, IPV6_JOIN_ANYCAST, CONST_NUMBER},
#endif
#ifdef SO_NO
  {"SO_NO", NULL, SO_NO, CONST_NUMBER},
#endif
#ifdef SO_SECURITY_AUTHENTICATION
  {"SO_SECURITY_AUTHENTICATION", NULL, SO_SECURITY_AUTHENTICATION, CONST_NUMBER},
#endif
#ifdef SO_PRIORITY
  {"SO_PRIORITY", NULL, SO_PRIORITY, CONST_NUMBER},
#endif
#ifdef IPV6_RXHOPOPTS
  {"IPV6_RXHOPOPTS", NULL, IPV6_RXHOPOPTS, CONST_NUMBER},
#endif
#ifdef IPV6_NEXTHOP
  {"IPV6_NEXTHOP", NULL, IPV6_NEXTHOP, CONST_NUMBER},
#endif
#ifdef SO_DETACH
  {"SO_DETACH", NULL, SO_DETACH, CONST_NUMBER},
#endif
#ifdef IP_PMTUDISC_DONT
  {"IP_PMTUDISC_DONT", NULL, IP_PMTUDISC_DONT, CONST_NUMBER},
#endif
#ifdef IPV6_TCLASS
  {"IPV6_TCLASS", NULL, IPV6_TCLASS, CONST_NUMBER},
#endif
#ifdef IPV6_RECVDSTOPTS
  {"IPV6_RECVDSTOPTS", NULL, IPV6_RECVDSTOPTS, CONST_NUMBER},
#endif
#ifdef IP_DROP_MEMBERSHIP
  {"IP_DROP_MEMBERSHIP", NULL, IP_DROP_MEMBERSHIP, CONST_NUMBER},
#endif
#ifdef IPV6_RECVHOPLIMIT
  {"IPV6_RECVHOPLIMIT", NULL, IPV6_RECVHOPLIMIT, CONST_NUMBER},
#endif
#ifdef IP_MULTICAST_LOOP
  {"IP_MULTICAST_LOOP", NULL, IP_MULTICAST_LOOP, CONST_NUMBER},
#endif
#ifdef IP_ADD_SOURCE_MEMBERSHIP
  {"IP_ADD_SOURCE_MEMBERSHIP", NULL, IP_ADD_SOURCE_MEMBERSHIP, CONST_NUMBER},
#endif
#ifdef IP_TRANSPARENT
  {"IP_TRANSPARENT", NULL, IP_TRANSPARENT, CONST_NUMBER},
#endif
#ifdef IP_MTU
  {"IP_MTU", NULL, IP_MTU, CONST_NUMBER},
#endif
#ifdef SO_KEEPALIVE
  {"SO_KEEPALIVE", NULL, SO_KEEPALIVE, CONST_NUMBER},
#endif
#ifdef IPV6_RTHDR
  {"IPV6_RTHDR", NULL, IPV6_RTHDR, CONST_NUMBER},
#endif
#ifdef SO_RXQ
  {"SO_RXQ", NULL, SO_RXQ, CONST_NUMBER},
#endif
#ifdef IP_RECVTTL
  {"IP_RECVTTL", NULL, IP_RECVTTL, CONST_NUMBER},
#endif
#ifdef SO_PEERNAME
  {"SO_PEERNAME", NULL, SO_PEERNAME, CONST_NUMBER},
#endif
#ifdef IP_MULTICAST_TTL
  {"IP_MULTICAST_TTL", NULL, IP_MULTICAST_TTL, CONST_NUMBER},
#endif
#ifdef TCP_CORK
  {"TCP_CORK", NULL, TCP_CORK, CONST_NUMBER},
#endif
#ifdef IPV6_LEAVE_ANYCAST
  {"IPV6_LEAVE_ANYCAST", NULL, IPV6_LEAVE_ANYCAST, CONST_NUMBER},
#endif
#ifdef SO_RCVBUF
  {"SO_RCVBUF", NULL, SO_RCVBUF, CONST_NUMBER},
#endif
#ifdef IPV6_RXDSTOPTS
  {"IPV6_RXDSTOPTS", NULL, IPV6_RXDSTOPTS, CONST_NUMBER},
#endif
#ifdef IPV6_AUTHHDR
  {"IPV6_AUTHHDR", NULL, IPV6_AUTHHDR, CONST_NUMBER},
#endif
#ifdef IP_RETOPTS
  {"IP_RETOPTS", NULL, IP_RETOPTS, CONST_NUMBER},
#endif
#ifdef SO_PEERSEC
  {"SO_PEERSEC", NULL, SO_PEERSEC, CONST_NUMBER},
#endif
#ifdef SOL_IP
  {"SOL_IP", NULL, SOL_IP, CONST_NUMBER},
#endif
#ifdef IP_ROUTER_ALERT
  {"IP_ROUTER_ALERT", NULL, IP_ROUTER_ALERT, CONST_NUMBER},
#endif
#ifdef IP_UNBLOCK_SOURCE
  {"IP_UNBLOCK_SOURCE", NULL, IP_UNBLOCK_SOURCE, CONST_NUMBER},
#endif
#ifdef IP_RECVRETOPTS
  {"IP_RECVRETOPTS", NULL, IP_RECVRETOPTS, CONST_NUMBER},
#endif
#ifdef SO_PASSCRED
  {"SO_PASSCRED", NULL, SO_PASSCRED, CONST_NUMBER},
#endif
#ifdef SOL_UNIX
  {"SOL_UNIX", NULL, SOL_UNIX, CONST_NUMBER},
#endif
#ifdef SO_ERROR
  {"SO_ERROR", NULL, SO_ERROR, CONST_NUMBER},
#endif
#ifdef SO_PEERCRED
  {"SO_PEERCRED", NULL, SO_PEERCRED, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const luaL_reg obj_LSocketFD_pub_funcs[] = {
  {"new", LSocketFD__new__meth},
  {"fd", LSocketFD__fd__meth},
  {NULL, NULL}
};

static const luaL_reg obj_LSocketFD_methods[] = {
  {"close", LSocketFD__close__meth},
  {"shutdown", LSocketFD__shutdown__meth},
  {"fileno", LSocketFD__fileno__meth},
  {"set_nonblock", LSocketFD__set_nonblock__meth},
  {"set_option", LSocketFD__set_option__meth},
  {"set_int_option", LSocketFD__set_int_option__meth},
  {"get_int_option", LSocketFD__get_int_option__meth},
  {"connect", LSocketFD__connect__meth},
  {"bind", LSocketFD__bind__meth},
  {"listen", LSocketFD__listen__meth},
  {"accept", LSocketFD__accept__meth},
  {"send", LSocketFD__send__meth},
  {"recv", LSocketFD__recv__meth},
  {NULL, NULL}
};

static const luaL_reg obj_LSocketFD_metas[] = {
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

static const luaL_reg llnet_function[] = {
  {NULL, NULL}
};

static const obj_const llnet_constants[] = {
#ifdef SHUT_RDWR
  {"SHUT_RDWR", NULL, SHUT_RDWR, CONST_NUMBER},
#endif
#ifdef SHUT_WR
  {"SHUT_WR", NULL, SHUT_WR, CONST_NUMBER},
#endif
#ifdef AF_INET6
  {"AF_INET6", NULL, AF_INET6, CONST_NUMBER},
#endif
#ifdef SOCK_STREAM
  {"SOCK_STREAM", NULL, SOCK_STREAM, CONST_NUMBER},
#endif
#ifdef SOCK_CLOEXEC
  {"SOCK_CLOEXEC", NULL, SOCK_CLOEXEC, CONST_NUMBER},
#endif
#ifdef SOCK_NONBLOCK
  {"SOCK_NONBLOCK", NULL, SOCK_NONBLOCK, CONST_NUMBER},
#endif
#ifdef AF_IPX
  {"AF_IPX", NULL, AF_IPX, CONST_NUMBER},
#endif
#ifdef SOCK_DGRAM
  {"SOCK_DGRAM", NULL, SOCK_DGRAM, CONST_NUMBER},
#endif
#ifdef AF_NETLINK
  {"AF_NETLINK", NULL, AF_NETLINK, CONST_NUMBER},
#endif
#ifdef AF_PACKET
  {"AF_PACKET", NULL, AF_PACKET, CONST_NUMBER},
#endif
#ifdef SOCK_RAW
  {"SOCK_RAW", NULL, SOCK_RAW, CONST_NUMBER},
#endif
#ifdef AF_INET
  {"AF_INET", NULL, AF_INET, CONST_NUMBER},
#endif
#ifdef SOCK_RDM
  {"SOCK_RDM", NULL, SOCK_RDM, CONST_NUMBER},
#endif
#ifdef SOCK_SEQPACKET
  {"SOCK_SEQPACKET", NULL, SOCK_SEQPACKET, CONST_NUMBER},
#endif
#ifdef SHUT_RD
  {"SHUT_RD", NULL, SHUT_RD, CONST_NUMBER},
#endif
#ifdef AF_UNIX
  {"AF_UNIX", NULL, AF_UNIX, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_Errors), REG_META, obj_Errors_pub_funcs, obj_Errors_methods, obj_Errors_metas, NULL, NULL, obj_Errors_constants, true},
  { &(obj_type_Protocols), REG_META, obj_Protocols_pub_funcs, obj_Protocols_methods, obj_Protocols_metas, NULL, NULL, obj_Protocols_constants, false},
  { &(obj_type_Services), REG_META, obj_Services_pub_funcs, obj_Services_methods, obj_Services_metas, NULL, NULL, obj_Services_constants, false},
  { &(obj_type_LSockAddr), REG_OBJECT, obj_LSockAddr_pub_funcs, obj_LSockAddr_methods, obj_LSockAddr_metas, obj_LSockAddr_bases, obj_LSockAddr_fields, obj_LSockAddr_constants, false},
  { &(obj_type_Options), REG_PACKAGE, obj_Options_pub_funcs, NULL, NULL, NULL, NULL, obj_Options_constants, true},
  { &(obj_type_LSocketFD), REG_OBJECT, obj_LSocketFD_pub_funcs, obj_LSocketFD_methods, obj_LSocketFD_metas, obj_LSocketFD_bases, obj_LSocketFD_fields, obj_LSocketFD_constants, false},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, false}
};





static const ffi_export_symbol llnet_ffi_export[] = {
  {NULL, NULL}
};




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

	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
	luaL_register(L, "llnet", llnet_function);

	/* register module constants. */
	obj_type_register_constants(L, llnet_constants, -1, false);

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
		nobj_try_loading_ffi(L, "llnet", llnet_ffi_lua_code,
			llnet_ffi_export, priv_table);
	}
#endif

	/* Cache reference to llnet.Errors table for errno->string convertion. */
	lua_pushlightuserdata(L, llnet_Errors_key);
	lua_getfield(L, -2, "Errors");
	lua_rawset(L, LUA_REGISTRYINDEX);



	return 1;
}



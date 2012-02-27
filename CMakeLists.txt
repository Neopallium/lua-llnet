#
# A low-level network/sockets library for Lua.
#
cmake_minimum_required(VERSION 2.8)

project(lua-llnet C)

set(BUILD_SHARED_LIBS TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(INSTALL_CMOD ${CMAKE_INSTALL_PREFIX}/lib/lua/5.1 CACHE PATH
				"Directory to install Lua binary modules (configure lua via LUA_CPATH)")
set(LUA_NATIVE_OBJECTS_PATH ../LuaNativeObjects CACHE PATH
				"Directory to LuaNativeObjects bindings generator.")
set(USE_PRE_GENERATED_BINDINGS TRUE CACHE BOOL
				"Set this to FALSE to re-generate bindings using LuaNativeObjects")

set(COMMON_CFLAGS "${CFLAGS}")
set(COMMON_LDFLAGS)
set(COMMON_LIBS)

include(CheckCSourceCompiles)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(FindPkgConfig)
include(CustomMacros)

## Check for reentrant functions.
check_c_source_compiles("
	#define _BSD_SOURCE 1
	#include <netdb.h>
	int main() {
		return getservbyname_r(0, 0, 0, 0, 0, 0);
	}" HAVE_getservbyname_r)
check_c_source_compiles("
	#define _BSD_SOURCE 1
	#include <netdb.h>
	int main() {
		return getservbyport_r(0, 0, 0, 0, 0, 0);
	}" HAVE_getservbyport_r)
check_c_source_compiles("
	#define _BSD_SOURCE 1
	#include <netdb.h>
	int main() {
		return getprotobyname_r(0, 0, 0, 0, 0);
	}" HAVE_getprotobyname_r)
check_c_source_compiles("
	#define _BSD_SOURCE 1
	#include <netdb.h>
	int main() {
		return getprotobynumber_r(0, 0, 0, 0, 0);
	}" HAVE_getprotobynumber_r)

## check for system features.
if(MSVC)
	set(CMAKE_REQUIRED_LIBRARIES ws2_32.lib)
endif()
check_c_source_compiles("
	#ifdef _MSC_VER
	#define __WINDOWS__
	#else
	#if defined(_WIN32)
	#define __WINDOWS__
	#endif
	#endif

	#ifdef __WINDOWS__
	#include <winsock2.h>
	#include <in6addr.h>
	#include <Ws2ipdef.h>
	#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#endif
	int main() {
		struct sockaddr_in6 addr;
		int fd;
		addr.sin6_family = AF_INET6;
		addr.sin6_addr = in6addr_any;
		fd = socket(AF_INET6, SOCK_STREAM, 0);
		return bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in6));
	}" HAVE_IPV6)

## Lua 5.1.x
include(FindLua51)
if(NOT ${LUA51_FOUND})
    message(FATAL_ERROR "The FindLua51 module could not find lua :-(")
endif()
if(WIN32)
	set(COMMON_CFLAGS "${COMMON_CFLAGS} -I${LUA_INCLUDE_DIR}")
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} ${LUA_LIBRARY}")
endif()
## MAC OSX needs extra linker flags
if(APPLE)
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} -undefined dynamic_lookup")
endif()


## LuaNativeObjects
include(LuaNativeObjects)

## generate config.h
configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake
				${CMAKE_BINARY_DIR}/build/config.h)
if(MSVC)
	set(COMMON_CFLAGS "${COMMON_CFLAGS} -FI ${CMAKE_BINARY_DIR}/build/config.h")
else()
	set(COMMON_CFLAGS "${COMMON_CFLAGS} -include ${CMAKE_BINARY_DIR}/build/config.h")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${LUA_INCLUDE_DIR})

## LuaBuffer
set(LUA_LLNET_NOBJ_SRC
	llnet.nobj.lua
)
set(LUA_LLNET_SRC
	src/lsockaddr.c
	src/lsockaddr.h
	src/lsocket.c
	src/lsocket.h
)

if(${USE_PRE_GENERATED_BINDINGS})
	set(LUA_LLNET_NOBJ_SRC src/pre_generated-llnet.nobj.c)
else()
	# Generate Lua bindings.
	GenLuaNativeObjects(LUA_LLNET_NOBJ_SRC)
endif()
set(LUA_LLNET_SRC ${LUA_LLNET_SRC} ${LUA_LLNET_NOBJ_SRC})

add_library(lua-llnet MODULE ${LUA_LLNET_SRC})
target_link_libraries(lua-llnet ${COMMON_LIBS})
add_target_properties(lua-llnet COMPILE_FLAGS "${COMMON_CFLAGS}")
add_target_properties(lua-llnet LINK_FLAGS "${LD_FLAGS} ${COMMON_LDFLAGS}")
set_target_properties(lua-llnet PROPERTIES PREFIX "")
set_target_properties(lua-llnet PROPERTIES OUTPUT_NAME llnet)

install(TARGETS lua-llnet
        DESTINATION "${INSTALL_CMOD}")


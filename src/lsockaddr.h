/***************************************************************************
 *   Copyright (C) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>     *
 *                                                                         *
 ***************************************************************************/
#if !defined(__L_SOCKADDR_H__)
#define __L_SOCKADDR_H__

#include "lcommon.h"

#ifdef __WINDOWS__
#include <ws2tcpip.h>
typedef unsigned short int sa_family_t;
#else
#include <sys/socket.h>
#endif

#define L_SOCKADDR_MAX_LEN 256

struct LSockAddr {
	struct sockaddr  *addr;
	socklen_t        addrlen;
};

typedef struct LSockAddr LSockAddr;

#define MAKE_TEMP_ADDR(name) \
	char tmp_buf_ ## name [L_SOCKADDR_MAX_LEN]; \
	socklen_t tmp_buf_len_ ## name = L_SOCKADDR_MAX_LEN

#define GET_TEMP_ADDR_AND_PTR_LEN(name) \
	(struct sockaddr *)(tmp_buf_ ## name), &(tmp_buf_len_ ## name)

#define L_SOCKADDR_FILL_FROM_TEMP(addr, name) \
	l_sockaddr_fill((addr), (struct sockaddr *)(tmp_buf_ ## name), tmp_buf_len_ ## name)

#define L_SOCKADDR_TO_ADDR_AND_LEN(addr) \
	(addr)->addr, (addr)->addrlen

#define L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr) \
	(addr)->addr, (addr)->addrlen

L_LIB_API int l_sockaddr_init(LSockAddr *addr);

L_LIB_API int l_sockaddr_set_ip_port(LSockAddr *addr, const char *ip, int port);

L_LIB_API int l_sockaddr_set_unix(LSockAddr *addr, const char *path);

L_LIB_API int l_sockaddr_set_family(LSockAddr *addr, sa_family_t family);

L_LIB_API void l_sockaddr_clear(LSockAddr *addr);

L_LIB_API void l_sockaddr_cleanup(LSockAddr *addr);

L_LIB_API void l_sockaddr_resize(LSockAddr *addr, socklen_t addrlen);

L_LIB_API void l_sockaddr_fill(LSockAddr *addr, const struct sockaddr *saddr, socklen_t addrlen);

L_LIB_API sa_family_t l_sockaddr_get_family(LSockAddr *addr);

L_LIB_API int l_sockaddr_set_port(LSockAddr *addr, int port);

L_LIB_API int l_sockaddr_get_port(LSockAddr *addr);

L_LIB_API struct sockaddr *l_sockaddr_get_addr(LSockAddr *addr);

L_LIB_API socklen_t l_sockaddr_get_addrlen(LSockAddr *addr);

L_LIB_API int l_sockaddr_tostring(LSockAddr *addr, char *buf, size_t buf_len);

L_LIB_API int l_sockaddr_lookup_full(LSockAddr *addr, const char *node, const char *service,
	int ai_family, int ai_socktype, int ai_protocol, int ai_flags);

#endif /* __L_SOCKADDR_H__ */

/***************************************************************************
 *   Copyright (C) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>     *
 *                                                                         *
 ***************************************************************************/
#if !defined(__L_SOCKADDR_H__)
#define __L_SOCKADDR_H__

#include "lcommon.h"
#include <sys/socket.h>

#define L_SOCKADDR_MAX_LEN 256

struct LSockAddr {
	struct sockaddr  *addr;
	socklen_t        addrlen;
};

typedef struct LSockAddr LSockAddr;

#define L_SOCKADDR_TO_ADDR_AND_LEN(addr) \
	(addr)->addr, (addr)->addrlen

#define L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr) \
	(addr)->addr, (addr)->addrlen

L_LIB_API int l_sockaddr_init(LSockAddr *addr);

L_LIB_API int l_sockaddr_set_ip_port(LSockAddr *addr, const char *ip, int port);

L_LIB_API int l_sockaddr_set_unix(LSockAddr *addr, const char *path);

L_LIB_API int l_sockaddr_set_family(LSockAddr *addr, sa_family_t family);

L_LIB_API void l_sockaddr_clear(LSockAddr *addr);

L_LIB_API void l_sockaddr_resize(LSockAddr *addr, socklen_t addrlen);

L_LIB_API void l_sockaddr_fill(LSockAddr *addr, const struct sockaddr *saddr, socklen_t addrlen);

L_LIB_API sa_family_t l_sockaddr_get_family(LSockAddr *addr);

L_LIB_API struct sockaddr *l_sockaddr_get_addr(LSockAddr *addr);

L_LIB_API socklen_t l_sockaddr_get_addrlen(LSockAddr *addr);

#endif /* __L_SOCKADDR_H__ */

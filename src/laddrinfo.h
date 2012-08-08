/***************************************************************************
 *   Copyright (C) 2012 by Robert G. Jakabosky <bobby@sharedrealm.com>     *
 *                                                                         *
 ***************************************************************************/
#if !defined(__L_ADDRINFO_H__)
#define __L_ADDRINFO_H__

#include "lcommon.h"

#include "lsockaddr.h"

struct LAddrInfo {
	struct addrinfo  *res;
	struct addrinfo  *cur;
};

typedef struct LAddrInfo LAddrInfo;

L_LIB_API int l_addrinfo_init_full(LAddrInfo *info, const char *node, const char *service,
	int ai_family, int ai_socktype, int ai_protocol, int ai_flags);

L_LIB_API int l_addrinfo_init_ip(LAddrInfo *info, const char *host, const char *port);

L_LIB_API int l_addrinfo_init_ipv4(LAddrInfo *info, const char *host, const char *port);

L_LIB_API int l_addrinfo_init_ipv6(LAddrInfo *info, const char *host, const char *port);

L_LIB_API void l_addrinfo_cleanup(LAddrInfo *info);

L_LIB_API bool l_addrinfo_first(LAddrInfo *info);

L_LIB_API int l_addrinfo_get_addr(LAddrInfo *info, LSockAddr *addr);

L_LIB_API const char *l_addrinfo_get_canonname(LAddrInfo *info);

L_LIB_API int l_addrinfo_get_family(LAddrInfo *info);

L_LIB_API int l_addrinfo_get_socktype(LAddrInfo *info);

L_LIB_API int l_addrinfo_get_protocol(LAddrInfo *info);

L_LIB_API bool l_addrinfo_next(LAddrInfo *info);

#endif /* __L_ADDRINFO_H__ */

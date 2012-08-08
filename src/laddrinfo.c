/***************************************************************************
 *   Copyright (C) 2012 by Robert G. Jakabosky <bobby@sharedrealm.com>     *
 *                                                                         *
 ***************************************************************************/

#include "laddrinfo.h"

#ifdef __WINDOWS__
#include <winsock2.h>
#include <windows.h>
#ifdef HAVE_IPV6
#include <in6addr.h>
#endif
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <string.h>

int l_addrinfo_init_full(LAddrInfo *info, const char *node, const char *service,
	int ai_family, int ai_socktype, int ai_protocol, int ai_flags)
{
	struct addrinfo hints;
	int rc;

	/* clear info. */
	info->res = NULL;
	info->cur = NULL;

	/* fill hints. */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = ai_family;
	hints.ai_socktype = ai_socktype;
	hints.ai_protocol = ai_protocol;
	hints.ai_flags = ai_flags;

	rc = getaddrinfo(node, service, &hints, &(info->res));
	if(rc == 0) {
		/* start at first results. */
		info->cur = info->res;
	} else {
		/* error. */
		info->res = NULL;
	}

	return rc;
}

int l_addrinfo_init_ip(LAddrInfo *info, const char *host, const char *port) {
	return l_addrinfo_init_full(info, host, port, AF_UNSPEC, SOCK_STREAM, 0, 0);
}

int l_addrinfo_init_ipv4(LAddrInfo *info, const char *host, const char *port) {
	return l_addrinfo_init_full(info, host, port, AF_INET, SOCK_STREAM, 0, 0);
}

int l_addrinfo_init_ipv6(LAddrInfo *info, const char *host, const char *port) {
	return l_addrinfo_init_full(info, host, port, AF_INET6, SOCK_STREAM, 0, 0);
}

void l_addrinfo_cleanup(LAddrInfo *info) {
	if(info->res) {
		freeaddrinfo(info->res);
		info->res = NULL;
		info->cur = NULL;
	}
}

bool l_addrinfo_first(LAddrInfo *info) {
	if(info->res) {
		info->cur = info->res;
		return true;
	}
	/* no address info available. */
	return false;
}

int l_addrinfo_get_addr(LAddrInfo *info, LSockAddr *addr) {
	struct addrinfo *cur = info->cur;
	if(!cur) return -1;
	l_sockaddr_fill(addr, cur->ai_addr, cur->ai_addrlen);
	return 0;
}

const char *l_addrinfo_get_canonname(LAddrInfo *info) {
	struct addrinfo *cur = info->cur;
	if(!cur) return NULL;
	return cur->ai_canonname;
}

int l_addrinfo_get_family(LAddrInfo *info) {
	struct addrinfo *cur = info->cur;
	if(!cur) return -1;
	return cur->ai_family;
}

int l_addrinfo_get_socktype(LAddrInfo *info) {
	struct addrinfo *cur = info->cur;
	if(!cur) return -1;
	return cur->ai_socktype;
}

int l_addrinfo_get_protocol(LAddrInfo *info) {
	struct addrinfo *cur = info->cur;
	if(!cur) return -1;
	return cur->ai_protocol;
}

bool l_addrinfo_next(LAddrInfo *info) {
	struct addrinfo *cur = info->cur;
	if(cur) {
		/* get next address. */
		cur = cur->ai_next;
		info->cur = cur;
		if(cur) {
			return true;
		}
	}
	/* at end. or list is empty */
	return false;
}


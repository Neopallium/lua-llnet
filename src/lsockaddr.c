/***************************************************************************
 *   Copyright (C) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>     *
 *                                                                         *
 ***************************************************************************/

#include "lsockaddr.h"

#ifdef __WINDOWS__
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef HAVE_IPV6
#include <in6addr.h>
#endif
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdlib.h>

static int l_sockaddr_set_internal(LSockAddr *addr, sa_family_t family, socklen_t addrlen) {
	if(addrlen < sizeof(struct sockaddr)) {
		addrlen = sizeof(struct sockaddr);
	}
	l_sockaddr_resize(addr, addrlen);
	addr->addr->sa_family = family;

	return 0;
}

int l_sockaddr_init(LSockAddr *addr) {
	addr->addr = NULL;
	addr->addrlen = 0;

	return l_sockaddr_set_internal(addr, PF_UNSPEC, 0);
}

int l_sockaddr_set_ip_port(LSockAddr *addr, const char *ip, int port) {
#ifdef HAVE_IPV6
	unsigned char buf[sizeof(struct in6_addr)];
#else
	unsigned char buf[sizeof(struct in_addr)];
#endif
	sa_family_t family;
	int rc;

	/* try ipv4. */
	family = PF_INET;
	rc = inet_pton(family, ip, buf);
	if(rc <= 0) {
#ifdef HAVE_IPV6
		/* try ipv6. */
		family = PF_INET6;
		rc = inet_pton(family, ip, buf);
#endif
		if(rc == 0) {
			return -1;
		} else if(rc < 0) {
			perror("l_sockaddr_set_ip_port(): inet_pton failed");
			return -1;
		}
	}
	port = htons(port);
	/* create sockaddr */
	switch(family) {
	case PF_INET: {
		struct sockaddr_in *addr_in;
		l_sockaddr_set_internal(addr, family, sizeof(*addr_in));
		addr_in = (struct sockaddr_in *)addr->addr;
		addr_in->sin_port = port;
		memcpy(&(addr_in->sin_addr), buf, sizeof(struct in_addr));
		break;
	}
#ifdef HAVE_IPV6
	case PF_INET6: {
		struct sockaddr_in6 *addr_in;
		l_sockaddr_set_internal(addr, family, sizeof(*addr_in));
		addr_in = (struct sockaddr_in6 *)addr->addr;
		addr_in->sin6_port = port;
		addr_in->sin6_flowinfo = 0;
		memcpy(&(addr_in->sin6_addr), buf, sizeof(struct in6_addr));
		addr_in->sin6_scope_id = 0;
		break;
	}
#endif
	default:
		break;
	}

	return 0;
}

int l_sockaddr_set_unix(LSockAddr *addr, const char *path) {
#ifdef __WINDOWS__
	return -1;
#else
	struct sockaddr_un *addr_un;

	l_sockaddr_set_internal(addr, PF_LOCAL, sizeof(struct sockaddr_un));
	addr_un = (struct sockaddr_un *)addr->addr;
	strncpy(addr_un->sun_path, path, sizeof(addr_un->sun_path));

	return 0;
#endif
}

int l_sockaddr_set_family(LSockAddr *addr, sa_family_t family) {
	socklen_t addrlen = sizeof(struct sockaddr);

	/* resize to minimal family size. */
	switch(family) {
#ifndef __WINDOWS__
	/* local/file/unix */
	case PF_LOCAL:
		addrlen = sizeof(struct sockaddr_un);
		break;
#endif
	case PF_INET:
		addrlen = sizeof(struct sockaddr_in);
		break;
	case PF_INET6:
		addrlen = sizeof(struct sockaddr_in6);
		break;
	default:
		break;
	}

	return l_sockaddr_set_internal(addr, family, addrlen);
}

int l_sockaddr_set_len(LSockAddr *addr, socklen_t addrlen) {
	return l_sockaddr_set_internal(addr, PF_UNSPEC, addrlen);
	return 0;
}

void l_sockaddr_cleanup(LSockAddr *addr) {
	if(addr->addr != NULL) {
		free(addr->addr);
		addr->addr = NULL;
		addr->addrlen = 0;
	}
}

void l_sockaddr_resize(LSockAddr *addr, socklen_t addrlen) {
	addr->addr = (struct sockaddr *)realloc(addr->addr, addrlen);
	addr->addrlen = addrlen;
}

void l_sockaddr_fill(LSockAddr *addr, const struct sockaddr *saddr, socklen_t addrlen) {
	l_sockaddr_resize(addr, addrlen);
	memcpy(addr->addr, saddr, addrlen);
}

sa_family_t l_sockaddr_get_family(LSockAddr *addr) {
	return addr->addr->sa_family;
}

struct sockaddr *l_sockaddr_get_addr(LSockAddr *addr) {
	return addr->addr;
}

socklen_t l_sockaddr_get_addrlen(LSockAddr *addr) {
	return addr->addrlen;
}


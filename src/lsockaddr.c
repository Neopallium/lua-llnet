/***************************************************************************
 *   Copyright (C) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>     *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "lsockaddr.h"

static int l_sockaddr_init_internal(LSockAddr *addr, sa_family_t family, socklen_t addrlen) {
	addr->addr = NULL;
	addr->addrlen = 0;

	if(addrlen < sizeof(struct sockaddr)) {
		addrlen = sizeof(struct sockaddr);
	}
	l_sockaddr_resize(addr, addrlen);
	/* set family type */
	addr->addr->sa_family = family;

	return 0;
}

int l_sockaddr_init_ip_port(LSockAddr *addr, const char *ip, int port) {
	unsigned char buf[sizeof(struct in6_addr)];
	sa_family_t family;
	int rc;

	/* try ipv4. */
	family = PF_INET;
	rc = inet_pton(family, ip, buf);
	if(rc <= 0) {
		/* try ipv6. */
		family = PF_INET6;
		rc = inet_pton(family, ip, buf);
		if(rc == 0) {
			return -1;
		} else if(rc < 0) {
			perror("l_sockaddr_init_ip_port(): inet_pton failed");
			return -1;
		}
	}
	port = htons(port);
	/* create sockaddr */
	switch(family) {
	case PF_INET: {
		struct sockaddr_in *addr_in;
		l_sockaddr_init_internal(addr, family, sizeof(*addr_in));
		addr_in = (struct sockaddr_in *)addr->addr;
		addr_in->sin_port = port;
		memcpy(&(addr_in->sin_addr), buf, sizeof(struct in_addr));
		break;
	}
	case PF_INET6: {
		struct sockaddr_in6 *addr_in;
		l_sockaddr_init_internal(addr, family, sizeof(*addr_in));
		addr_in = (struct sockaddr_in6 *)addr->addr;
		addr_in->sin6_port = port;
		addr_in->sin6_flowinfo = 0;
		memcpy(&(addr_in->sin6_addr), buf, sizeof(struct in6_addr));
		addr_in->sin6_scope_id = 0;
		break;
	}
	default:
		break;
	}

	return 0;
}

int l_sockaddr_init_unix(LSockAddr *addr, const char *path) {
	struct sockaddr_un *addr_un;

	l_sockaddr_init_internal(addr, PF_LOCAL, sizeof(struct sockaddr_un));
	addr_un = (struct sockaddr_un *)addr->addr;
	strncpy(addr_un->sun_path, path, sizeof(addr_un->sun_path));

	return 0;
}

int l_sockaddr_init_family(LSockAddr *addr, sa_family_t family) {
	socklen_t addrlen = sizeof(struct sockaddr);

	/* resize to minimal family size. */
	switch(family) {
	/* local/file/unix */
	case PF_LOCAL:
		addrlen = sizeof(struct sockaddr_un);
		break;
	case PF_INET:
		addrlen = sizeof(struct sockaddr_in);
		break;
	case PF_INET6:
		addrlen = sizeof(struct sockaddr_in6);
		break;
	default:
		break;
	}

	return l_sockaddr_init_internal(addr, family, addrlen);
}

int l_sockaddr_init_len(LSockAddr *addr, socklen_t addrlen) {
	l_sockaddr_init_internal(addr, PF_UNSPEC, addrlen);

	l_sockaddr_resize(addr, addrlen);
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


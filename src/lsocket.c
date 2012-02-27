/***************************************************************************
 * Copyright (C) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>       *
 *                                                                         *
 ***************************************************************************/

#include "lsocket.h"
#ifdef __WINDOWS__
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

int l_socket_set_nonblock(LSocketFD sock, bool val) {
#ifdef __WINDOWS__
	unsigned long flag = val;
	return ioctlsocket(sock, FIONBIO, &flag);
#else
	int flags;
	flags = fcntl(sock, F_GETFL);
	if(flags < 0) return flags;
	if(val) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~(O_NONBLOCK);
	}
	return fcntl(sock, F_SETFL, flags);
#endif
}

int l_socket_set_close_on_exec(LSocketFD sock, bool val) {
#ifdef __WINDOWS__
	return 0;
#else
	int flags;
	flags = fcntl(sock, F_GETFD);
	if(flags < 0) return flags;
	if(val) {
		flags |= FD_CLOEXEC;
	} else {
		flags &= ~(FD_CLOEXEC);
	}
	return fcntl(sock, F_SETFD, flags);
#endif
}

int l_socket_get_option(LSocketFD sock, int level, int opt, void *val, socklen_t *len) {
	return getsockopt(sock, level, opt, val, len);
}

int l_socket_set_option(LSocketFD sock, int level, int opt, const void *val, socklen_t len) {
	return setsockopt(sock, level, opt, val, len);
}

int l_socket_get_int_option(LSocketFD sock, int level, int opt, int *val) {
	socklen_t len = sizeof(*val);
	return l_socket_get_option(sock, level, opt, val, &len);
}

int l_socket_set_int_option(LSocketFD sock, int level, int opt, int val) {
	return l_socket_set_option(sock, level, opt, &val, sizeof(val));
}

LSocketFD l_socket_open(int domain, int type, int protocol, int flags) {
	type |= flags;
	return socket(domain, type, protocol);
}

LSocketFD l_socket_close_internal(LSocketFD sock) {
#ifdef __WINDOWS__
	return closesocket(sock);
#else
	return close(sock);
#endif
}

int l_socket_shutdown(LSocketFD sock, int how) {
	return shutdown(sock, how);
}

int l_socket_connect(LSocketFD sock, LSockAddr *addr) {
	return connect(sock, L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr));
}

int l_socket_bind(LSocketFD sock, LSockAddr *addr) {
	return bind(sock, L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr));
}

int l_socket_listen(LSocketFD sock, int backlog) {
	return listen(sock, backlog);
}

LSocketFD l_socket_accept(LSocketFD sock, LSockAddr *peer, int flags) {
#ifdef __WINDOWS__
	LSocketFD rc;
	if(peer != NULL) {
		socklen_t peerlen = l_sockaddr_get_addrlen(peer);
		rc = accept(sock, l_sockaddr_get_addr(peer), &peerlen);
	} else {
		rc = accept(sock, NULL, NULL);
	}
	if(rc == INVALID_SOCKET) {
		return rc;
	}
	if((flags & SOCK_NONBLOCK) == SOCK_NONBLOCK) {
		l_socket_set_nonblock(rc, 1);
	}
	return rc;
#else
	if(peer != NULL) {
		socklen_t peerlen = l_sockaddr_get_addrlen(peer);
		return accept4(sock, l_sockaddr_get_addr(peer), &peerlen, flags);
	}
	return accept4(sock, NULL, NULL, flags);
#endif
}

int l_socket_send(LSocketFD sock, const void *buf, size_t len, int flags) {
	return send(sock, buf, len, flags);
}

int l_socket_recv(LSocketFD sock, void *buf, size_t len, int flags) {
	return recv(sock, buf, len, flags);
}


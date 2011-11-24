/***************************************************************************
 * Copyright (C) 2007-2010 by Robert G. Jakabosky <bobby@neoawareness.com> *
 *                                                                         *
 ***************************************************************************/

#include "lsocket.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

int l_socket_set_nonblock(LSocketFD sock, bool val) {
	int flags;
	flags = fcntl(sock, F_GETFL);
	if(flags < 0) return flags;
	if(val) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~(O_NONBLOCK);
	}
	return fcntl(sock, F_SETFL, flags);
}

int l_socket_set_close_on_exec(LSocketFD sock, bool val) {
	int flags;
	flags = fcntl(sock, F_GETFD);
	if(flags < 0) return flags;
	if(val) {
		flags |= FD_CLOEXEC;
	} else {
		flags &= ~(FD_CLOEXEC);
	}
	return fcntl(sock, F_SETFD, flags);
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

int l_socket_close_internal(LSocketFD sock) {
	return close(sock);
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
	if(peer != NULL) {
		socklen_t peerlen = l_sockaddr_get_addrlen(peer);
		return accept4(sock, l_sockaddr_get_addr(peer), &peerlen, flags);
	}
	return accept4(sock, NULL, NULL, flags);
}

int l_socket_send(LSocketFD sock, const void *buf, size_t len, int flags) {
	return send(sock, buf, len, flags);
}

int l_socket_recv(LSocketFD sock, void *buf, size_t len, int flags) {
	return recv(sock, buf, len, flags);
}


/***************************************************************************
 * Copyright (C) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>       *
 *                                                                         *
 ***************************************************************************/

#include "lsocket.h"
#ifdef __WINDOWS__
#include <winsock2.h>

#define SHUT_RD   SD_RECEIVE 
#define SHUT_WR   SD_SEND 
#define SHUT_RDWR SD_BOTH 

#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

int l_socket_set_nonblock(LSocket *sock, bool val) {
#ifdef __WINDOWS__
	unsigned long flag = val;
	return ioctlsocket(sock->fd, FIONBIO, &flag);
#else
	int flags;
	flags = fcntl(sock->fd, F_GETFL);
	if(flags < 0) return flags;
	if(val) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~(O_NONBLOCK);
	}
	return fcntl(sock->fd, F_SETFL, flags);
#endif
}

int l_socket_set_close_on_exec(LSocket *sock, bool val) {
#ifdef __WINDOWS__
	return 0;
#else
	int flags;
	flags = fcntl(sock->fd, F_GETFD);
	if(flags < 0) return flags;
	if(val) {
		flags |= FD_CLOEXEC;
	} else {
		flags &= ~(FD_CLOEXEC);
	}
	return fcntl(sock->fd, F_SETFD, flags);
#endif
}

int l_socket_get_option(LSocket *sock, int level, int opt, void *val, socklen_t *len) {
	return getsockopt(sock->fd, level, opt, val, len);
}

int l_socket_set_option(LSocket *sock, int level, int opt, const void *val, socklen_t len) {
	return setsockopt(sock->fd, level, opt, val, len);
}

int l_socket_get_int_option(LSocket *sock, int level, int opt, int *val) {
	socklen_t len = sizeof(*val);
	return l_socket_get_option(sock, level, opt, val, &len);
}

int l_socket_set_int_option(LSocket *sock, int level, int opt, int val) {
	return l_socket_set_option(sock, level, opt, &val, sizeof(val));
}

int l_socket_pair(int type, int flags, LSocket *sv[2]) {
	LSocketFD fds[2];
	int rc;

	type |= flags;
#ifdef __WINDOWS__
	/* TODO: use TCP sockets. */
	errno = EAFNOSUPPORT;
	rc = -1;
#else
	rc = socketpair(AF_UNIX, type, 0, fds);
	if(rc >= 0) {
		sv[0]->fd = fds[0];
		sv[1]->fd = fds[1];
	}
#endif
	return rc;
}

int l_socket_open(LSocket *sock, int domain, int type, int protocol, int flags) {
	int rc;
	type |= flags;
	rc = socket(domain, type, protocol);
	sock->fd = rc;
	return rc;
}

int l_socket_close(LSocket *sock) {
	int rc;
	if(sock->fd == INVALID_SOCKET) return 0;
#ifdef __WINDOWS__
	rc = closesocket(sock->fd);
#else
	rc = close(sock->fd);
#endif
	sock->fd = INVALID_SOCKET;
	return rc;
}

int l_socket_shutdown(LSocket *sock, int read, int write) {
	int how = 0;
	if(read) {
		how = SHUT_RD;
	}
	if(write) {
		how |= SHUT_WR;
	}
	return shutdown(sock->fd, how);
}

int l_socket_connect(LSocket *sock, LSockAddr *addr) {
	return connect(sock->fd, L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr));
}

int l_socket_bind(LSocket *sock, LSockAddr *addr) {
	return bind(sock->fd, L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr));
}

int l_socket_listen(LSocket *sock, int backlog) {
	return listen(sock->fd, backlog);
}

int l_socket_get_sockname(LSocket *sock, LSockAddr *addr) {
	MAKE_TEMP_ADDR(tmp1);
	int rc;

	rc = getsockname(sock->fd, GET_TEMP_ADDR_AND_PTR_LEN(tmp1));
	if(rc == 0) {
		L_SOCKADDR_FILL_FROM_TEMP(addr, tmp1);
	}
	return rc;
}

int l_socket_get_peername(LSocket *sock, LSockAddr *addr) {
	MAKE_TEMP_ADDR(tmp1);
	int rc;

	rc = getpeername(sock->fd, GET_TEMP_ADDR_AND_PTR_LEN(tmp1));
	if(rc == 0) {
		L_SOCKADDR_FILL_FROM_TEMP(addr, tmp1);
	}
	return rc;
}

int l_socket_accept(LSocket *sock, LSocket *client, LSockAddr *peer, int flags) {
	LSocketFD rc;
	MAKE_TEMP_ADDR(tmp1);
#ifdef __WINDOWS__
	if(peer != NULL) {
		rc = accept(sock->fd, GET_TEMP_ADDR_AND_PTR_LEN(tmp1));
		if(rc >= 0) {
			L_SOCKADDR_FILL_FROM_TEMP(peer, tmp1);
		}
	} else {
		rc = accept(sock->fd, NULL, NULL);
	}
	if(rc != INVALID_SOCKET && (flags & SOCK_NONBLOCK) == SOCK_NONBLOCK) {
		client->fd = rc;
		l_socket_set_nonblock(client, 1);
	}
#else
	if(peer != NULL) {
		rc = accept4(sock->fd, GET_TEMP_ADDR_AND_PTR_LEN(tmp1), flags);
		if(rc >= 0) {
			L_SOCKADDR_FILL_FROM_TEMP(peer, tmp1);
		}
	} else {
		rc = accept4(sock->fd, NULL, NULL, flags);
	}
#endif
	client->fd = rc;
	return rc;
}

int l_socket_send(LSocket *sock, const void *buf, size_t len, int flags) {
	return send(sock->fd, buf, len, flags);
}

int l_socket_recv(LSocket *sock, void *buf, size_t len, int flags) {
	return recv(sock->fd, buf, len, flags);
}


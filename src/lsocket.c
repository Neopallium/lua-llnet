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

#define l_return_on_error(func, rc, msg, ret) \
	if(rc < 0) { \
		perror("l_socket_" #func ": " msg); \
		return ret; \
	}

int l_socket_set_nonblock(LSocketFD sock, bool val) {
	int flags;
	if(l_socket_is_closed(sock)) {
		return 0;
	}
	flags = fcntl(sock, F_GETFL);
	l_return_on_error(set_nonblock, flags, "get flags", flags);
	if(val) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~(O_NONBLOCK);
	}
	flags = fcntl(sock, F_SETFL, flags);
	l_return_on_error(set_nonblock, flags, "set flags", flags);
	return flags;
}

int l_socket_set_close_on_exec(LSocketFD sock, bool val) {
	int flags;
	if(l_socket_is_closed(sock)) {
		return 0;
	}
	flags = fcntl(sock, F_GETFD);
	l_return_on_error(set_close_on_exec, flags, "get flags", flags);
	if(val) {
		flags |= FD_CLOEXEC;
	} else {
		flags &= ~(FD_CLOEXEC);
	}
	flags = fcntl(sock, F_SETFD, flags);
	l_return_on_error(set_close_on_exec, flags, "set flags", flags);
	return flags;
}

int l_socket_get_option(LSocketFD sock, int level, int opt, void *val, socklen_t *len) {
	int rc;
	if(l_socket_is_closed(sock)) return -1;
	rc = getsockopt(sock, level, opt, val, len);
	l_return_on_error(getsockopt, rc, "set socket option", rc);
	return rc;
}

int l_socket_set_option(LSocketFD sock, int level, int opt, const void *val, socklen_t len) {
	int rc;
	if(l_socket_is_closed(sock)) return -1;
	rc = setsockopt(sock, level, opt, val, len);
	l_return_on_error(setsockopt, rc, "set socket option", rc);
	return rc;
}

int l_socket_get_int_option(LSocketFD sock, int level, int opt, int *val) {
	socklen_t len = sizeof(*val);
	return l_socket_get_option(sock, level, opt, val, &len);
}

int l_socket_set_int_option(LSocketFD sock, int level, int opt, int val) {
	return l_socket_set_option(sock, level, opt, &val, sizeof(val));
}

LSocketFD l_socket_open(int domain, int type, int protocol, int flags) {
	int rc;
	type |= flags;
	rc = socket(domain, type, protocol);
	l_return_on_error(open, rc, "open socket", rc);
	return rc;
}

int l_socket_close_internal(LSocketFD sock) {
	int rc;
	if(l_socket_is_closed(sock)) return -1;
	rc = close(sock);
	l_return_on_error(close, rc, "closing socket", -1);
	return -1;
}

int l_socket_shutdown(LSocketFD sock, int how) {
	int rc;
	if(l_socket_is_closed(sock)) return -1;
	rc = shutdown(sock, how);
	l_return_on_error(connect, rc, "shutdown socket", rc);
	return rc;
}

int l_socket_connect(LSocketFD sock, LSockAddr *addr) {
	int rc;
	if(l_socket_is_closed(sock)) return -1;
	rc = connect(sock, L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr));
	l_return_on_error(connect, rc, "connect socket", rc);
	return rc;
}

int l_socket_bind(LSocketFD sock, LSockAddr *addr) {
	int rc;
	if(l_socket_is_closed(sock)) return -1;
	rc = bind(sock, L_SOCKADDR_TO_CONST_ADDR_AND_LEN(addr));
	l_return_on_error(bind, rc, "bind socket", rc);
	return rc;
}

int l_socket_listen(LSocketFD sock, int backlog) {
	int rc;
	if(l_socket_is_closed(sock)) return -1;
	rc = listen(sock, backlog);
	l_return_on_error(listen, rc, "start socket listening", rc);
	return rc;
}

LSocketFD l_socket_accept(LSocketFD sock, LSockAddr *peer, int flags) {
	int rc;
	if(l_socket_is_closed(sock)) return -1;

	if(peer != NULL) {
		socklen_t peerlen = l_sockaddr_get_addrlen(peer);
		rc = accept4(sock, l_sockaddr_get_addr(peer), &peerlen, flags);
	} else {
		rc = accept4(sock, NULL, NULL, flags);
	}
	if(rc < 0) {
		/* @todo add MUCH better error handling for all syscalls. */
		switch(errno) {
		case EAGAIN:
			return -1;
		default:
			l_return_on_error(accept, rc, "accepting socket", -1);
			break;
		}
	}
	return rc;
}

int l_socket_send(LSocketFD sock, const void *buf, size_t len, int flags) {
	return send(sock, buf, len, flags);
}

int l_socket_recv(LSocketFD sock, void *buf, size_t len, int flags) {
	return recv(sock, buf, len, flags);
}


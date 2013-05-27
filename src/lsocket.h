/***************************************************************************
 * Copyright (C) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>       *
 *                                                                         *
 ***************************************************************************/
#ifndef __L_SOCKET_H__
#define __L_SOCKET_H__

#include "lcommon.h"
#include "lsockaddr.h"

#ifdef __WINDOWS__
#include <winsock2.h>
#include <ws2def.h>
typedef SOCKET LSocketFD;
#define SOCK_NONBLOCK 1
#ifndef SOL_IP
#define SOL_IP IPPROTO_IP
#endif
#ifndef SOL_IPV6
#define SOL_IPV6 IPPROTO_IPV6
#endif
#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif
#ifndef SOL_UDP
#define SOL_UDP IPPROTO_UDP
#endif
#else
typedef int LSocketFD;
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#ifdef HAVE_IPV6
#include <netinet/ip6.h>
#endif
#include <netinet/tcp.h>
#include <netinet/udp.h>
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET ((LSocketFD) -1)
#endif

typedef struct {
	LSocketFD fd;
} LSocket;

L_LIB_API int l_socket_set_nonblock(LSocket *sock, bool val);

L_LIB_API int l_socket_set_close_on_exec(LSocket *sock, bool val);

L_LIB_API int l_socket_get_option(LSocket *sock, int level, int opt, void *val, socklen_t *len);

L_LIB_API int l_socket_set_option(LSocket *sock, int level, int opt, const void *val, socklen_t len);

L_LIB_API int l_socket_get_int_option(LSocket *sock, int level, int opt, int *val);

L_LIB_API int l_socket_set_int_option(LSocket *sock, int level, int opt, int val);

#define l_socket_is_closed(sock) ((sock) < 0)

L_LIB_API int l_socket_pair(int type, int flags, LSocket *sv[2]);

L_LIB_API int l_socket_open(LSocket *sock, int domain, int type, int protocol, int flags);

L_LIB_API int l_socket_close(LSocket *sock);

L_LIB_API int l_socket_shutdown(LSocket *sock, int read, int write);

L_LIB_API int l_socket_connect(LSocket *sock, LSockAddr *addr);

L_LIB_API int l_socket_bind(LSocket *sock, LSockAddr *addr);

L_LIB_API int l_socket_listen(LSocket *sock, int backlog);

L_LIB_API int l_socket_get_sockname(LSocket *sock, LSockAddr *addr);

L_LIB_API int l_socket_get_peername(LSocket *sock, LSockAddr *addr);

L_LIB_API int l_socket_accept(LSocket *sock, LSocket *client, LSockAddr *peer, int flags);

L_LIB_API int l_socket_send(LSocket *sock, const void *buf, size_t len, int flags);

L_LIB_API int l_socket_recv(LSocket *sock, void *buf, size_t len, int flags);

#endif /* __L_SOCKET_H__ */

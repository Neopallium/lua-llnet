/***************************************************************************
 * Copyright (C) 2007-2010 by Robert G. Jakabosky <bobby@neoawareness.com> *
 *                                                                         *
 ***************************************************************************/
#ifndef __L_SOCKET_H__
#define __L_SOCKET_H__

#include "lcommon.h"
#include "lsockaddr.h"

typedef int LSocketFD;

L_LIB_API int l_socket_set_nonblock(LSocketFD sock, bool val);

L_LIB_API int l_socket_set_close_on_exec(LSocketFD sock, bool val);

L_LIB_API int l_socket_get_option(LSocketFD sock, int level, int opt, void *val, socklen_t *len);

L_LIB_API int l_socket_set_option(LSocketFD sock, int level, int opt, const void *val, socklen_t len);

L_LIB_API int l_socket_get_int_option(LSocketFD sock, int level, int opt, int *val);

L_LIB_API int l_socket_set_int_option(LSocketFD sock, int level, int opt, int val);

#define l_socket_is_closed(sock) ((sock) < 0)

L_LIB_API LSocketFD l_socket_open(int domain, int type, int protocol, int flags);

L_LIB_API LSocketFD l_socket_close_internal(LSocketFD sock);
#define l_socket_close(sock) (sock) = l_socket_close_internal(sock)

L_LIB_API int l_socket_shutdown(LSocketFD sock, int how);

L_LIB_API int l_socket_connect(LSocketFD sock, LSockAddr *addr);

L_LIB_API int l_socket_bind(LSocketFD sock, LSockAddr *addr);

L_LIB_API int l_socket_listen(LSocketFD sock, int backlog);

L_LIB_API LSocketFD l_socket_accept(LSocketFD sock, LSockAddr *peer, int flags);

L_LIB_API int l_socket_send(LSocketFD sock, const void *buf, size_t len, int flags);

L_LIB_API int l_socket_recv(LSocketFD sock, void *buf, size_t len, int flags);

#endif /* __L_SOCKET_H__ */

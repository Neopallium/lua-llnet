About
=====

[![travis-ci status](https://secure.travis-ci.org/Neopallium/lua-llnet.png?branch=master)](http://travis-ci.org/Neopallium/lua-llnet/builds)

A low-level network/socket library.

TCP Tunning
===========
When running the client/server examples with a larger number of concurrent connections or with keep-alive disabled make sure to tune your system to get good results.

Linux
-----

	# somaxconn needs to be larger then the listen backlog value used by the server.
	# (see note below about listen backlog).
	echo 65536 > /proc/sys/net/core/somaxconn
	# These are useful when stress testing/benchmarking where a large number of connections
	# will be opened & closed quickly.
	echo 1 >/proc/sys/net/ipv4/tcp_tw_recycle
	echo 1 >/proc/sys/net/ipv4/tcp_tw_reuse
	echo 10 >/proc/sys/net/ipv4/tcp_fin_timeout
	# increase client-side port range, not needed on server.
	echo 1024 65535 > /proc/sys/net/ipv4/ip_local_port_range
	# If you have iptables setup with the connection tracking (nf_conntrack) module enabled
	# then also increase these values
	echo 262144 >/proc/sys/net/netfilter/nf_conntrack_max
	echo 32768 >/sys/module/nf_conntrack/parameters/hashsize

Note about listen backlog
-------------------------
If the backlog value passed to listen() is greater then the value in /proc/sys/net/core/somaxconn it will be silenty truncated to some very small value.  This goes against what the man-page for listen() says:

	If    the   backlog   argument   is   greater   than   the   value   in
	/proc/sys/net/core/somaxconn, then it is  silently  truncated  to  that
	value;  the  default  value  in  this  file  is 128.  In kernels before
	2.4.25, this limit was a hard coded value, SOMAXCONN,  with  the  value
	128.

Installation
============

Lastest Git Revision
--------------------

With CMake:

	$ git clone git://github.com/Neopallium/lua-llnet.git
	$ cd lua-llnet ; mkdir build ; cd build
	$ cmake ..
	$ make
	$ sudo make install


Example usage
=============
See the files under the examples/ folder



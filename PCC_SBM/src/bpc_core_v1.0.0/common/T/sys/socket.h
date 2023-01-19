#ifndef __T__sys__Socket_h__
#define __T__sys__Socket_h__

#include <cxxtest/Mock.h>
#include <sys/socket.h>

CXXTEST_MOCK_GLOBAL( int,
					 socket,
					 ( int domain, int type, int protocol ),
					 ( domain, type, protocol ) );

CXXTEST_MOCK_GLOBAL( int,
					 connect,
					 ( int sockfd, const struct sockaddr *addr, socklen_t addrlen ),
					 ( sockfd, addr, addrlen ) );

CXXTEST_MOCK_GLOBAL( int,
					 bind,
					 ( int sockfd, const struct sockaddr *addr, socklen_t addrlen ),
					 ( sockfd, addr, addrlen ) );

CXXTEST_MOCK_GLOBAL( int,
					 listen,
					 ( int sockfd, int backlog ),
					 ( sockfd, backlog ) );

CXXTEST_MOCK_GLOBAL( int,
					 accept,
					 ( int sockfd, struct sockaddr *addr, socklen_t *addrlen ),
					 ( sockfd, addr, addrlen ) );

CXXTEST_MOCK_GLOBAL( ssize_t,
					 send,
					 ( int sockfd, const void *buf, size_t len, int flags ),
					 ( sockfd, buf, len, flags ) );

CXXTEST_MOCK_GLOBAL( ssize_t,
					 recv,
					 ( int sockfd, void *buf, size_t len, int flags ),
					 ( sockfd, buf, len, flags ) );

CXXTEST_MOCK_GLOBAL( int,
                     setsockopt,
                     ( int sockfd, int level, int optname, const void *optval, socklen_t optlen ),
                     ( sockfd, level, optname, optval, optlen ) );

#endif // __T__sys__Socket_h__

//
// Local Variables:
// compile-command: "make -C ../../.. "
// tags-file-name: "../../../TAGS"
// c-basic-offset: 4
// End:
//

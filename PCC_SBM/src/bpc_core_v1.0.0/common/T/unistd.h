#ifndef __T__Unistd_h__
#define __T__Unistd_h__

#include <cxxtest/Mock.h>
#include <unistd.h>

CXXTEST_MOCK_GLOBAL( int,
					 close,
					 ( int fd ),
					 ( fd ) );

CXXTEST_MOCK_GLOBAL( pid_t,
					 fork,
					 (),
					 () );

CXXTEST_MOCK_GLOBAL( ssize_t,
                     write,
                     ( int fd, const void *buf, size_t count ),
                     ( fd, buf, count ) );

CXXTEST_MOCK_GLOBAL( ssize_t,
                     read,
                     ( int fd, void *buf, size_t count ),
                     ( fd, buf, count ) );

CXXTEST_MOCK_VOID_GLOBAL( _exit,
                          ( int status ),
                          ( status ) );

#endif // __T__Unistd_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

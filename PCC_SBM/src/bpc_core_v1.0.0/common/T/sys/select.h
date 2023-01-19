#ifndef __T__sys__Select_h__
#define __T__sys__Select_h__

#include <cxxtest/Mock.h>
#include <sys/select.h>

CXXTEST_MOCK_GLOBAL( int,        
					 select,     
					 ( int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ),
					 ( nfds, readfds, writefds, exceptfds, timeout  ) );

#endif // __T__sys__Select_h__

//
// Local Variables:
// compile-command: "make -C ../../.. "
// tags-file-name: "../../../TAGS"
// c-basic-offset: 4
// End:
//

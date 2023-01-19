#ifndef __T__sys__Timerfd_h__
#define __T__sys__Timerfd_h__

#include <cxxtest/Mock.h>

#include <sys/timerfd.h>

CXXTEST_MOCK_GLOBAL( int,
					 timerfd_create,
					 ( int clockid, int flags ),
					 ( clockid, flags  ) );

CXXTEST_MOCK_GLOBAL( int,
					 timerfd_settime,
					 ( int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value ),
					 ( fd, flags, new_value, old_value  ) );

#endif // __T__sys__Timerfd_h__

//
// Local Variables:
// compile-command: "make -C ../../.. "
// tags-file-name: "../../../TAGS"
// c-basic-offset: 4
// End:
//

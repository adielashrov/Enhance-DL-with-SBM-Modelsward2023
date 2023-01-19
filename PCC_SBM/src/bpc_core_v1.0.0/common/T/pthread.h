#ifndef __T__Pthread_h__
#define __T__Pthread_h__

#include <cxxtest/Mock.h>
#include <pthread.h>

CXXTEST_MOCK_GLOBAL( int,
                     pthread_create,
					 ( pthread_t *thread,
                       const pthread_attr_t *attr,
                       void *( *start_routine )( void* ),
                       void *arg ),
					 ( thread, attr, start_routine, arg ) );

#endif // __T__Pthread_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

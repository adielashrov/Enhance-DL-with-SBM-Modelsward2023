#ifndef __IThreaded_h__
#define __IThreaded_h__

#include "Error.h"
#include "T/pthread.h"
#include <cstdio>

class IThreaded
{
public:
    virtual void run()
    {
        const pthread_attr_t *DEFAULT_ATTRIBUTES = 0;
        pthread_t newThread;

        if ( T::pthread_create( &newThread, DEFAULT_ATTRIBUTES, IThreaded::entryPointCaller, this ) != 0 )
        {
            printf("IThreaded -> Failed to create thread\n");
            throw Error( Error::PTHREAD_CREATE_FAILED );
        }
    }

    static void *entryPointCaller( void *instance )
    {
        try
        {
            ((IThreaded *)instance)->entryPoint();
        }
        catch ( const Error &e )
        {
            if ( e.code() == Error::SELECTION_DEADLOCKED )
            {
                printf( "Program terminated due to deadlock\n" );
            }
            else
            {
                printf( "IThreaded: Thread crashed. Error: %u\n", e.code() );
            }

            fflush( 0 );
        }

        return 0;
    }

    virtual void entryPoint() = 0;

    virtual ~IThreaded() {}
};

#endif // __IThreaded_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __SayGoodEvening_h__
#define __SayGoodEvening_h__

#include "Event.h"
#include "BThread.h"

class SayGoodEvening : public BThread
{
public:

    SayGoodEvening() : BThread( "SayGoodEvening" )
    {
    }

    void entryPoint()
    {
        SayGoodEveningFlowAndWait();
    }

    void SayGoodEveningFlow()
    {
        printf("Enter SayGoodEveningFlow...\n");
        Vector<Event> requested;
        Vector<Event> watched;
        Vector<Event> blocked;

        Event goodEvening( IEvent::SAY_GOOD_EVENING );
        requested.append( goodEvening );

        bSync( requested, watched, blocked );
        printf( "Say Good Evening: #1\n" );

        bSync( requested, watched, blocked );
        printf( "Say Good Evening: #2\n" );

        bSync( requested, watched, blocked );
        printf( "Say Good Evening: #3\n" );

        done();
        printf( "Say Good Evening: done\n" );
    }


    void busyWaitAndHoldBackAllBThreads()
    {
        return;
        int duration = rand() % 3 + 1;
        sleep(duration);
    }

    void SayGoodEveningFlowAndWait()
    {
        printf("Enter SayGoodEveningFlow...\n");
        Vector<Event> requested;
        Vector<Event> watched;
        Vector<Event> blocked;

        Event goodEvening( IEvent::SAY_GOOD_EVENING );
        requested.append( goodEvening );

        busyWaitAndHoldBackAllBThreads();
        bSync( requested, watched, blocked );
        printf( "Say Good Evening: #1\n" );

        busyWaitAndHoldBackAllBThreads();
        bSync( requested, watched, blocked );
        printf( "Say Good Evening: #2\n" );

        busyWaitAndHoldBackAllBThreads();
        bSync( requested, watched, blocked );
        printf( "Say Good Evening: #3\n" );

        done();
        printf( "Say Good Evening: done\n" );
    }
};

#endif // __SayGoodEvening_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

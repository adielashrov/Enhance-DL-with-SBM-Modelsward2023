#ifndef __SayGoodMorning_h__
#define __SayGoodMorning_h__

#include "Event.h"
#include "BThread.h"
#include <stdlib.h>


class SayGoodMorning : public BThread
{
public:

    SayGoodMorning() : BThread( "SayGoodMorning" )
    {
    }

    void entryPoint()
    {
        SayGoodMorningFlowAndWait();
    }


    void busyWaitAndHoldBackAllBThreads()
    {
        return;
        int duration = rand() % 3 + 1;
        sleep(duration);
    }

    void SayGoodMorningFlowAndWait()
    {

        printf("Enter SayGoodMorningFlow...\n");
        Vector<Event> requested;
        Vector<Event> watched;
        Vector<Event> blocked;

        Event goodMorning( IEvent::SAY_GOOD_MORNING );
        requested.append( goodMorning );

        busyWaitAndHoldBackAllBThreads();
        bSync( requested, watched, blocked );
        printf( "Say Good Morning: #1\n" );

        busyWaitAndHoldBackAllBThreads();
        bSync( requested, watched, blocked );
        printf( "Say Good Morning: #2\n" );

        busyWaitAndHoldBackAllBThreads();
        bSync( requested, watched, blocked );
        printf( "Say Good Morning: #3\n" );

        done();
        printf( "Say Good Morning: done\n" );
    }


    void SayGoodMorningFlow()
    {
        printf("Enter SayGoodMorningFlow...\n");
        Vector<Event> requested;
        Vector<Event> watched;
        Vector<Event> blocked;

        Event goodMorning( IEvent::SAY_GOOD_MORNING );
        requested.append( goodMorning );

        bSync( requested, watched, blocked );
        printf( "Say Good Morning: #1\n" );

        bSync( requested, watched, blocked );
        printf( "Say Good Morning: #2\n" );

        bSync( requested, watched, blocked );
        printf( "Say Good Morning: #3\n" );

        done();
        printf( "Say Good Morning: done\n" );
    }
};

#endif // __SayGoodMorning_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __Interleaver_h__
#define __Interleaver_h__

#include "BThread.h"
#include "Event.h"
#include "IEvent.h"

class Interleaver : public BThread
{
public:

    Interleaver() : BThread( "Interleaver" )
    {
    }

    void entryPoint()
    {
        interleaveFlow();
    }

    void interleaveFlow()
    {
        printf("Enter interleaveFlow...\n");
        Vector<Event> requested;
        Vector<Event> watched;
        Vector<Event> blocked;

        Event goodMorning( IEvent::SAY_GOOD_MORNING );
        Event goodEvening( IEvent::SAY_GOOD_EVENING );

        while ( true )
        {
            watched.clear();
            blocked.clear();

            watched.append( goodEvening );
            blocked.append( goodMorning );
            bSync( requested, watched, blocked );

            watched.clear();
            blocked.clear();

            watched.append( goodMorning );
            blocked.append( goodEvening );

            bSync( requested, watched, blocked );
        }
    }
};

#endif // __Interleaver_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

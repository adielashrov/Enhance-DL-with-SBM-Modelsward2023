#ifndef __IEvent_h__
#define __IEvent_h__

class IEvent
{
public:
    enum EventType {
        SAY_GOOD_MORNING = 0,
        SAY_GOOD_EVENING = 1,

        NUMBER_OF_EVENTS = 2,
    };

    static Vector<Event> allEvents()
    {
        Vector<Event> result;

        for ( unsigned i = 0; i < NUMBER_OF_EVENTS; ++i )
        {
            result.append( Event( i ) );
        }

        return result;
    }
};

#endif // __IEvent_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

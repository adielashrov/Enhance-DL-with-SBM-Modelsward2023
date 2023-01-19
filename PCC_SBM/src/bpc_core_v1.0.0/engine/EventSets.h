#ifndef __EventSets_h__
#define __EventSets_h__

#include "Event.h"
#include "Vector.h"

class EventSets
{
public:
    EventSets()
    {
    }

    EventSets( const Vector<Event> &requested,
               const Vector<Event> &watched,
               const Vector<Event> &blocked ) :
        _requested( requested ), _watched( watched ), _blocked( blocked )
    {
    }

    EventSets &operator=( const EventSets &other )
    {
        _requested = other._requested;
        _watched = other._watched;
        _blocked = other._blocked;

        return *this;
    }

    Vector<Event> _requested;
    Vector<Event> _watched;
    Vector<Event> _blocked;
};

#endif // __EventSets_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

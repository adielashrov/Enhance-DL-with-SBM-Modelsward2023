#ifndef __Transition_h__
#define __Transition_h__

#include "Event.h"
#include "StringTag.h"

struct Transition
{
    Transition( Event event, StringTag state ) : _event( event ), _state( state )
    {
    }

    Transition()
    {
    }

    bool operator==( const Transition &other )
    {
        return ( _event.type() == other._event.type() ) && ( _state == other._state );
    }

    Event _event;
    StringTag _state;
};

#endif // __Transition_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

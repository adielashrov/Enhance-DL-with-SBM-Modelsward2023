#ifndef __ThreadState_h__
#define __ThreadState_h__

#include "AtomicPropositions.h"
#include "Event.h"
#include "String.h"
#include "Transition.h"
#include "UniqueVector.h"

struct ThreadState
{
    StringTag _label;
    Vector<Event> _history;
    Vector<Event> _requestedEvents;
    Vector<Event> _watchedEvents;
    Vector<Event> _blockedEvents;
    Vector<Transition> _successors;
    Vector<Transition> _predecessors;
    AtomicPropositions _atomicPropositions;

    void clear()
    {
        _label = "";

        _history.clear();
        _requestedEvents.clear();
        _watchedEvents.clear();
        _blockedEvents.clear();
        _successors.clear();
        _predecessors.clear();
    }

    StringTag labelOfSuccessor( Event event )
    {
        for ( unsigned i = 0; i < _successors.size(); ++i )
        {
            if ( _successors[i]._event == event )
            {
                return _successors[i]._state;
            }
        }

        return "";
    }

    Vector<StringTag> allSuccessors()
    {
        UniqueVector<StringTag> successors;

        for ( unsigned i = 0; i < _successors.size(); ++i )
        {
            successors.append( _successors[i]._state );
        }

        return successors;
    }

    Vector<StringTag> allPredecessors()
    {
        UniqueVector<StringTag> predecessors;

        for ( unsigned i = 0; i < _predecessors.size(); ++i )
        {
            predecessors.append( _predecessors[i]._state );
        }

        return predecessors;
    }

    void removeSuccessor( const StringTag &label )
    {
        unsigned i = 0;

        while ( i < _successors.size() )
        {
            if ( _successors[i]._state == label )
            {
                _successors.eraseAt( i );
            }
            else
            {
                ++i;
            }
        }
    }

    void removePredecessor( const StringTag &label )
    {
        unsigned i = 0;

        while ( i < _predecessors.size() )
        {
            if ( _predecessors[i]._state == label )
            {
                _predecessors.eraseAt( i );
            }
            else
            {
                ++i;
            }
        }
    }
};

#endif // __ThreadState_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

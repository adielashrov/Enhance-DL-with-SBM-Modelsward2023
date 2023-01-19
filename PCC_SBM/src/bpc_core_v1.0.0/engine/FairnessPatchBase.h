#ifndef __FairnessPatchBase_h__
#define __FairnessPatchBase_h__

#include "Map.h"
#include "StateCycle.h"
#include "BThread.h"
#include "IEvent.h"
#include "IEvent.h"
#include "StringTag.h"
#include "T/Truth.h"
#include "Transition.h"

class FairnessPatchBase : public BThread
{
public:
    FairnessPatchBase()
    {
        _watched = IEvent::allEvents();
    }

    void entryPoint()
    {
        findEnabledEventsAtTarget();

        while ( T::alwaysTrue() )
        {
            adjustBlockedEvents();
            bSync( _requested, _watched, _blocked, blocking() ? _currentState + "-- blocking" : _currentState );
            updateCurrentState();
            clearBlocked();
        }
    }

protected:
    typedef Vector<Transition> Transitions;
    Map<StringTag, Transitions> _stateToTransitions;
    StringTag _currentState;
    StringTag _targetState;

private:
    Vector<Event> _requested;
    Vector<Event> _watched;
    Vector<Event> _blocked;
    UniqueVector<Event> _enabledEventsAtTarget;

    void clearBlocked()
    {
        _blocked.clear();
    }

    bool blocking() const
    {
        return _blocked.size() > 0;
    }

    void findEnabledEventsAtTarget()
    {
        for ( unsigned i = 0; i < _stateToTransitions[_targetState].size(); ++i )
        {
            _enabledEventsAtTarget.append( _stateToTransitions[_targetState][i]._event );
        }
    }

    bool atTargetState()
    {
        return _currentState == _targetState;
    }

    void adjustBlockedEvents()
    {
        if ( atTargetState() )
        {
            if ( coinSaysBlock() )
            {
                printf( "\t\tCoins say block\n" );
                blockAllButRandomOne();
            }
        }
    }

    static bool coinSaysBlock()
    {
        static const unsigned ETA_NOMINATOR = 1;
        static const unsigned ETA_DENOMINATOR = 10;

        unsigned randomValue = rand() % ETA_DENOMINATOR;

        return randomValue < ETA_NOMINATOR;
    }

    void blockAllButRandomOne()
    {
        unsigned eventToAllow = rand() % _enabledEventsAtTarget.size();

        for ( unsigned i = 0; i < _enabledEventsAtTarget.size(); ++i )
        {
            if ( i != eventToAllow )
            {
                _blocked.append( _enabledEventsAtTarget[i] );
            }
        }
    }

    void updateCurrentState()
    {
        // printf( "** Updating state\n" );
        // printf( "\t\t Old state: %s\n", _currentState.ascii() );

        Transitions transitions = _stateToTransitions[_currentState];
        unsigned lastEventType = lastEvent().type();

        for ( unsigned i = 0; i < transitions.size(); ++i )
        {
            if ( transitions[i]._event.type() == lastEventType )
            {
                _currentState = transitions[i]._state;
                // printf( "\t\t New state: %s\n", _currentState.ascii() );
                return;
            }
        }

        trapMode();
    }

    void trapMode()
    {
        _watched.clear();
        _blocked.clear();

        while ( T::alwaysTrue() )
        {
            bSync( _requested, _watched, _blocked, "Disabled" );
        }
    }
};

#endif // __FairnessPatchBase_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

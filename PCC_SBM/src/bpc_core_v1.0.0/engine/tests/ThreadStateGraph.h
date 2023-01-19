#ifndef __ThreadStateGraph_h__
#define __ThreadStateGraph_h__

#include "Map.h"
#include "Queue.h"
#include "StringTag.h"
#include "ThreadState.h"

struct ThreadStateGraph
{
    Map<StringTag, ThreadState> _graph;
    Vector<Event> _events;
    StringTag _initialState;

    void clear()
    {
        _graph.clear();
        _events.clear();
        _initialState = "";
    }

    void applyAxioms()
    {
        removeNonRequested();
        eliminateNonReachables();
    }

    void assertNoDeadlocks()
    {
        Map<StringTag, ThreadState>::iterator it;

        for ( it = _graph.begin(); it != _graph.end(); ++it )
        {
            TS_ASSERT( !it->second._successors.empty() );
        }
    }

    void dump()
    {
        printf( "State graph size is: %u\n\n", _graph.size() );

        Map<StringTag, ThreadState>::iterator it;
        for ( it = _graph.begin(); it != _graph.end(); ++it )
        {
            printf( "%s:\n", it->second._label.ascii() );

            printf( "Requested: " );
            for ( unsigned i = 0; i < it->second._requestedEvents.size(); ++i )
            {
                printf( "%u ", it->second._requestedEvents[i].type() );
            }
            printf( "\n" );

            printf( "Watched: " );
            for ( unsigned i = 0; i < it->second._watchedEvents.size(); ++i )
            {
                printf( "%u ", it->second._watchedEvents[i].type() );
            }
            printf( "\n" );

            printf( "Blocked: " );
            for ( unsigned i = 0; i < it->second._blockedEvents.size(); ++i )
            {
                printf( "%u ", it->second._blockedEvents[i].type() );
            }
            printf( "\n" );

            printf( "History: " );
            for ( unsigned i = 0; i < it->second._history.size(); ++i )
            {
                printf( "%u ", it->second._history[i].type() );
            }
            printf( "\n" );

            printf( "Successors: " );
            for ( unsigned i = 0; i < it->second._successors.size(); ++i )
            {
                printf( "%u-->%s  ", it->second._successors[i]._event.type(), it->second._successors[i]._state.ascii() );
            }
            printf( "\n" );

            printf( "Predecessors: " );
            for ( unsigned i = 0; i < it->second._predecessors.size(); ++i )
            {
                printf( "%u-->%s  ", it->second._predecessors[i]._event.type(),
                        it->second._predecessors[i]._state.ascii() );
            }
            printf( "\n" );

            printf( "Atomic Propositions: " );
            for ( unsigned i = 0; i < it->second._atomicPropositions.size(); ++i )
            {
                printf( "%s ", it->second._atomicPropositions.at( i ).ascii() );
            }
            printf( "\n\n" );
        }
    }

    void eliminateNonReachables()
    {
        init();

        while ( moreStatesToCheck() )
        {
            checkAnotherState();
        }

        Vector<StringTag> toErase;

        Map<StringTag, ThreadState>::iterator it;

        for ( it = _graph.begin(); it != _graph.end(); ++it )
        {
            if ( !_visitedStates.exists( it->first ) )
            {
                toErase.append( it->first );
            }
        }

        for ( unsigned i = 0; i < toErase.size(); ++i )
        {
            removeState( toErase[i] );
        }
    }

private:
    Queue<StringTag> _statesToCheck;
    Map<StringTag, char> _visitedStates;

    void init()
    {
        _visitedStates.clear();
        _statesToCheck.clear();

        _visitedStates[_initialState] = 0;
        _statesToCheck.push( _initialState );
    }

    bool moreStatesToCheck() const
    {
        return !_statesToCheck.empty();
    }

    void checkAnotherState()
    {
        Vector<StringTag> successors = _graph[_statesToCheck.peak()].allSuccessors();
        _statesToCheck.pop();

        for ( unsigned i = 0; i < successors.size(); ++i )
        {
            if ( !_visitedStates.exists( successors[i] ) )
            {
                _visitedStates[successors[i]] = 0;
                _statesToCheck.push( successors[i] );
            }
        }
    }

    void addAllUnreachedStates()
    {
        Map<StringTag, ThreadState>::iterator it;

        for ( it = _graph.begin(); it != _graph.end(); ++it )
        {
            if ( !_visitedStates.exists( it->first ) )
            {
                _visitedStates[it->first] = 0;
            }
        }
    }

    void removeNonRequested()
    {
        Map<StringTag, ThreadState>::iterator it;

        for ( it = _graph.begin(); it != _graph.end(); ++it )
        {
            removeNonRequstedAtState( it->first );
        }
    }

    void removeNonRequstedAtState( const StringTag &label )
    {
        unsigned i = 0;

        while ( i < _graph[label]._successors.size() )
        {
            if ( !_graph[label]._requestedEvents.exists( _graph[label]._successors[i]._event ) )
            {
                removeTransition( label, i );
            }
            else
            {
                ++i;
            }
        }
    }

    void removeTransition( const StringTag &label, unsigned successorIndex )
    {
        StringTag successor = _graph[label]._successors[successorIndex]._state;

        Transition predecessor( _graph[label]._successors[successorIndex]._event, label );
        _graph[successor]._predecessors.erase( predecessor );

        _graph[label]._successors.eraseAt( successorIndex );
    }

    void removeState( const StringTag &label )
    {
        for ( unsigned i = 0; i < _graph[label]._successors.size(); ++i )
        {
            StringTag successor = _graph[label]._successors[i]._state;
            _graph[successor].removePredecessor( label );
        }

        for ( unsigned i = 0; i < _graph[label]._predecessors.size(); ++i )
        {
            StringTag predecessor = _graph[label]._predecessors[i]._state;
            _graph[predecessor].removeSuccessor( label );
        }

        _graph.erase( label );
    }
};

#endif // __ThreadStateGraph_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

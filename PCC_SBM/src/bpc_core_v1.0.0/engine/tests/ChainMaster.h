#ifndef __ChainMaster_h__
#define __ChainMaster_h__

#include "StateEnumerator.h"
#include "StringTag.h"
#include "ThreadStateGraph.h"

static const char *TRAP_STATE_LABEL = "TRAP";

class ChainMaster
{
public:
    Vector<ThreadStateGraph> createChain( ThreadStateGraph &fullGraph )
    {
        init( fullGraph );

        for ( unsigned i = 0; i < _fullGraph->_graph.size() - 1; ++i )
        {
            addIteration();
        }

        addFullGraphToChain();

        return _chain;
    }

private:
    ThreadStateGraph *_fullGraph;
    Vector<ThreadStateGraph> _chain;
    ThreadState _basicTrapState;
    Vector<StringTag> _stateEnumeration;

    void init( ThreadStateGraph &fullGraph )
    {
        _chain.clear();
        _basicTrapState.clear();
        _fullGraph = &fullGraph;
        _stateEnumeration = _fullGraph->bfsStateEnumeration();

        prepareTrapState();
    }

    void prepareTrapState()
    {
        _basicTrapState._label = TRAP_STATE_LABEL;

        for ( unsigned i = 0; i < _fullGraph->_events.size(); ++i )
        {
            addTrapSelfLoop( _fullGraph->_events[i] );
        }
    }

    void addTrapSelfLoop( Event event )
    {
        Transition selfLoop( event, _basicTrapState._label );
        _basicTrapState._successors.append( selfLoop );
        _basicTrapState._predecessors.append( selfLoop );
    }

    void addFullGraphToChain()
    {
        _chain.append( *_fullGraph );
    }

    void addIteration()
    {
        ThreadState originalStateToAdd = _fullGraph->_graph[_stateEnumeration.popFirst()];

        if ( firstIteration() )
        {
            addOneStateGraph( originalStateToAdd );
        }
        else
        {
            ThreadStateGraph previousGraph = _chain.last();
            addStateToGraph( previousGraph, originalStateToAdd );
            _chain.append( previousGraph );
        }
    }

    bool firstIteration() const
    {
        return _chain.empty();
    }

    void addOneStateGraph( ThreadState &originalState )
    {
        ThreadStateGraph graph;

        graph._events = _fullGraph->_events;
        graph._initialState = _fullGraph->_initialState;

        ThreadState state;
        ThreadState trapState = _basicTrapState;

        state._label = originalState._label;
        state._requestedEvents = originalState._requestedEvents;
        state._watchedEvents = originalState._watchedEvents;
        state._blockedEvents = originalState._blockedEvents;
        state._history = originalState._history;

        for ( unsigned i = 0; i < originalState._successors.size(); ++i )
        {
            if ( originalState._successors[i]._state == state._label )
            {
                state._successors.append( originalState._successors[i] );
                state._predecessors.append( originalState._successors[i] );
            }
            else
            {
                Transition trapTransition( originalState._successors[i]._event, TRAP_STATE_LABEL );
                state._successors.append( trapTransition );

                Transition reverseTrapTransition( originalState._successors[i]._event, state._label );
                trapState._predecessors.append( reverseTrapTransition );
            }
        }

        graph._graph[state._label] = state;
        graph._graph[TRAP_STATE_LABEL] = trapState;

        adjustTrapStateRequestedEvents( graph );

        _chain.append( graph );
    }

    void adjustTrapStateRequestedEvents( ThreadStateGraph &graph )
    {
        ThreadState *trapState = &graph._graph[TRAP_STATE_LABEL];
        trapState->_requestedEvents.clear();

        Map<StringTag, ThreadState>::iterator it;

        for ( it = _fullGraph->_graph.begin(); it != _fullGraph->_graph.end(); ++it )
        {
            StringTag label = it->first;

            if ( !graph._graph.exists( label ) )
            {
                trapState->_requestedEvents = trapState->_requestedEvents + it->second._requestedEvents;
                trapState->_requestedEvents.uniqueElements();
            }
        }
    }

    void addStateToGraph( ThreadStateGraph &graph, ThreadState &originalState )
    {
        ThreadState state;
        ThreadState *trapState = &graph._graph[TRAP_STATE_LABEL];

        state._label = originalState._label;
        state._requestedEvents = originalState._requestedEvents;
        state._watchedEvents = originalState._watchedEvents;
        state._blockedEvents = originalState._blockedEvents;
        state._history = originalState._history;

        for ( unsigned i = 0; i < originalState._successors.size(); ++i )
        {
            if ( originalState._successors[i]._state == state._label )
            {
                state._successors.append( originalState._successors[i] );
                state._predecessors.append( originalState._successors[i] );
            }
            else if ( graph._graph.exists( originalState._successors[i]._state ) )
            {
                state._successors.append( originalState._successors[i] );

                Transition reverseTransition( originalState._successors[i]._event, state._label );
                graph._graph[originalState._successors[i]._state]._predecessors.append( reverseTransition );
            }
            else
            {
                Transition trapTransition( originalState._successors[i]._event, TRAP_STATE_LABEL );
                state._successors.append( trapTransition );

                Transition reverseTrapTransition( originalState._successors[i]._event, state._label );
                trapState->_predecessors.append( reverseTrapTransition );
            }
        }

        for ( unsigned i = 0; i < originalState._predecessors.size(); ++i )
        {
            if ( graph._graph.exists( originalState._predecessors[i]._state ) )
            {
                state._predecessors.append( originalState._predecessors[i] );

                Transition falseTransition( originalState._predecessors[i]._event, TRAP_STATE_LABEL );
                graph._graph[originalState._predecessors[i]._state]._successors.erase( falseTransition );

                Transition correctedTransition( originalState._predecessors[i]._event, state._label );
                graph._graph[originalState._predecessors[i]._state]._successors.append( correctedTransition );

                Transition falseTrapTransition( originalState._predecessors[i]._event,
                                                originalState._predecessors[i]._state );
                trapState->_predecessors.erase( falseTrapTransition );
            }
        }

        graph._graph[state._label] = state;

        adjustTrapStateRequestedEvents( graph );
    }
};

#endif // __ChainMaster_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

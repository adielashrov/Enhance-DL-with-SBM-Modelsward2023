#ifndef __StateEnumerator_h__
#define __StateEnumerator_h__

#include "ThreadStateGraph.h"
#include "UniqueVector.h"

class StateEnumerator
{
public:
    Vector<StringTag> enumerate( ThreadStateGraph &graph )
    {
        TS_TRACE( "Special enumeration starting" );

        // TS_TRACE( "Dumping graph" );
        // graph.dump();

        storeGraph( graph );
        bfs();
        createSubgraphs();

        TS_TRACE( "Special enumeration done" );

        Vector<StringTag> bla;
        return bla;
    }

    void dump()
    {
        Map<Event, States>::iterator it;

        printf( "Total states in graph: %u\n\n", _graph->_graph.size() );

        for ( it = _eventToRequestingSubgraph.begin(); it != _eventToRequestingSubgraph.end(); ++it )
        {
            printf( "Event %u: %u states\n\n", it->first.type(), it->second.size() );
        }
    }

private:
    typedef UniqueVector<StringTag> States;

    ThreadStateGraph *_graph;
    Map<Event, States> _eventToRequestingSubgraph;
    Map<StringTag, States> _predecessors;

    UniqueVector<StringTag> _statesToGoOver;
    UniqueVector<StringTag> _statesAlreadyChecked;

    void storeGraph( ThreadStateGraph &graph )
    {
        _graph = &graph;
    }

    void bfs()
    {
        storeInitialState();

        while ( moreStatesToGoOver() )
        {
            goOverAnotherState();
        }

        _predecessors[_graph->_initialState] = States();

        printf( "Total reachable states: %u\n", _predecessors.size() );
    }

    void storeInitialState()
    {
        _statesToGoOver.clear();
        _statesAlreadyChecked.clear();

        _statesToGoOver.append( _graph->_initialState );
        _statesAlreadyChecked.append( _graph->_initialState );
    }

    bool moreStatesToGoOver() const
    {
        return !_statesToGoOver.empty();
    }

    void goOverAnotherState()
    {
        StringTag label = _statesToGoOver.popFirst();
        ThreadState *state = &_graph->_graph[label];

        Vector<StringTag> successors = state->allSuccessors();

        for ( unsigned i = 0; i < successors.size(); ++i )
        {
            StringTag successorLabel = successors[i];

            if ( !_statesAlreadyChecked.exists( successorLabel ) )
            {
                _statesToGoOver.append( successorLabel );
                _statesAlreadyChecked.append( successorLabel );
            }

            if ( !_predecessors.exists( successorLabel ) )
            {
                _predecessors[successorLabel] = States();
            }

            _predecessors[successorLabel].append( label );
        }
    }

    void createSubgraphs()
    {
        Map<StringTag, States>::iterator it;

        for ( it = _predecessors.begin(); it != _predecessors.end(); ++it )
        {
            processSingleState( it->first );
        }
    }

    bool noRequestedEventsInState( const StringTag &label ) const
    {
        return _graph->_graph[label]._requestedEvents.empty();
    }

    void processSingleState( StringTag label )
    {
        if ( noRequestedEventsInState( label ) )
        {
            return;
        }

        States predecessors = findAllPredecessors( label );

        for ( unsigned i = 0; i < _graph->_graph[label]._requestedEvents.size(); ++i )
        {
            Event requestedEvent = _graph->_graph[label]._requestedEvents[i];

            if ( _eventToRequestingSubgraph.exists( requestedEvent ) )
            {
                States states;
                _eventToRequestingSubgraph[requestedEvent] = states;
            }

            _eventToRequestingSubgraph[requestedEvent] += predecessors;
        }
    }

    States findAllPredecessors( StringTag label )
    {
        States predecessors;

        _statesToGoOver.clear();
        _statesAlreadyChecked.clear();

        _statesToGoOver.append( label );

        while ( !_statesToGoOver.empty() )
        {
            StringTag state = _statesToGoOver.popFirst();
            _statesAlreadyChecked.append( state );

            for ( unsigned i = 0; i < _predecessors[state].size(); ++i )
            {
                StringTag predecessor = _predecessors[state][i];

                if ( !_statesAlreadyChecked.exists( predecessor ) )
                {
                    _statesToGoOver.append( predecessor );
                }

                predecessors.append( predecessor );
            }
        }

        return predecessors;
    }
};

#endif // __StateEnumerator_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

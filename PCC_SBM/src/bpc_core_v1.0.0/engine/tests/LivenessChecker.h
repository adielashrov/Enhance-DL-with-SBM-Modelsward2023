#ifndef __LivenessChecker_h__
#define __LivenessChecker_h__

#include "StateCycle.h"
#include "TemperatureToken.h"

class LivenessChecker
{
public:

    bool check( ThreadStateGraph &graph, TemperatureToken &token )
    {
        storeInput( graph, token );
        findHotStates();
        findHotCycles();

        return !hotCyclesExist();
    }

    Vector<StateCycle> find( ThreadStateGraph &graph, TemperatureToken &token )
    {
        storeInput( graph, token );
        findHotStates();
        dumpHotStates();
        findHotCycles();

        return _hotCycles;
    }

private:
    TemperatureToken *_token;
    ThreadStateGraph *_graph;
    Vector<StringTag> _hotStates;
    Vector<StateCycle> _hotCycles;

    void storeInput( ThreadStateGraph &graph, TemperatureToken &token )
    {
        _graph = &graph;
        _token = &token;
    }

    void findHotStates()
    {
        Map<StringTag, ThreadState>::iterator iterator;

        for ( iterator = _graph->_graph.begin(); iterator != _graph->_graph.end(); ++iterator )
        {
            if ( _token->hot( iterator->second ) )
            {
                _hotStates.append( iterator->second._label );
            }
        }
    }

    void dumpHotStates()
    {
        printf( "Hot states are:\n" );
        for ( unsigned i = 0; i < _hotStates.size(); ++i )
        {
            printf( "%s\n", _hotStates[i].ascii() );
        }
    }

    bool hotCyclesExist() const
    {
        return _hotCycles.size() > 0;
    }

    void findHotCycles()
    {
        for ( unsigned i = 0; i < _hotStates.size(); ++i )
        {
            runDfsOnState( _hotStates[i] );
            extractHotCycles( _hotStates[i] );
        }
    }

    Vector<StringTag> _checked;
    Vector<StringTag> _toCheck;

    typedef UniqueVector<StringTag> Predecessors;
    Map<StringTag, Predecessors> _stateToPredecessors;

    void resetDfs()
    {
        _checked.clear();
        _toCheck.clear();
        _stateToPredecessors.clear();
    }

    void runDfsOnState( const StringTag &label )
    {
        resetDfs();
        _toCheck.append( label );
        _checked.append( label );

        Predecessors rootPredecessors;
        _stateToPredecessors[label] = rootPredecessors;

        while ( !_toCheck.empty() )
        {
            StringTag currentState = _toCheck.popFirst();
            Vector<StringTag> successors = _graph->_graph[currentState].allSuccessors();

            for ( unsigned i = 0; i < successors.size(); ++i )
            {
                if ( _token->hot( _graph->_graph[successors[i]] ) )
                {
                    if ( !_checked.exists( successors[i] ) )
                    {
                        _checked.append( successors[i] );
                        _toCheck.append( successors[i] );

                        Predecessors predecessors;
                        predecessors.append( currentState );
                        _stateToPredecessors[successors[i]] = predecessors;
                    }
                    else
                    {
                        _stateToPredecessors[successors[i]].append( currentState );
                    }
                }
            }
        }
    }

    void extractHotCycles( const StringTag &root )
    {
        // For now, assume only simple cycles, i.e. no parallelism or nesting etc

        StringTag currentState;
        StringTag predecessor;

        for ( unsigned i = 0; i < _stateToPredecessors[root].size(); ++i )
        {
            currentState = root;
            predecessor = _stateToPredecessors[root][i];

            StateCycle cycle;
            cycle.append( predecessor );

            while ( predecessor != root )
            {
                currentState = predecessor;
                predecessor = _stateToPredecessors[currentState][0];
                cycle.append( predecessor );
            }

            _hotCycles.append( cycle );
        }
    }
};

#endif // __LivenessChecker_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

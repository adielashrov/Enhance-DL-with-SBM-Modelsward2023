#ifndef __HotStateAnalyzer_h__
#define __HotStateAnalyzer_h__

#include "TemperatureToken.h"

class HotStateAnalyzer
{
public:

    void analyze( ThreadStateGraph &graph, TemperatureToken &token )
    {
        storeInput( graph, token );
        partitionHotAndCold();

        TS_TRACE( Stringf( "Number of hot states: %u", _hotStates.size() ).ascii() );

        classifyStates();

        TS_TRACE( Stringf( "Number of hot traps: %u", _hotTraps.size() ).ascii() );
        TS_TRACE( Stringf( "Number of hot escapables: %u", _hotEscapables.size() ).ascii() );

        // TS_TRACE( "Hot traps are:" );

        // Map<StringTag, char>::iterator it;

        // for ( it = _hotTraps.begin(); it != _hotTraps.end(); ++it )
        // {
        //     TS_TRACE( "State:" );

        //     TS_TRACE( it->first.ascii() );

        //     TS_TRACE( "Predecessors:" );

        //     Vector<Transition> &predecessors( _graph->_graph[it->first]._predecessors );

        //     for ( unsigned i = 0; i < predecessors.size(); ++i )
        //     {
        //         TS_TRACE( predecessors[i]._state.ascii() );
        //     }
        // }
    }

    // Vector<StringTag> getEscapables()
    // {
    //     return _hotEscapables;
    // }

    // Vector<StringTag> getTraps()
    // {
    //     return _hotTraps;
    // }

private:
    TemperatureToken *_token;
    ThreadStateGraph *_graph;
    Map<StringTag, char> _hotStates;
    Map<StringTag, char> _coldStates;
    Map<StringTag, char> _hotEscapables;
    Map<StringTag, char> _hotTraps;
    Map<StringTag, char> _checked;
    Queue<StringTag> _toCheck;
    Vector<StringTag> _newEscapables;

    void resetDfs( const StringTag &root )
    {
        _checked.clear();
        _toCheck.clear();

        _toCheck.push( root );
        _checked[root] = 0;
    }

    void storeInput( ThreadStateGraph &graph, TemperatureToken &token )
    {
        _graph = &graph;
        _token = &token;
    }

    void partitionHotAndCold()
    {
        Map<StringTag, ThreadState>::iterator iterator;

        for ( iterator = _graph->_graph.begin(); iterator != _graph->_graph.end(); ++iterator )
        {
            if ( hotState( iterator->first ) )
            {
                _hotStates[iterator->second._label] = 0;
            }
            else
            {
                _coldStates[iterator->second._label] = 0;
            }
        }
    }

    bool hotState( const StringTag &label ) const
    {
        return _token->hot( _graph->_graph[label] );
    }

    bool coldState( const StringTag &label ) const
    {
        return !hotState( label );
    }

    // void dumpHotStates()
    // {
    //     printf( "Hot states are:\n" );
    //     for ( unsigned i = 0; i < _hotStates.size(); ++i )
    //     {
    //         printf( "%s\n", _hotStates[i].ascii() );
    //     }
    // }

    void classifyStates()
    {
        _newEscapables.clear();

        TS_TRACE( "Performing first iteration" );

        startWithColdStates();

        while ( !_newEscapables.empty() )
        {
            TS_TRACE( "\t Another iteration" );
            findNewEscapables();
        }

        markTraps();
    }

    void startWithColdStates()
    {
        Map<StringTag, char>::iterator it;

        for ( it = _coldStates.begin(); it != _coldStates.end(); ++it )
        {
            Vector<Transition> &predecessors( _graph->_graph[it->first]._predecessors );

            for ( unsigned i = 0; i < predecessors.size(); ++i )
            {
                if ( ( !_hotEscapables.exists( predecessors[i]._state ) )&&
                     ( _hotStates.exists( predecessors[i]._state ) ) )
                {
                    markHotEscapableState( predecessors[i]._state );
                }
            }
        }
    }

    void markHotEscapableState( const StringTag &state )
    {
        _hotEscapables[state] = 0;
        _newEscapables.append( state );
    }

    void findNewEscapables()
    {
        Vector<StringTag> lastRoundEscapables = _newEscapables;
        _newEscapables.clear();

        for ( unsigned i = 0; i < lastRoundEscapables.size(); ++i )
        {
            Vector<Transition> &predecessors( _graph->_graph[lastRoundEscapables[i]]._predecessors );

            for ( unsigned i = 0; i < predecessors.size(); ++i )
            {
                if ( ( !_hotEscapables.exists( predecessors[i]._state ) )&&
                     ( _hotStates.exists( predecessors[i]._state ) ) )
                {
                    markHotEscapableState( predecessors[i]._state );
                }
            }
        }
    }

    void markTraps()
    {
        Map<StringTag, char>::iterator it;

        for ( it = _hotStates.begin(); it != _hotStates.end(); ++it )
        {
            if ( !_hotEscapables.exists( it->first ) )
            {
                _hotTraps[it->first] = 0;
            }
        }
    }
};

#endif // __HotStateAnalyzer_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

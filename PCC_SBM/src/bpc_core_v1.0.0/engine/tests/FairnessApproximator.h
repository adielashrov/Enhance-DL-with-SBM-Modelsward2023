#ifndef __FairnessApproximator_h__
#define __FairnessApproximator_h__

#include "BenefitDatabase.h"
#include "Stringf.h"
#include "TemperatureToken.h"
#include "ThreadStateGraph.h"

class FairnessApproximator
{
public:
    void approximate( ThreadStateGraph &graph, TemperatureToken &token )
    {
        storeInput( graph, token );
        partitionHotAndCold();

        TS_TRACE( Stringf( "Number of hot states: %u", _hotStates.size() ).ascii() );

        findColdNeighborhood();

        TS_TRACE( Stringf( "Size of initial cold neighborhood: %u", _currentNeighborhood.size() ).ascii() );

        findMinimalFairness();

        TS_TRACE( Stringf( "Number of new fair state: %u", _fairStates.size() ).ascii() );
    }

private:
    TemperatureToken *_token;
    ThreadStateGraph *_graph;
    Map<StringTag, char> _hotStates;
    Map<StringTag, char> _coldStates;
    Map<StringTag, unsigned> _currentNeighborhood;
    Map<StringTag, char> _statesToFix;
    Vector<StringTag> _fairStates;
    Queue<StringTag> _statesLeavingNeighborhood;
    BenefitDatabase _benefitDatabase;

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

        _statesToFix = _hotStates;
    }

    bool hotState( const StringTag &label ) const
    {
        return _token->hot( _graph->_graph[label] );
    }

    bool coldState( const StringTag &label ) const
    {
        return !hotState( label );
    }

    void findColdNeighborhood()
    {
        _currentNeighborhood.clear();

        Map<StringTag, char>::iterator iterator;

        for ( iterator = _coldStates.begin(); iterator != _coldStates.end(); ++iterator )
        {
            Vector<Transition> &predecessors( _graph->_graph[iterator->first]._predecessors );

            for ( unsigned i = 0; i < predecessors.size(); ++i )
            {
                if ( _hotStates.exists( predecessors[i]._state ) && !_currentNeighborhood.exists( predecessors[i]._state ) )
                {
                    _currentNeighborhood[predecessors[i]._state] = 0;
                    _benefitDatabase.addState( predecessors[i]._state, calculateBenefit( predecessors[i]._state ) );
                }
            }
        }
    }

    void findMinimalFairness()
    {
        while ( !_statesToFix.empty() )
        {
            addFairnessToAnotherState();

            while ( !_statesLeavingNeighborhood.empty() )
            {
                adjustNeighborhood();
            }
        }
    }

    void addFairnessToAnotherState()
    {
        // Map<StringTag, unsigned>::iterator iterator = _currentNeighborhood.begin();
        // StringTag state = iterator->first;
        // unsigned maxBenefit = iterator->second;
        // ++iterator;

        // for ( ; iterator != _currentNeighborhood.end(); ++iterator )
        // {
        //     if ( iterator->second > maxBenefit )
        //     {
        //         state = iterator->first;
        //         maxBenefit = iterator->second;
        //     }
        // }

        StringTag state = _benefitDatabase.getHighest();

        _fairStates.append( state );
        _statesLeavingNeighborhood.push( state );
    }

    unsigned calculateBenefit( const StringTag &label )
    {
        unsigned value = 0;

        Vector<Transition> &predecessors( _graph->_graph[label]._predecessors );

        for ( unsigned i = 0; i < predecessors.size(); ++i )
        {
            if ( _statesToFix.exists( predecessors[i]._state ) )
            {
                ++value;
            }
        }

        return value;
    }

    void adjustNeighborhood()
    {
        StringTag state = _statesLeavingNeighborhood.peak();
        _statesLeavingNeighborhood.pop();

        try
        {
            _statesToFix.erase( state );
        }
        catch ( ... )
        {
            TS_TRACE( "Failed 1" ); // THIS IS WHERE WE FAIL!
            throw;
        }

        try
        {
            _currentNeighborhood.erase( state );
        }
        catch ( ... )
        {
            TS_TRACE( "Failed 2" );
            throw;
        }

        _benefitDatabase.removeState( state );

        reduceBenefitOfSuccessors( state );

        Vector<Transition> &predecessors( _graph->_graph[state]._predecessors );

        for ( unsigned i = 0; i < predecessors.size(); ++i )
        {
            StringTag predecessor = predecessors[i]._state;

            if ( _currentNeighborhood.exists( predecessor ) )
            {
                if ( allSuccessorsAreFixed( predecessor ) )
                {
                    _statesLeavingNeighborhood.push( predecessor );
                }
            }
            else if ( _statesToFix.exists( predecessor ) )
            {
                _benefitDatabase.addState( predecessor, calculateBenefit( predecessor ) );
                _currentNeighborhood[predecessor] = 0;
            }
        }
    }

    void reduceBenefitOfSuccessors( const StringTag &state )
    {
        Vector<Transition> &successors( _graph->_graph[state]._successors );

        for ( unsigned i = 0; i < successors.size(); ++i )
        {
            StringTag successor = successors[i]._state;

            if ( _currentNeighborhood.exists( successor ) )
            {
                _benefitDatabase.decrementBenefit( successor );
                //--_currentNeighborhood[successor];
            }
        }
    }

    bool allSuccessorsAreFixed( const StringTag &state )
    {
        Vector<Transition> &successors( _graph->_graph[state]._successors );

        for ( unsigned i = 0; i < successors.size(); ++i )
        {
            if ( _statesToFix.exists( successors[i]._state ) )
            {
                return false;
            }
        }

        return true;
    }
};

#endif // __FairnessApproximator_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

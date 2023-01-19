#ifndef __ManualIntersector_h__
#define __ManualIntersector_h__

#include "File.h"
#include "Queue.h"
#include "Stringf.h"
#include "ThreadStateGraph.h"

class ManualIntersector
{
public:
    void intersect( ThreadStateGraph &threadOne, ThreadStateGraph &threadTwo, ThreadStateGraph &output )
    {
        storeGraphs( threadOne, threadTwo, output );
        initializeOutputGraph();

        while ( haveStatesToCheck() )
        {
            checkAnotherState();
        }
    }

private:
    ThreadStateGraph *_threadOne;
    ThreadStateGraph *_threadTwo;
    ThreadStateGraph *_output;

    Map<StringTag, StringTag> _projectToFirst;
    Map<StringTag, StringTag> _projectToSecond;

    Map<StringTag, char> _statesAlreadyChecked;
    Queue<StringTag> _toCheck;

    bool haveStatesToCheck() const
    {
        return !_toCheck.empty();
    }

    void storeGraphs( ThreadStateGraph &threadOne, ThreadStateGraph &threadTwo, ThreadStateGraph &output )
    {
        _threadOne = &threadOne;
        _threadTwo = &threadTwo;
        _output = &output;
    }

    void initializeOutputGraph()
    {
        _output->_events = _threadOne->_events;
        createInitialState();
    }

    void createInitialState()
    {
        StringTag newInitial = addCombinedState( _threadOne->_initialState, _threadTwo->_initialState );
        //TS_TRACE( Stringf( "New initial state: %s", newInitial.ascii() ).ascii() );

        _output->_initialState = newInitial;
    }

    StringTag addCombinedState( const StringTag &labelOne, const StringTag &labelTwo )
    {
        StringTag newLabel = fuseLabels( labelOne, labelTwo );

        ThreadState newState;
        newState._label = newLabel;

        fuseStates( labelOne, labelTwo, newState );

        _projectToFirst[newLabel] = labelOne;
        _projectToSecond[newLabel] = labelTwo;

        _output->_graph[newLabel] = newState;

        if ( !_statesAlreadyChecked.exists( newLabel ) )
        {
            _toCheck.push( newLabel );
            _statesAlreadyChecked[newLabel] = 0;
        }

        return newLabel;
    }

    static StringTag fuseLabels( const StringTag &labelOne, const StringTag &labelTwo )
    {
        return Stringf( "%s %s", labelOne.ascii(), labelTwo.ascii() );
    }

    StringTag projectToFirst( StringTag &fusedLabel )
    {
        return _projectToFirst[fusedLabel];
    }

    StringTag projectToSecond( StringTag &fusedLabel )
    {
        return _projectToSecond[fusedLabel];
    }

    void fuseStates( const StringTag &first, const StringTag &second, ThreadState &fused )
    {
        ThreadState &stateOne( _threadOne->_graph[first] );
        ThreadState &stateTwo( _threadTwo->_graph[second] );

        fused._requestedEvents = stateOne._requestedEvents + stateTwo._requestedEvents;
        fused._watchedEvents = stateOne._watchedEvents + stateTwo._watchedEvents;
        fused._blockedEvents = stateOne._blockedEvents + stateTwo._blockedEvents;

        fused._requestedEvents.uniqueElements();
        fused._watchedEvents.uniqueElements();
        fused._blockedEvents.uniqueElements();

        for ( unsigned i = 0; i < fused._blockedEvents.size(); ++i )
        {
            Event blockedEvent = fused._blockedEvents[i];

            if ( fused._watchedEvents.exists( blockedEvent ) )
            {
                fused._watchedEvents.eraseByValue( blockedEvent );
            }
            if ( fused._requestedEvents.exists( blockedEvent ) )
            {
                fused._requestedEvents.eraseByValue( fused._blockedEvents[i] );
            }
        }

        for ( unsigned i = 0; i < fused._requestedEvents.size(); ++i )
        {
            Event requestedEvent = fused._requestedEvents[i];

            if ( fused._watchedEvents.exists( requestedEvent ) )
            {
                fused._watchedEvents.eraseByValue( requestedEvent );
            }
        }

        for ( unsigned i = 0; i < stateOne._atomicPropositions.size(); ++i )
        {
            fused._atomicPropositions.add( stateOne._atomicPropositions.at( i ) );
        }

        for ( unsigned i = 0; i < stateTwo._atomicPropositions.size(); ++i )
        {
            fused._atomicPropositions.add( stateTwo._atomicPropositions.at( i ) );
        }
    }

    void checkAnotherState()
    {
        StringTag label = _toCheck.peak();
        _toCheck.pop();

        //TS_TRACE( Stringf( "Checking state: %s", label.ascii() ).ascii() );

        for ( unsigned i = 0; i < _output->_graph[label]._requestedEvents.size(); ++i )
        {
            addSingleEdge( label, _output->_graph[label]._requestedEvents[i] );
        }

        for ( unsigned i = 0; i < _output->_graph[label]._watchedEvents.size(); ++i )
        {
            addSingleEdge( label, _output->_graph[label]._watchedEvents[i] );
        }

        addSelfLoops( label );
    }

    void addSelfLoops( StringTag &label )
    {
        ThreadState &state( _output->_graph[label] );

        for ( unsigned i = 0; i < _output->_events.size(); ++i )
        {
            Event event = _output->_events[i];

            if ( ( !state._requestedEvents.exists( event ) ) &&
                 ( !state._watchedEvents.exists( event ) ) &&
                 ( !state._blockedEvents.exists( event ) ) )
            {
                Transition transition( event, label );
                state._successors.append( transition );
                state._predecessors.append( transition );
            }
        }
    }

    void addSingleEdge( StringTag &label, Event &event )
    {
        ThreadState &first( _threadOne->_graph[projectToFirst( label )] );
        ThreadState &second( _threadTwo->_graph[projectToSecond( label )] );

        StringTag newLabel = fuseLabels( first.labelOfSuccessor( event ), second.labelOfSuccessor( event ) );

        if ( !_output->_graph.exists( newLabel ) )
        {
            //TS_TRACE( Stringf( "Encountered a new state: %s", newLabel.ascii() ).ascii() );
            addCombinedState( first.labelOfSuccessor( event ), second.labelOfSuccessor( event ) );
        }

        ThreadState &source( _output->_graph[label] );
        ThreadState &target( _output->_graph[newLabel] );

        Transition transition( event, newLabel );
        source._successors.append( transition );

        Transition backwardTransition( event, label );
        target._predecessors.append( backwardTransition );
    }
};

#endif // __ManualIntersector_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

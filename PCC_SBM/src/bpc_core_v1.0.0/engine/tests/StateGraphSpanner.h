#ifndef __StateGraphSpanner_h__
#define __StateGraphSpanner_h__

#include "Map.h"
#include "MockErrno.h"
#include "MockSocketFactory.h"
#include "MockTruth.h"
#include "RealMalloc.h"
#include "Stringf.h"
#include "ThreadState.h"
#include "ThreadStateGraph.h"

class MockForStateGraphSpanner :
    public RealMalloc,
    public MockErrno,
    public MockSocketFactory,
    public MockTruth
{
public:
};

class StateGraphSpanner : public MockSocket::ReceiveCallBack
{
public:
    class ThreadToken
    {
    public:
        virtual BThread *freshCopy() = 0;
    };

    ThreadStateGraph *_stateGraph;
    Vector<Event> _events;
    StateGraphSpanner::ThreadToken *_threadToken;

    void span( StateGraphSpanner::ThreadToken &threadToken, ThreadStateGraph &threadStateGraph )
    {
        enum {
            MAX_DEPTH_IN_GRAPH = 700,
        };

        TS_ASSERT( _mock = new MockForStateGraphSpanner );

        _mock->numberOfTruths = MAX_DEPTH_IN_GRAPH;

        _stateGraph = &threadStateGraph;
        _events = _stateGraph->_events;

        _threadToken = &threadToken;

        _socket = &( _mock->mockSocket );
        _socket->receiveCallBack = this;

        addInitialState();

        while ( haveStateToCheck() )
        {
            checkAnotherState();
        }

        TS_ASSERT_THROWS_NOTHING( delete _mock );
    }

private:
    MockForStateGraphSpanner *_mock;
    MockSocket *_socket;
    Vector<StringTag> _labelsToCheck;
    Event _lastAnnouncedEvent;
    unsigned _numOfSyncsToAllow;
    StringTag _previousStateLabel;
    Vector<Event> _currentEventSequence;

    bool haveStateToCheck() const
    {
        return !_labelsToCheck.empty();
    }

    void checkAnotherState()
    {
        StringTag label = _labelsToCheck[0];
        _labelsToCheck.erase( label );

        for ( unsigned i = 0; i < _events.size(); ++i )
        {
            Event event = _events[i];

            if ( !_stateGraph->_graph[label]._blockedEvents.exists( event ) )
            {
                if ( ( _stateGraph->_graph[label]._requestedEvents.exists( event ) ) ||
                     ( _stateGraph->_graph[label]._watchedEvents.exists( event ) ) )
                {
                    continueWithEvent( label, event );
                }
                else
                {
                    addSelfLoop( label, event );
                }
            }
        }
    }

    void addSelfLoop( StringTag label, Event event )
    {
        Transition selfLoop( event, label );

        _stateGraph->_graph[label]._successors.append( selfLoop );
        _stateGraph->_graph[label]._predecessors.append( selfLoop );
    }

    void pushAnnouncedEvent( Event event )
    {
        HeapData serialized;
        event.serialize( serialized );
        _socket->nextReceivedData.append( serialized );
        _currentEventSequence.append( event );
    }

    void clearSocket()
    {
        _socket->nextReceivedData.clear();
        _socket->receiveIndex = 0;
        _socket->receiveWasCalled = false;
    }

    void continueWithEvent( StringTag label, Event event )
    {
        clearSocket();
        _previousStateLabel = "";

        _numOfSyncsToAllow = _stateGraph->_graph[label]._history.size() + 1;

        for ( unsigned i = 0; i < _numOfSyncsToAllow - 1; ++i )
        {
            pushAnnouncedEvent( _stateGraph->_graph[label]._history[i] );
        }

        pushAnnouncedEvent( event );

        BThread *thread = _threadToken->freshCopy();

        try
        {
            thread->entryPoint();
        }
        catch ( const Error &e )
        {
        }

        deleteCopy( thread );
    }

    void deleteCopy( BThread *thread )
    {
        delete thread;

        _socket->wasCreated = _socket->wasCreatedIpPort = _socket->wasDiscarded = false;
    }

    void addInitialState()
    {
        BThread *thread = _threadToken->freshCopy();

        _numOfSyncsToAllow = 0;
        try
        {
            thread->entryPoint();
        }
        catch ( const Error &e )
        {
        }

        storeInitialState();
        deleteCopy( thread );
    }

    void storeInitialState()
    {
        _stateGraph->_initialState = _stateGraph->_graph.begin()->second._label;
    }

    StringTag getLastLabel() const
    {
        SyncRequest syncRequest( _socket->lastSentData );
        return syncRequest.getLabel();
    }

    Vector<Event> getLastBlocked() const
    {
        SyncRequest syncRequest( _socket->lastSentData );
        return syncRequest.getBlocked();
    }

    AtomicPropositions getLastAtomicPropositions() const
    {
        SyncRequest syncRequest( _socket->lastSentData );
        return syncRequest.getAtomicPropositions();
    }

    Vector<Event> getLastRequested()
    {
        SyncRequest syncRequest( _socket->lastSentData );
        return syncRequest.getRequested();
    }

    Vector<Event> getLastWatched() const
    {
        SyncRequest syncRequest( _socket->lastSentData );
        return syncRequest.getWatched();
    }

    void extractLastEvent()
    {
        _lastAnnouncedEvent = _currentEventSequence[0];
        _currentEventSequence.eraseAt( 0 );
    }

    void receiveWasCalled()
    {
        StringTag label = getLastLabel();

        if ( _stateGraph->_graph.exists( _previousStateLabel ) )
        {
            extractLastEvent();

            Transition successor( _lastAnnouncedEvent, label );

            if ( !_stateGraph->_graph[_previousStateLabel]._successors.exists( successor ) )
            {
                _stateGraph->_graph[_previousStateLabel]._successors.append( successor );
            }
        }

        if ( _stateGraph->_graph.exists( label ) )
        {
            if ( _stateGraph->_graph.exists( _previousStateLabel ) )
            {
                Transition predecessor( _lastAnnouncedEvent, _previousStateLabel );
                if ( !_stateGraph->_graph[label]._predecessors.exists( predecessor ) )
                {
                    _stateGraph->_graph[label]._predecessors.append( predecessor );
                }
            }

            _previousStateLabel = label;

            if ( _numOfSyncsToAllow == 0 )
            {
                throw Error( (Error::Code)0 );
            }
            else
            {
                --_numOfSyncsToAllow;
            }

            return;
        }

        ThreadState state;
        state._label = label;
        state._requestedEvents = getLastRequested();
        state._watchedEvents = getLastWatched();
        state._blockedEvents = getLastBlocked();
        state._atomicPropositions = getLastAtomicPropositions();

        if ( _stateGraph->_graph.exists( _previousStateLabel ) )
        {
            Transition predecessor( _lastAnnouncedEvent, _previousStateLabel );

            state._predecessors.append( predecessor );
            state._history = _stateGraph->_graph[_previousStateLabel]._history;
            state._history.append( _lastAnnouncedEvent );
        }

        _stateGraph->_graph[label] = state;

        _labelsToCheck.append( label );

        _previousStateLabel = label;

        if ( _numOfSyncsToAllow == 0 )
        {
            throw Error( (Error::Code)0 );
        }
        else
        {
            --_numOfSyncsToAllow;
        }
    }
};

#endif // __StateGraphSpanner_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

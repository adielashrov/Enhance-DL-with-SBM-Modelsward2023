#ifndef __Weaver_h__
#define __Weaver_h__

#include "AutoThreadListener.h"
#include "Event.h"
#include "EventSets.h"
#include "IWeaver.h"
#include "ISocket.h"
#include "Map.h"
#include "Vector.h"
#include <algorithm>
#include <random>

class Weaver : public IWeaver
{
public:
    Weaver( ISelector &selector ) : _selector( selector )
    {
        // fixate random draw
        // srand(time(NULL));
        uint32_t seed_val = 0;
        _range.seed(seed_val);

    }

    ~Weaver()
    {
        discardListeners();
    }

    void addSensorThread( const StringTag &threadName )
    {
        _sensorThreads.append(threadName);
    }

	void handleAcceptedSocket( ISocket &socket )
    {
        IThreadListener *threadListener = T::createThreadListener( *this, _selector, socket );
        storeThreadName( socket, threadListener );
        storeListener( threadListener );
    }

    void removeThread( IThreadListener *thread )
    {
        removeThreadFromDataStructures( thread );
        destroyThread( thread );

        if ( everyoneSynchornized() )
        {
            // printf("--->removeThread - everyoneSynchornized()...\n");
            selectNextEvent();
        }
        else
        {
            try
            {
                // printf("--->removeThread - attemptEagerSynchronization()...\n");
                attemptEagerSynchronization();
            }
            catch ( const EagerSynchronizationError &e )
            {
                clearDataStructures();
            }
        }
    }

    void bSync( IThreadListener *thread,
                const Vector<Event> &requested,
                const Vector<Event> &watched,
                const Vector<Event> &blocked )
    {
        // printBSyncStatus( thread, requested, watched, blocked );
        storeEvents( thread, requested, watched, blocked );

        if ( everyoneSynchornized() )
        {
            // printf("--->everyoneSynchornized()...\n");
            selectNextEvent();
        }
        else
        {
            try
            {
                // printf("--->attemptEagerSynchronization()\n");
                attemptEagerSynchronization();
            }
            catch ( const EagerSynchronizationError &e )
            {
                clearDataStructures();
            }
        }
    }

    void printBSyncStatus(IThreadListener *thread,
                const Vector<Event> &requested,
                const Vector<Event> &watched,
                const Vector<Event> &blocked)
    {
        StringTag threadName = _threadsToNames[thread];
        printf("======printBSyncStatus======\n");
        printf("Enter - bSync: Thread %s\n", threadName.ascii());
        printEventVector2(requested, "requested");
        printEventVector2(watched, "watched");
        printEventVector2(blocked, "blocked");
        printf("============================\n");
    }

private:
    class EagerSynchronizationError
    {
    };

    ISelector &_selector;
    Vector<IThreadListener *> _threadListeners;
    Map<IThreadListener *, EventSets> _threadsToEvents;
    Vector<Event *> _requested;
    Vector<Event *> _blocked;
    Event _selectedEvent;
    Vector<StringTag> _sensorThreads;
    Map<IThreadListener *, StringTag> _threadsToNames;
    Vector<IThreadListener *> _unsynchronizedThreads;
    Vector<Event *> _candidates;
    std::mt19937 _range;

    void storeListener( IThreadListener *threadListener )
    {
        _threadListeners.append( threadListener );
    }

    void storeThreadName( ISocket &socket, IThreadListener *threadListener )
    {
        HeapData nameData;
        socket.receive( nameData );
        _threadsToNames[threadListener] = StringTag( (char *)nameData.data(), nameData.size() );
    }

    bool everyoneSynchornized() const
    {
    	// printf("Weaver-> everyoneSynchornized: _threadsToEvents.size(): %u == _threadListeners.size(): %u  ? (%d) \n", _threadsToEvents.size(), _threadListeners.size(), _threadsToEvents.size() == _threadListeners.size() );
    	return _threadsToEvents.size() == _threadListeners.size();
    }

    void attemptEagerSynchronization()
    {
        collectRequested();
        collectBlocked();
        collectCandidates();

        findUnsynchronizedThreads();

        if( checkIfAllUnsyncThreadsAreSensors() )
        {
            // printf("AllUnsyncThreadsAreSensors...\n");
            if ( existCandidates() )
            {
                // printf("existCandidates...\n");
                selectFirstCandidate();
                announceEvent();
            }
        }

        clearDataStructures();
    }

    bool existCandidates() const
    {
        return !_candidates.empty();
    }

    void selectFirstCandidate()
    {
        if(_candidates.size() > 0)
        {
            // printf("_candidates size %zu\n", _candidates.size() );
            
            // _selectedEvent = (*_candidates[0]); // Select first event
            
            int index = rand() % _candidates.size(); // Select random event - use rand

            // std::uniform_int_distribution<int> int_dist(0, _candidates.size()-1); // Select random event - using random
            // int index = int_dist(_range);
            _selectedEvent = (*_candidates[index]);
            // printf("_selectedEvent, type: %u, index: %d\n", _selectedEvent.type(), index ); 
        }
        else
        {
            // printf("_candidates.size() is not larger than 0 \n");
            throw EagerSynchronizationError();
        }
    }

    bool checkIfAllUnsyncThreadsAreSensors()
    {
        bool allUnsyncThreadsAreSensors = true;
        for ( unsigned i = 0; i < _unsynchronizedThreads.size(); ++i )
        {
            IThreadListener *thread  = _unsynchronizedThreads[i];
            StringTag threadName = _threadsToNames[thread];
            // printf("Thread %s is not synchronized \n", threadName.ascii());
            if(!_sensorThreads.exists(threadName))  // Unsynced Thread should be a sensor
            {
                // printf("Thread %s is not synchronized, and it is NOT a sensor!\n", threadName.ascii());
                allUnsyncThreadsAreSensors = false;
            }
            else
            {
                // printf("Thread %s is not synchronized, and it is a sensor!\n", threadName.ascii());
            } 
         
        }

        // printf("checkIfAllUnsyncThreadsAreSensors: return value: %d\n", allUnsyncThreadsAreSensors);

        return allUnsyncThreadsAreSensors;
    }

    void findUnsynchronizedThreads()
    {
        for ( unsigned i = 0; i < _threadListeners.size(); ++i )
        {
            if ( !_threadsToEvents.exists( _threadListeners[i] ) )
            {
                _unsynchronizedThreads.append( _threadListeners[i] );
            }
        }
    }

    void storeEvents( IThreadListener *thread,
                      const Vector<Event> &requested,
                      const Vector<Event> &watched,
                      const Vector<Event> &blocked )
    {
        // printf("storeEvents for thread: %s \n", getThreadName(thread).ascii());
        _threadsToEvents[thread] = EventSets( requested, watched, blocked );
    }

    StringTag getThreadName( IThreadListener *thread )
    {
        StringTag retValue = "BThread";
        if ( _threadsToNames.exists(thread) ) {
           return _threadsToNames[thread];
        }
        return retValue;
    }

    void discardListeners()
    {
        for ( unsigned i = 0; i < _threadListeners.size(); ++i )
        {
            T::discardThreadListener( _threadListeners[i] );
        }
    }

    void collectRequested()
    {
        Map<IThreadListener *, EventSets>::iterator it;

        for ( it = _threadsToEvents.begin(); it != _threadsToEvents.end(); ++it )
        {
            for ( unsigned i = 0; i < it->second._requested.size(); ++i )
            {
                _requested.append( &(it->second._requested[i]) );
            }
        }

        // printfEventVector(_requested, "all_requested");
    }

    void collectBlocked()
    {
        Map<IThreadListener *, EventSets>::iterator it;

        for ( it = _threadsToEvents.begin(); it != _threadsToEvents.end(); ++it )
        {
            for ( unsigned i = 0; i < it->second._blocked.size(); ++i )
            {
                _blocked.append( &(it->second._blocked[i]) );
            }
        }
        // printfEventVector(_blocked, "all_blocked");
    }


    void selectEvent()
    {
        collectRequested();
        collectBlocked();
        //findRequestedNotBlocked();
        collectCandidates();
        if ( existCandidates() )
        {
            // printf("selectEvent -> existCandidates() is true \n");
            selectFirstCandidate();
        }
        else
        {
            throw Error( Error::SELECTION_DEADLOCKED );
        }

    }

    void findRequestedNotBlocked()
    {
        for ( unsigned i = 0; i < _requested.size(); ++i )
        {
            if ( enabledEvent( _requested[i] ) )
            {
                _selectedEvent = (*_requested[i]);
                // printf("Selected Event: %u \n", _selectedEvent.type());
                return;
            }
        }

        throw Error( Error::SELECTION_DEADLOCKED );
    }

    void collectCandidates()
    {
        for ( unsigned i = 0; i < _requested.size(); ++i )
        {
            if ( enabledEvent( _requested[i] ) )
            {
                _candidates.append( _requested[i] );
            }
        }

        // printfEventVector(_candidates, "all_candidates");
    }

    bool enabledEvent( Event *event )
    {
        for ( unsigned i = 0; i < _blocked.size(); ++i )
        {
            if ( *_blocked[i] == *event )
            {
                return false;
            }
        }

        return true;
    }

    bool threadRequestedOrWatched( IThreadListener *thread )
    {
        return threadRequested( thread ) || threadWatched( thread );
    }

    bool threadRequested( IThreadListener *thread )
    {
        for ( unsigned i = 0; i < _threadsToEvents[thread]._requested.size(); ++i )
        {
            if ( _threadsToEvents[thread]._requested[i] == _selectedEvent )
            {
                return true;
            }
        }

        return false;
    }

    bool threadWatched( IThreadListener *thread )
    {
        for ( unsigned i = 0; i < _threadsToEvents[thread]._watched.size(); ++i )
        {
            if ( _threadsToEvents[thread]._watched[i] == _selectedEvent )
            {
                return true;
            }
        }

        return false;
    }

    void announceEvent()
    {
        Event event( _selectedEvent );
        announceToSynchronizedThreads( event );
        // announceToUnsynchronizedThreads( event );
    }

    void announceToSynchronizedThreads( Event &event )
    {
        Map<IThreadListener *, EventSets>::iterator it;
        Map<IThreadListener *, EventSets> copy = _threadsToEvents;

        for ( it = copy.begin(); it != copy.end(); ++it )
        {
            if ( threadRequestedOrWatched( it->first ) )
            {
                it->first->announceChosenEvent( event );
                _threadsToEvents.erase( it->first );
            }
        }
    }

    void announceToUnsynchronizedThreads( Event &event )
    {
        for ( unsigned i = 0; i < _unsynchronizedThreads.size(); ++i )
        {
            _unsynchronizedThreads[i]->announceChosenEvent( event );
        }
    }

    /*
    void announceEvent()
    {
        for ( unsigned i = 0; i < _threadListeners.size(); ++i )
        {
            if ( threadRequestedOrWatched( _threadListeners[i] ) )
            {
                Event event( _selectedEvent );
                _threadListeners[i]->announceChosenEvent( event );
                _threadsToEvents.erase( _threadListeners[i] );
            }
        }
    }
    */

    void clearDataStructures()
    {
        _requested.clear();
        _blocked.clear();
        _candidates.clear();
        _unsynchronizedThreads.clear();
    }

    void removeThreadFromDataStructures( IThreadListener *thread )
    {
        if ( _threadListeners.exists( thread ) )
        {
            _threadListeners.erase( thread );
        }
        if ( _threadsToEvents.exists( thread ) )
        {
            _threadsToEvents.erase( thread );
        }
        if ( _threadsToNames.exists( thread ) )
        {
            _threadsToNames.erase( thread );
        }
    }

    void destroyThread( IThreadListener *thread )
    {
        T::discardThreadListener( thread );
    }

    void selectNextEvent()
    {
        selectEvent();
        announceEvent();
        clearDataStructures();
    }

    void printEventVector(Vector<Event *> events, const char* name )
    {
    	std::cout << name << ":{ ";
    	for ( unsigned i = 0; i < events.size(); ++i )
		{
            std::cout << events[i]->type() << " ";
		}
        std::cout << "}" << std::endl;
    }

    void printEventVector2(const Vector<Event> &events, char* name )
    {
        std::cout << name << ":{ "; 
        if(events.size() > 0)
        {
            Vector<Event> events_copy = events;
            for(std::vector<Event>::iterator it = events_copy.begin(); it != events_copy.end(); ++it) {
                std::cout << (*it).type() << " ";
            }
        }
        std::cout << "}" << std::endl;
    }
};

#endif // __Weaver_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

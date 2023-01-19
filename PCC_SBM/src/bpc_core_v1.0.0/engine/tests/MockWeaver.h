#ifndef __MockWeaver_h__
#define __MockWeaver_h__

#include "Event.h"
#include "ISelector.h"
#include "IWeaver.h"
#include "Vector.h"

class MockWeaver : public IWeaver
{
public:
	MockWeaver()
	{
		wasCreated = false;
		wasDiscarded = false;

        removeThreadWasCalled = false;
        bSyncWasCalled = false;
	}

	bool wasCreated;
	bool wasDiscarded;

    ISelector *lastSelector;

	void mockConstructor( ISelector &selector )
	{
		TS_ASSERT( !wasCreated );
		wasCreated = true;

        lastSelector = &selector;
	}

	void mockDestructor()
	{
		TS_ASSERT( wasCreated );
		TS_ASSERT( !wasDiscarded );
		wasDiscarded = true;
	}

	void handleAcceptedSocket( ISocket & )
	{
	}

    Vector<Event> lastRequested;
    Vector<Event> lastWatched;
    Vector<Event> lastBlocked;
    IThreadListener *lastThread;
    bool bSyncWasCalled;

    void bSync( IThreadListener *thread,
                const Vector<Event> &requested,
                const Vector<Event> &watched,
                const Vector<Event> &blocked )
    {
        lastRequested = requested;
        lastWatched = watched;
        lastBlocked = blocked;
        lastThread = thread;

        bSyncWasCalled = true;
    }

    IThreadListener *lastRemovedThread;
    bool removeThreadWasCalled;

    void removeThread( IThreadListener *thread )
    {
        removeThreadWasCalled = true;
        lastRemovedThread = thread;
    }

	Vector<StringTag> _sensorThreads;

    void addSensorThread( const StringTag &threadName )
    {
        _sensorThreads.append(threadName);
    }
};

#endif // __MockWeaver_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

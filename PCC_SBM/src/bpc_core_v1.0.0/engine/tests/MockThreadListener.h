#ifndef __MockThreadListener_h__
#define __MockThreadListener_h__

#include "Event.h"
#include "ISelector.h"
#include "ISocket.h"
#include "IThreadListener.h"
#include "IWeaver.h"

class MockThreadListener : public IThreadListener
{
public:
	MockThreadListener()
	{
		wasCreated = false;
		wasDiscarded = false;

        announceWasCalled = false;
	}

	bool wasCreated;
	bool wasDiscarded;

    IWeaver *lastWeaver;
    ISelector *lastSelector;
    ISocket *lastSocket;

	void mockConstructor( IWeaver &weaver, ISelector &selector, ISocket &socket )
	{
		TS_ASSERT( !wasCreated );
		wasCreated = true;

        lastWeaver = &weaver;
        lastSelector = &selector;
        lastSocket = &socket;
	}

	void mockDestructor()
	{
		TS_ASSERT( wasCreated );
		TS_ASSERT( !wasDiscarded );
		wasDiscarded = true;
	}

    int descriptor()
    {
        return 0;
    }

    void canRead()
    {
    }

    bool announceWasCalled;
    unsigned lastEventType;

    void announceChosenEvent( Event &event )
    {
        announceWasCalled = true;
        lastEventType = event.type();
    }
};

#endif // __MockThreadListener_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

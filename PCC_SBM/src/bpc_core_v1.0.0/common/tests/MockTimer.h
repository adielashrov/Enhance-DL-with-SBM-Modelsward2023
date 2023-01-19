#ifndef __MockTimer_h__
#define __MockTimer_h__

#include "ISelector.h"
#include "ITimer.h"

class MockTimer : public ITimer
{
public:
	MockTimer()
	{
        wasCreated = false;
		wasDiscarded = false;

        setWasCalled = false;
        stopWasCalled = false;
    }

	bool wasCreated;
	bool wasDiscarded;
    ISelector *lastSelector;
    ITimer::IExpired *lastExpired;

	void mockConstructor( ISelector &selector, ITimer::IExpired &expired )
	{
		TS_ASSERT( !wasCreated );
		wasCreated = true;

        lastSelector = &selector;
        lastExpired = &expired;
	}

	void mockDestructor()
	{
		TS_ASSERT( wasCreated );
		TS_ASSERT( !wasDiscarded );
		wasDiscarded = true;
	}

    bool setWasCalled;
    unsigned lastSetSeconds;

    void set( unsigned seconds )
    {
        setWasCalled = true;
        lastSetSeconds = seconds;
    }

    bool stopWasCalled;

    void stop()
    {
        stopWasCalled = true;
    }
};

#endif // __MockTimer_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

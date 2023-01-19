#ifndef __MockTimerFactory_h__
#define __MockTimerFactory_h__

#include "MockTimer.h"
#include "T/TimerFactory.h"

class MockTimerFactory :
	public T::Base_createTimer,
	public T::Base_discardTimer
{
public:
	MockTimer mockTimer;

	~MockTimerFactory()
	{
		if ( mockTimer.wasCreated )
		{
			TS_ASSERT( mockTimer.wasDiscarded );
		}
	}

	ITimer *createTimer( ISelector &selector, ITimer::IExpired &expired )
	{
		mockTimer.mockConstructor( selector, expired );
		return &mockTimer;
	}

	void discardTimer( ITimer *timer )
	{
		TS_ASSERT_EQUALS( timer, &mockTimer );
		mockTimer.mockDestructor();
	}
};

#endif // __MockTimerFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __AutoTimer_h__
#define __AutoTimer_h__

#include "ITimer.h"
#include "T/TimerFactory.h"

class AutoTimer
{
public:
	AutoTimer( ISelector &selector, ITimer::IExpired &expired )
	{
		_timer = T::createTimer( selector, expired );
	}

	~AutoTimer()
	{
		T::discardTimer( _timer );
		_timer = 0;
	}

	operator ITimer &()
	{
		return *_timer;
	}

	ITimer *operator->()
	{
		return _timer;
	}

private:
	ITimer *_timer;
};

#endif // __AutoTimer_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

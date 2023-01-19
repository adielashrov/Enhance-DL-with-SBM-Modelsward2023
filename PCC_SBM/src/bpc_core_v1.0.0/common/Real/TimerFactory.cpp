#include "Timer.h"

namespace T
{
	ITimer *createTimer( ISelector &selector, ITimer::IExpired &expired )
	{
		return new Timer( selector, expired );
	}

	void discardTimer( ITimer *timer )
	{
		delete timer;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

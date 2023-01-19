#ifndef __T__TimerFactory_h__
#define __T__TimerFactory_h__

#include "ITimer.h"
#include "cxxtest/Mock.h"

class ISelector;

namespace T
{
	ITimer *createTimer( ISelector &selector, ITimer::IExpired &expired );
	void discardTimer( ITimer *timer );
}

// ITimer *createTimer( int descriptor );
// void discardTimer( ITimer *timer );

CXXTEST_SUPPLY( createTimer,        /* => T::Base_AllocateIrp */
				ITimer *,           /* Return type            */
				createTimer,        /* Name of mock member    */
				( ISelector &selector, ITimer::IExpired &expired ),  /* Prototype              */
				T::createTimer,        /* Name of real function  */
				( selector, expired )       /* Parameter list         */ );

CXXTEST_SUPPLY_VOID( discardTimer,        /* => T::Base_AllocateIrp */
					 discardTimer,        /* Name of mock member    */
					 ( ITimer *timer ),  /* Prototype              */
					 T::discardTimer,        /* Name of real function  */
					 ( timer )            /* Parameter list         */ );

#endif // __T__TimerFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

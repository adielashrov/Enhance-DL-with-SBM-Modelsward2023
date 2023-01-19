#ifndef __T__Truth_h__
#define __T__Truth_h__

#include <cxxtest/Mock.h>

namespace T
{
	bool alwaysTrue();
}

CXXTEST_SUPPLY( alwaysTrue,        /* => T::Base_AllocateIrp */
				bool,           /* Return type            */
				alwaysTrue,        /* Name of mock member    */
				(),  /* Prototype              */
				T::alwaysTrue,        /* Name of real function  */
				()       /* Parameter list         */ );

#endif // __T__Truth_h__

//
// Local Variables:
// compile-command: "make -C ../../.. "
// tags-file-name: "../../../TAGS"
// c-basic-offset: 4
// End:
//

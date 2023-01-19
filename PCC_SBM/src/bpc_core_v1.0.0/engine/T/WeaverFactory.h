#ifndef __T__WeaverFactory_h__
#define __T__WeaverFactory_h__

#include "cxxtest/Mock.h"

class IWeaver;
class ISelector;

namespace T
{
	IWeaver *createWeaver( ISelector &selector );
	void discardWeaver( IWeaver *weaver );
}

CXXTEST_SUPPLY( createWeaver,
				IWeaver *,
				createWeaver,
				( ISelector &selector ),
				T::createWeaver,
				( selector ) );

CXXTEST_SUPPLY_VOID( discardWeaver,
					 discardWeaver,
					 ( IWeaver *weaver ),
					 T::discardWeaver,
					 ( weaver ) );

#endif // __T__WeaverFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

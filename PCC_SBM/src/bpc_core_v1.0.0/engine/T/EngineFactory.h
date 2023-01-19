#ifndef __T__EngineFactory_h__
#define __T__EngineFactory_h__

#include "cxxtest/Mock.h"

class IEngine;
class ISelector;

namespace T
{
	IEngine *createEngine();
	void discardEngine( IEngine *engine );
}

CXXTEST_SUPPLY( createEngine,
				IEngine *,
				createEngine,
				(),
				T::createEngine,
				() );

CXXTEST_SUPPLY_VOID( discardEngine,
					 discardEngine,
					 ( IEngine *engine ),
					 T::discardEngine,
					 ( engine ) );

#endif // __T__EngineFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

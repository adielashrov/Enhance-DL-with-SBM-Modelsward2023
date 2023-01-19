#ifndef __MockEngineFactory_h__
#define __MockEngineFactory_h__

#include "MockEngine.h"
#include "T/EngineFactory.h"

class MockEngineFactory :
	public T::Base_createEngine,
	public T::Base_discardEngine
{
public:
	MockEngine mockEngine;

	~MockEngineFactory()
	{
		if ( mockEngine.wasCreated )
		{
			TS_ASSERT( mockEngine.wasDiscarded );
		}
	}

	IEngine *createEngine()
	{
		mockEngine.mockConstructor();
		return &mockEngine;
	}

	void discardEngine( IEngine *engine )
	{
		TS_ASSERT_EQUALS( engine, &mockEngine );
		mockEngine.mockDestructor();
	}
};

#endif // __MockEngineFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

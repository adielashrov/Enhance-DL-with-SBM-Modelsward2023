#ifndef __MockWeaverFactory_h__
#define __MockWeaverFactory_h__

#include "MockWeaver.h"
#include "T/WeaverFactory.h"

class MockWeaverFactory :
	public T::Base_createWeaver,
	public T::Base_discardWeaver
{
public:
	MockWeaver mockWeaver;

	~MockWeaverFactory()
	{
		if ( mockWeaver.wasCreated )
		{
			TS_ASSERT( mockWeaver.wasDiscarded );
		}
	}

	IWeaver *createWeaver( ISelector &selector )
	{
		mockWeaver.mockConstructor( selector );
		return &mockWeaver;
	}

	void discardWeaver( IWeaver *weaver )
	{
		TS_ASSERT_EQUALS( weaver, &mockWeaver );
		mockWeaver.mockDestructor();
	}
};

#endif // __MockWeaverFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

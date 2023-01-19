#ifndef __MockEngine_h__
#define __MockEngine_h__

#include "IEngine.h"
#include "TestLog.h"

class MockEngine : public IEngine
{
public:
	MockEngine()
	{
		wasCreated = false;
		wasDiscarded = false;

        runWasCalled = false;
    }

	bool wasCreated;
	bool wasDiscarded;

	void mockConstructor()
	{
		TS_ASSERT( !wasCreated );
		wasCreated = true;
	}

	void mockDestructor()
	{
		TS_ASSERT( wasCreated );
		TS_ASSERT( !wasDiscarded );
		wasDiscarded = true;
	}

    bool runWasCalled;

    void run()
    {
        runWasCalled = true;

        TestLog::log( "Engine was run" );
    }

    void entryPoint()
    {
    }

    void interrupt()
    {
    }

    StringTag lastThreadName;
    void addSensorThread( const StringTag &threadName )
    {
    	lastThreadName = threadName;
    }
};

#endif // __MockEngine_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

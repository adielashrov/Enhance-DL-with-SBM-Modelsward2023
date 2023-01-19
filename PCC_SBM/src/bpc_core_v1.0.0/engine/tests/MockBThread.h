#ifndef __MockBThread_h__
#define __MockBThread_h__

#include "TestLog.h"
#include "IBThread.h"

class MockBThread : public IBThread
{
public:
	MockBThread()
	{
		wasCreated = false;
		wasDiscarded = false;

        runWasCalled = false;
        doneWasCalled = false;
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

    void entryPoint()
    {
    }

    void bSync( const Vector<Event> &, const Vector<Event> &, const Vector<Event> &, StringTag = "" )
    {
    }

    bool runWasCalled;

    void run()
    {
        runWasCalled = true;
    }

    bool doneWasCalled;

    void done()
    {
        doneWasCalled = true;
    }
};

#endif // __MockBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

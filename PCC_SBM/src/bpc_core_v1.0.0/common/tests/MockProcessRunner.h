#ifndef __MockProcessRunner_h__
#define __MockProcessRunner_h__

#include "IProcessRunner.h"

class MockProcessRunner : public IProcessRunner
{
public:
	MockProcessRunner()
	{
        wasCreated = false;
		wasDiscarded = false;

        runWasCalled = false;
        nextRunResult = 115;
	}

	bool wasCreated;
	bool wasDiscarded;

    IProcessRunner::IRunable *lastChild;

	void mockConstructor( IProcessRunner::IRunable &child )
	{
		TS_ASSERT( !wasCreated );
		wasCreated = true;

        lastChild = &child;
	}

	void mockDestructor()
	{
		TS_ASSERT( wasCreated );
		TS_ASSERT( !wasDiscarded );
		wasDiscarded = true;
	}

    bool runWasCalled;
    pid_t nextRunResult;

    pid_t run()
    {
        runWasCalled = true;
        return nextRunResult;
    }
};

#endif // __MockProcessRunner_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

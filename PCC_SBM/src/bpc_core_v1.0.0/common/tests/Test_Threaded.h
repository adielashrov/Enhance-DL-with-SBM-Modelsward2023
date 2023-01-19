#include <cxxtest/TestSuite.h>

#include "Error.h"
#include "IThreaded.h"
#include "MockErrno.h"
#include "RealMalloc.h"
#include "T/pthread.h"

class ThreadedForTests : public IThreaded
{
public:
    ThreadedForTests()
    {
        entryPointCalled = false;
    }

    bool entryPointCalled;

    void entryPoint()
    {
        entryPointCalled = true;
    }
};

class MockForThreaded :
    public RealMalloc,
    public MockErrno,
    public T::Base_pthread_create
{
public:
    MockForThreaded()
    {
        nextThreadId = 184;
        lastArgument = 0;
        pthreadCreateShouldFail = false;
    }

    int nextThreadId;
    void *lastArgument;
    bool pthreadCreateShouldFail;

    int pthread_create( pthread_t *thread, const pthread_attr_t *attr, void *( *start_routine )( void* ), void *arg )
    {
        const pthread_attr_t *DEFAULT_ATTRIBUTES = 0;
        TS_ASSERT_EQUALS( attr, DEFAULT_ATTRIBUTES );

        TS_ASSERT_EQUALS( start_routine, IThreaded::entryPointCaller
                          );

        lastArgument = arg;

        *thread = nextThreadId;

        return pthreadCreateShouldFail ? -1 : 0;
    }
};

class ThreadedTestSuite : public CxxTest::TestSuite
{
public:
	MockForThreaded *mock;

	void setUp()
	{
		TS_ASSERT( mock = new MockForThreaded );
	}

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete mock );
	}

    void test_run()
    {
        ThreadedForTests threaded;

        TS_ASSERT_THROWS_NOTHING( threaded.run() );

        TS_ASSERT_EQUALS( mock->lastArgument, &threaded );
    }

    void test_run__create_fails()
    {
        ThreadedForTests threaded;

        mock->pthreadCreateShouldFail = true;

        TS_ASSERT_THROWS_EQUALS( threaded.run(),
                                 const Error &e,
                                 e.code(),
                                 Error::PTHREAD_CREATE_FAILED );
    }

    void test_entry_point_caller()
    {
        ThreadedForTests threaded;

        TS_ASSERT( !threaded.entryPointCalled );

        TS_ASSERT_THROWS_NOTHING( IThreaded::entryPointCaller( &threaded ) );

        TS_ASSERT( threaded.entryPointCalled );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

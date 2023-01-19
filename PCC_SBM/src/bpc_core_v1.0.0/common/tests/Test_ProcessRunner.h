#include <cxxtest/TestSuite.h>

#include "MockErrno.h"
#include "ProcessRunner.h"
#include "T/unistd.h"
#include "TestLog.h"

class MockRunableForTests : public IProcessRunner::IRunable
{
public:
    MockRunableForTests()
    {
        runWasCalled = false;
    }

    bool runWasCalled;

    void run()
    {
        TestLog::log( "Run was called" );
        runWasCalled = true;
    }
};

class MockForProcessRunner :
    public MockErrno,
    public T::Base_fork,
    public T::Base__exit
{
public:
    MockForProcessRunner()
    {
        forkCalled = false;
        exitWasCalled = false;
    }

    pid_t nextForkResult;
    bool forkCalled;

    pid_t fork()
    {
        TS_ASSERT( !exitWasCalled );

        forkCalled = true;
        return nextForkResult;
    }

    bool exitWasCalled;

    void _exit( int status )
    {
        TestLog::log( "Exit was called" );
        TS_ASSERT_EQUALS( status, 0 );
        exitWasCalled = true;
    }
};

class ProcessRunnerTestSuite : public CxxTest::TestSuite
{
public:
	MockForProcessRunner *mock;
    MockRunableForTests *child;

	void setUp()
	{
		TS_ASSERT( mock = new MockForProcessRunner );
        TS_ASSERT( child = new MockRunableForTests );
	}

	void tearDown()
	{
        TS_ASSERT_THROWS_NOTHING( delete child );
        TS_ASSERT_THROWS_NOTHING( delete mock );
	}

    void test_run__child()
    {
        ProcessRunner processRunner( *child );

        mock->nextForkResult = 0;

        TS_ASSERT_THROWS_NOTHING( processRunner.run() );

        TS_ASSERT( mock->forkCalled );
        TS_ASSERT( child->runWasCalled );
        TS_ASSERT( mock->exitWasCalled );

        TS_ASSERT( TestLog::preceded( "Run was called", "Exit was called" ) );
    }

    void test_run__forK_fails()
    {
        ProcessRunner processRunner( *child );

        mock->nextForkResult = -1;

        TS_ASSERT_THROWS_EQUALS( processRunner.run(),
                                 const Error &e,
                                 e.code(),
                                 Error::FORK_FAILED );
    }

    void test_run__parent()
    {
        ProcessRunner processRunner( *child );

        mock->nextForkResult = 117;

        TS_ASSERT_EQUALS( processRunner.run(), 117 );

        TS_ASSERT( mock->forkCalled );
        TS_ASSERT( !child->runWasCalled );
        TS_ASSERT( !mock->exitWasCalled );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

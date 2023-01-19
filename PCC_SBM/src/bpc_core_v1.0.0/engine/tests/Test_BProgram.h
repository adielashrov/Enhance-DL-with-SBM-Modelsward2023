#include <cxxtest/TestSuite.h>

#include "BProgram.h"
#include "MockBThread.h"
#include "MockEngineFactory.h"
#include "MockErrno.h"

class MockForBProgram :
    public MockErrno,
    public MockEngineFactory
{
public:
};

class BProgramTestSuite : public CxxTest::TestSuite
{
public:
	MockForBProgram *mock;
    MockEngine *engine;

	void setUp()
	{
		TS_ASSERT( mock = new MockForBProgram );

        engine = &( mock->mockEngine );
	}

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete mock );
	}

    void test_run_program()
    {
        BProgram *bProgram;

        TS_ASSERT( bProgram = new BProgram );

        MockBThread thread1;
        MockBThread thread2;
        MockBThread thread3;

        TS_ASSERT_THROWS_NOTHING( bProgram->addThread( thread1 ) );
        TS_ASSERT_THROWS_NOTHING( bProgram->addThread( thread2 ) );
        TS_ASSERT_THROWS_NOTHING( bProgram->addThread( thread3 ) );

        TS_ASSERT_THROWS_NOTHING( bProgram->runProgram() );

        TS_ASSERT( !engine->runWasCalled );

        TS_ASSERT( thread1.runWasCalled );
        TS_ASSERT( thread2.runWasCalled );
        TS_ASSERT( thread3.runWasCalled );

        TS_ASSERT_THROWS_NOTHING( delete bProgram );

        TS_ASSERT( engine->wasDiscarded );
    }

    void test_run_engine()
    {
        BProgram *bProgram;

        TS_ASSERT( bProgram = new BProgram );

        TS_ASSERT( engine->wasCreated );

        TS_ASSERT_THROWS_NOTHING( bProgram->runEngine() );

        TS_ASSERT( engine->runWasCalled );

        TS_ASSERT_THROWS_NOTHING( delete bProgram );

        TS_ASSERT( engine->wasDiscarded );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

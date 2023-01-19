#include <cxxtest/TestSuite.h>

#include "Engine.h"
#include "MockSelectorFactory.h"
#include "MockServerSocketFactory.h"
#include "MockTruth.h"
#include "MockWeaverFactory.h"

class MockForEngine :
	public MockServerSocketFactory,
	public MockSelectorFactory,
	public MockWeaverFactory,
	public MockTruth
{
public:
};

class EngineTestSuite : public CxxTest::TestSuite
{
public:
	MockForEngine *mock;
	MockServerSocket *serverSocket;
	MockSelector *selector;
	MockWeaver *weaver;

	void setUp()
	{
		TS_ASSERT( mock = new MockForEngine );

		serverSocket = &( mock->mockServerSocket );
		selector = &( mock->mockSelector );
		weaver = &( mock->mockWeaver );
	}

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete mock );
	}

	void test_constructor()
	{
		Engine *engine;

		TS_ASSERT( engine = new Engine );

		TS_ASSERT( selector->wasCreated );

		TS_ASSERT( weaver->wasCreated );
        TS_ASSERT_EQUALS( weaver->lastSelector, selector );

		TS_ASSERT( serverSocket->wasCreated );
		TS_ASSERT_EQUALS( serverSocket->lastSelector, selector );
		TS_ASSERT_EQUALS( serverSocket->lastPort, EngineConfiguration::BTHREAD_MANAGEMENT_PORT );
		TS_ASSERT_EQUALS( serverSocket->lastAccepter, weaver );

		TS_ASSERT_THROWS_NOTHING( delete engine );
		TS_ASSERT( selector->wasDiscarded );
		TS_ASSERT( serverSocket->wasDiscarded );
		TS_ASSERT( weaver->wasDiscarded );
	}

	void test_entry_point()
	{
		mock->numberOfTruths = 5;

		TS_ASSERT_THROWS_NOTHING( Engine::main() );

		TS_ASSERT_EQUALS( selector->numberOfSelectCalls, 5U );
	}
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

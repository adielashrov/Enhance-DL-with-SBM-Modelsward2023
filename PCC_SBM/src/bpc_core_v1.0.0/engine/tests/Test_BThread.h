#include <cxxtest/TestSuite.h>

#include "BThread.h"
#include "Event.h"
#include "MockSocketFactory.h"
#include "RealMalloc.h"

class BThreadForTests : public BThread
{
public:
    void entryPoint()
    {
    }
};

class MockForBThread :
    public RealMalloc,
    public MockSocketFactory
{
public:
};

class BThreadTestSuite : public CxxTest::TestSuite
{
public:
	MockForBThread *mock;
    MockSocket *socket;

	void setUp()
	{
		TS_ASSERT( mock = new MockForBThread );

        socket = &( mock->mockSocket );
	}

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete mock );
	}

    void test_constructor_destructor()
    {
        BThreadForTests *bThread;

        TS_ASSERT( bThread = new BThreadForTests );

        TS_ASSERT( socket->wasCreatedIpPort );
        TS_ASSERT_EQUALS( socket->lastPort, EngineConfiguration::BTHREAD_MANAGEMENT_PORT );
        TS_ASSERT_EQUALS( socket->lastIp, inet_addr( EngineConfiguration::LOOPBACK_IP ) );

        TS_ASSERT_THROWS_NOTHING( delete bThread );

        TS_ASSERT( socket->wasDiscarded );
    }

    void test_bSync__no_propositions()
    {
    	/*
        Event event1( 1 );
        Event event2( 2 );
        Event event3( 3 );
        Event event4( 4 );

        BThreadForTests bThread;

        Vector<Event> requested;
        requested.append( event1 );
        requested.append( event2 );

        Vector<Event> watched;
        watched.append( event3 );

        Vector<Event> blocked;
        blocked.append( event4 );

        char socketData[] = { 0x01, 0x02, 0x03, 0x04 };
        ConstSimpleData constData( socketData, sizeof(socketData) );

        socket->nextReceivedData.append( constData );

        TestLog::clear();

        StringTag label = "hi";

        TS_ASSERT_THROWS_NOTHING( bThread.bSync( requested, watched, blocked, label ) );

        char expectedData[] = {
            // Label
            0x02, 0x00, 0x00, 0x00,
            0x68, 0x69,

            // Requested
            0x02, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x01, 0x00, 0x00, 0x00, // First
            0x04, 0x00, 0x00, 0x00, // Length of Second
            0x02, 0x00, 0x00, 0x00, // Second

            // WatchedFor
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x03, 0x00, 0x00, 0x00, // Length of first

            // Blocked
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x04, 0x00, 0x00, 0x00, // Length of first

            // Atomic Propositions
            0x00, 0x00, 0x00, 0x00 // Number of propositions
        };

        // TS_ASSERT_EQUALS( socket->lastSentData.size(), sizeof(expectedData) );
        // TS_ASSERT_SAME_DATA( socket->lastSentData.data(), expectedData, sizeof(expectedData) );

        TS_ASSERT( socket->receiveWasCalled );

        Event lastEvent;

        TS_ASSERT_THROWS_NOTHING( lastEvent = bThread.lastEvent() );

        TS_ASSERT_EQUALS( (int)lastEvent.type(), 0x04030201 );

        TS_ASSERT( TestLog::preceded( "Socket send called", "Socket receive called" ) );
        */
    }

    void test_bSync__with_propositions()
    {
    	/*
        Event event1( 1 );
        Event event2( 2 );
        Event event3( 3 );
        Event event4( 4 );

        BThreadForTests bThread;

        Vector<Event> requested;
        requested.append( event1 );
        requested.append( event2 );

        Vector<Event> watched;
        watched.append( event3 );

        Vector<Event> blocked;
        blocked.append( event4 );

        char socketData[] = { 0x01, 0x02, 0x03, 0x04 };
        ConstSimpleData constData( socketData, sizeof(socketData) );

        socket->nextReceivedData.append( constData );

        TestLog::clear();

        StringTag label = "hi";

        AtomicPropositions atomicPropositions;
        atomicPropositions.add( "ap1" );
        atomicPropositions.add( "ap2" );
        atomicPropositions.add( "ap3" );

        TS_ASSERT_THROWS_NOTHING( bThread.bSync( requested, watched, blocked, atomicPropositions, label ) );

        char expectedData[] = {
            // Label
            0x02, 0x00, 0x00, 0x00,
            0x68, 0x69,

            // Requested
            0x02, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x01, 0x00, 0x00, 0x00, // First
            0x04, 0x00, 0x00, 0x00, // Length of Second
            0x02, 0x00, 0x00, 0x00, // Second

            // WatchedFor
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x03, 0x00, 0x00, 0x00, // Length of first

            // Blocked
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x04, 0x00, 0x00, 0x00, // Length of first

            // Atomic Propositions
            0x03, 0x00, 0x00, 0x00, // Number of propositions

            0x03, 0x00, 0x00, 0x00, // Length of first
            0x61, 0x70, 0x31,       // First AP

            0x03, 0x00, 0x00, 0x00, // Length of second
            0x61, 0x70, 0x32,       // Second AP

            0x03, 0x00, 0x00, 0x00, // Length of third
            0x61, 0x70, 0x33,       // Third AP
        };

        // TS_ASSERT_EQUALS( socket->lastSentData.size(), sizeof(expectedData) );
        // TS_ASSERT_SAME_DATA( socket->lastSentData.data(), expectedData, sizeof(expectedData) );

        TS_ASSERT( socket->receiveWasCalled );

        Event lastEvent;

        TS_ASSERT_THROWS_NOTHING( lastEvent = bThread.lastEvent() );

        TS_ASSERT_EQUALS( (int)lastEvent.type(), 0x04030201 );

        TS_ASSERT( TestLog::preceded( "Socket send called", "Socket receive called" ) );
        */

    }

    void test_done()
    {
        BThreadForTests *bThread;

        TS_ASSERT( bThread = new BThreadForTests );

        TS_ASSERT( !socket->closeSocketWasCalled );

        TS_ASSERT_THROWS_NOTHING( bThread->done() );
        TS_ASSERT( socket->closeSocketWasCalled );

        TS_ASSERT_THROWS_NOTHING( delete bThread );

        TS_ASSERT( socket->wasDiscarded );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

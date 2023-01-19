#include <cxxtest/TestSuite.h>

#include "MockErrno.h"
#include "MockSelector.h"
#include "MockSocket.h"
#include "MockSocketFactory.h"
#include "MockWeaver.h"
#include "RealMalloc.h"
#include "ThreadListener.h"

class MockForThreadListener :
    public RealMalloc,
    public MockErrno,
    public MockSocketFactory
{
public:
};

class ThreadListenerTestSuite : public CxxTest::TestSuite
{
public:
	MockForThreadListener *mock;
    MockSelector *selector;
    MockWeaver *weaver;
    MockSocket *socket;

	void setUp()
	{
		TS_ASSERT( mock = new MockForThreadListener );
        TS_ASSERT( selector = new MockSelector );
        TS_ASSERT( weaver = new MockWeaver );

        socket = &( mock->mockSocket );
        socket->wasCreated = true;
    }

	void tearDown()
	{
        TS_ASSERT_THROWS_NOTHING( delete weaver );
        TS_ASSERT_THROWS_NOTHING( delete selector );
        TS_ASSERT_THROWS_NOTHING( delete mock );
	}

    void test_constructor_destructor()
    {
        ThreadListener *threadListener;

        TS_ASSERT( threadListener = new ThreadListener( *weaver, *selector, *socket ) );

        TS_ASSERT_EQUALS( selector->lastRegistetedDescriptor, socket->descriptor() );
        TS_ASSERT_EQUALS( selector->lastRegisteredSelectable, threadListener );

        TS_ASSERT_THROWS_NOTHING( delete threadListener );

        TS_ASSERT_EQUALS( selector->lastUnregisteredSelectable, threadListener );
        TS_ASSERT( socket->wasDiscarded );
    }

    void test_annouce_chosen_event()
    {
        ThreadListener threadListener( *weaver, *selector, *socket );

        char serialized[] = {
            // Label
            0x03, 0x00, 0x00, 0x00,
            0x68, 0x69, 0x00,

            // Requested
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x0a, 0x0b, 0x0c, 0x0e, // First

            // WatchedFor
            0x02, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x11, 0x22, 0x33, 0x1a, // First
            0x04, 0x00, 0x00, 0x00, // Length of second
            0x66, 0x54, 0x00, 0x02, // Second

            // Blocked
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x33, 0x11, 0x94, 0xa5, // First

            // Atomic Propositions
            0x00, 0x00, 0x00, 0x00 // Number of propositions
        };

        socket->nextReceivedData.append( ConstSimpleData( serialized, sizeof(serialized) ) );
        TS_ASSERT_THROWS_NOTHING( threadListener.canRead() );

        Event announcedEvent( 0x04030201 );
        char serializedEvent[] = { 0x01, 0x02, 0x03, 0x04 };

        TS_ASSERT_THROWS_NOTHING( threadListener.announceChosenEvent( announcedEvent ) );

        // TS_ASSERT_EQUALS( socket->lastSentData.size(), sizeof(serializedEvent) );
        // TS_ASSERT_SAME_DATA( socket->lastSentData.data(), serializedEvent, sizeof(serializedEvent) );
    }

    void test_can_read()
    {
        ThreadListener threadListener( *weaver, *selector, *socket );

        char serialized[] = {
            // Label
            0x03, 0x00, 0x00, 0x00,
            0x68, 0x69, 0x00,

            // Requested
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x0a, 0x0b, 0x0c, 0x0e, // First

            // WatchedFor
            0x02, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x11, 0x22, 0x33, 0x1a, // First
            0x04, 0x00, 0x00, 0x00, // Length of second
            0x66, 0x54, 0x00, 0x02, // Second

            // Blocked
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x33, 0x11, 0x94, 0xa5, // First

            // Atomic Propositions
            0x00, 0x00, 0x00, 0x00 // Number of propositions
        };

        socket->nextReceivedData.append( ConstSimpleData( serialized, sizeof(serialized) ) );

        TS_ASSERT_THROWS_NOTHING( threadListener.canRead() );

        TS_ASSERT_EQUALS( weaver->lastRequested.size(), 1U );
        TS_ASSERT_EQUALS( weaver->lastRequested[0].type(), 0x0e0c0b0aU );

        TS_ASSERT_EQUALS( weaver->lastWatched.size(), 2U );
        TS_ASSERT_EQUALS( weaver->lastWatched[0].type(), 0x1a332211U );
        TS_ASSERT_EQUALS( weaver->lastWatched[1].type(), 0x02005466U );

        TS_ASSERT_EQUALS( weaver->lastBlocked.size(), 1U );
        TS_ASSERT_EQUALS( weaver->lastBlocked[0].type(), 0xa5941133U );

        TS_ASSERT_EQUALS( weaver->lastThread, &threadListener );
    }

    void test_can_read__socket_closed()
    {
        ThreadListener threadListener( *weaver, *selector, *socket );

        socket->socketShouldClose = true;

        TS_ASSERT_THROWS_NOTHING( threadListener.canRead() );

        TS_ASSERT( weaver->removeThreadWasCalled );
        TS_ASSERT( !weaver->bSyncWasCalled );
        TS_ASSERT_EQUALS( weaver->lastRemovedThread, &threadListener );
    }

    void test_can_read__socket_error()
    {
        ThreadListener threadListener( *weaver, *selector, *socket );

        socket->socketShouldThrow = true;

        TS_ASSERT_THROWS_EQUALS( threadListener.canRead(),
                                 const Error &e,
                                 e.code(),
                                 Error::RECV_FAILED );

        TS_ASSERT( !weaver->removeThreadWasCalled );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

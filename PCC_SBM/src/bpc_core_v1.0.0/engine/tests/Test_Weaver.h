#include <cxxtest/TestSuite.h>

#include "MockErrno.h"
#include "MockSelector.h"
#include "MockSocket.h"
#include "MockThreadListenerFactory.h"
#include "Weaver.h"
#include <cstdio>

class MockForWeaver :
    public MockErrno,
    public MockThreadListenerFactory
{
public:
};

class WeaverTestSuite : public CxxTest::TestSuite
{
public:
	MockForWeaver *mock;
    MockSelector *selector;
    MockSocket *socket1;
    MockSocket *socket2;
    MockSocket *socket3;
    MockThreadListener *threadListener1;
    MockThreadListener *threadListener2;
    MockThreadListener *threadListener3;

	void setUp()
	{
		TS_ASSERT( mock = new MockForWeaver );

        TS_ASSERT( selector = new MockSelector );
        TS_ASSERT( socket1 = new MockSocket );
        TS_ASSERT( socket2 = new MockSocket );
        TS_ASSERT( socket3 = new MockSocket );

        threadListener1 = mock->registerThreadListener( socket1 );
        threadListener2 = mock->registerThreadListener( socket2 );
        threadListener3 = mock->registerThreadListener( socket3 );
    }

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete socket3 );
        TS_ASSERT_THROWS_NOTHING( delete socket2 );
        TS_ASSERT_THROWS_NOTHING( delete socket1 );
		TS_ASSERT_THROWS_NOTHING( delete selector );
        TS_ASSERT_THROWS_NOTHING( delete mock );
	}


	//TODO: test is out of order
    void test_handle_accepted_socket()
    {

    	return;

        Weaver *weaver;

        TS_ASSERT( weaver = new Weaver( *selector ) );

        TS_ASSERT_THROWS_NOTHING( weaver->handleAcceptedSocket( *socket1 ) );

        TS_ASSERT( threadListener1->wasCreated );
        TS_ASSERT_EQUALS( threadListener1->lastWeaver, weaver );
        TS_ASSERT_EQUALS( threadListener1->lastSelector, selector );
        TS_ASSERT_EQUALS( threadListener1->lastSocket, socket1 );

        TS_ASSERT_THROWS_NOTHING( weaver->handleAcceptedSocket( *socket2 ) );

        TS_ASSERT( threadListener2->wasCreated );
        TS_ASSERT_EQUALS( threadListener2->lastWeaver, weaver );
        TS_ASSERT_EQUALS( threadListener2->lastSelector, selector );
        TS_ASSERT_EQUALS( threadListener2->lastSocket, socket2 );

        TS_ASSERT( !threadListener1->wasDiscarded );
        TS_ASSERT( !threadListener2->wasDiscarded );

        TS_ASSERT_THROWS_NOTHING( delete weaver );

        TS_ASSERT( threadListener1->wasDiscarded );
        TS_ASSERT( threadListener2->wasDiscarded );
    }

    void test_bSync__blocking_works()
    {

        /*
        printf("\n\ntest_bSync__blocking_works -> Enter\n");
        Weaver weaver( *selector );
        printf("test_bSync__blocking_works initialized: selector: ({%p})\n", selector);

        printf("test_bSync__blocking_works -> handleAcceptedSocket -> start\n");
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket3 ) );
        printf("test_bSync__blocking_works -> handleAcceptedSocket -> end\n");


        printf("test_bSync__blocking_works -> event allocation\n");

        Event event1( 1U );
        Event event2( 2U );
        Event event3( 3U );
        Event event4( 4U );

        printf("test_bSync__blocking_works -> vector allocation \n");
        Vector<Event> request1;
        Vector<Event> request2;
        Vector<Event> request3;

        Vector<Event> watch1;
        Vector<Event> watch2;
        Vector<Event> watch3;

        Vector<Event> block1;
        Vector<Event> block2;
        Vector<Event> block3;

        printf("test_bSync__blocking_works -> append events to vector\n");

        request1.append( event1 );
        request1.append( event2 );

        request2.append( event2 );
        request2.append( event3 );

        watch1.append( event3 );
        watch3.append( event3 );

        block1.append( event4 );

        block3.append( event1 );
        block3.append( event2 );

        printf("test_bSync__blocking_works -> before testing (asserts)...\n");

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener1, request1, watch1, block1 ) );
        printf("test_bSync__blocking_works -> after ASSERT - 1...\n");

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        printf("test_bSync__blocking_works -> after ASSERT - 2...\n");
        
        assertAnnounceWasntCalled();
        printf("test_bSync__blocking_works -> after assertAnnounceWasntCalled - 1...\n");

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener3, request3, watch3, block3 ) );
        printf("test_bSync__blocking_works -> after ASSERT - 3...\n");
        
        assertAnnounceWasCalled( event3 );
        printf("test_bSync__blocking_works -> after assertAnnounceWasntCalled - 2...\n");
        */
    }

    void assertAnnounceWasCalled( Event &event )
    {
        TS_ASSERT( threadListener1->announceWasCalled );
        TS_ASSERT( threadListener2->announceWasCalled );
        TS_ASSERT( threadListener3->announceWasCalled );

        TS_ASSERT_EQUALS( threadListener1->lastEventType, event.type() );
        TS_ASSERT_EQUALS( threadListener2->lastEventType, event.type() );
        TS_ASSERT_EQUALS( threadListener3->lastEventType, event.type() );

        threadListener1->announceWasCalled = false;
        threadListener2->announceWasCalled = false;
        threadListener3->announceWasCalled = false;
    }

    void assertAnnouncedToThread( MockThreadListener *thread, Event &event )
    {
        TS_ASSERT( thread->announceWasCalled );
        TS_ASSERT_EQUALS( thread->lastEventType, event.type() );

        thread->announceWasCalled = false;
    }

    void assertDidntAnnounceToThread( MockThreadListener *thread )
    {
        TS_ASSERT( !thread->announceWasCalled );
    }

    void test_bSync__deadlock()
    {
        /*
        Weaver weaver( *selector );

        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket3 ) );

        Event event1( 1 );
        Event event2( 2 );
        Event event3( 3 );
        Event event4( 4 );

        Vector<Event> request1;
        Vector<Event> request2;
        Vector<Event> request3;

        Vector<Event> watch1;
        Vector<Event> watch2;
        Vector<Event> watch3;

        Vector<Event> block1;
        Vector<Event> block2;
        Vector<Event> block3;

        request1.append( event1 );
        request1.append( event2 );

        request2.append( event2 );
        request2.append( event3 );

        request3.append( event4 );

        block1.append( event4 );

        block2.append( event3 );

        block3.append( event1 );
        block3.append( event2 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener1, request1, watch1, block1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        assertAnnounceWasntCalled();
        TS_ASSERT_THROWS_EQUALS( weaver.bSync( threadListener3, request3, watch3, block3 ),
                                 const Error &e,
                                 e.code(),
                                 Error::SELECTION_DEADLOCKED );
        assertAnnounceWasntCalled();
        */
    }

    void assertAnnounceWasntCalled()
    {
        TS_ASSERT( !threadListener1->announceWasCalled );
        TS_ASSERT( !threadListener2->announceWasCalled );
        TS_ASSERT( !threadListener3->announceWasCalled );
    }

    void test_bSync__announce_only_if_requested_or_watched()
    {
        /*
        Weaver weaver( *selector );

        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket3 ) );

        Event event1( 1 );
        Event event2( 2 );
        Event event3( 3 );
        Event event4( 4 );

        Vector<Event> request1;
        Vector<Event> request2;
        Vector<Event> request3;

        Vector<Event> watch1;
        Vector<Event> watch2;
        Vector<Event> watch3;

        Vector<Event> block1;
        Vector<Event> block2;
        Vector<Event> block3;

        request1.append( event3 );
        request1.append( event2 );
        request2.append( event1 );
        request3.append( event4 );

        watch3.append( event2 );

        block2.append( event3 );
        block3.append( event1 );
        block3.append( event4 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener1, request1, watch1, block1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        assertAnnounceWasntCalled();
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener3, request3, watch3, block3 ) );
        assertAnnouncedToThread( threadListener1, event2 );
        assertDidntAnnounceToThread( threadListener2 );
        assertAnnouncedToThread( threadListener3, event2 );
        */
    }

    void test_bSync__consecutive_syncs__continue_to_block_if_not_informed()
    {
        /*
        Weaver weaver( *selector );

        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket3 ) );

        Event event1( 1 );
        Event event2( 2 );
        Event event3( 3 );
        Event event4( 4 );

        Vector<Event> request1;
        Vector<Event> request2;
        Vector<Event> request3;

        Vector<Event> watch1;
        Vector<Event> watch2;
        Vector<Event> watch3;

        Vector<Event> block1;
        Vector<Event> block2;
        Vector<Event> block3;

        request1.append( event1 );

        watch2.append( event1 );

        block3.append( event2 );
        block3.append( event3 );
        block3.append( event4 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener1, request1, watch1, block1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener3, request3, watch3, block3 ) );

        assertAnnouncedToThread( threadListener1, event1 );
        assertAnnouncedToThread( threadListener2, event1 );
        assertDidntAnnounceToThread( threadListener3 );

        request1.clear();
        request1.append( event2 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        assertAnnounceWasntCalled();

        TS_ASSERT_THROWS_EQUALS( weaver.bSync( threadListener1, request1, watch1, block1 ),
                                 const Error &e,
                                 e.code(),
                                 Error::SELECTION_DEADLOCKED );
                                 */
    }

    void test_bSync__consecutive_syncs()
    {
        /*
        Weaver weaver( *selector );

        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket3 ) );

        Event event1( 1 );
        Event event2( 2 );
        Event event3( 3 );
        Event event4( 4 );

        Vector<Event> request1;
        Vector<Event> request2;
        Vector<Event> request3;

        Vector<Event> watch1;
        Vector<Event> watch2;
        Vector<Event> watch3;

        Vector<Event> block1;
        Vector<Event> block2;
        Vector<Event> block3;

        watch3.append( event4 );
        block3.append( event3 );

        request1.append( event1 );
        request1.append( event2 );

        watch2.append( event2 );
        block2.append( event1 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener1, request1, watch1, block1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener3, request3, watch3, block3 ) );

        assertAnnouncedToThread( threadListener1, event2 );
        assertAnnouncedToThread( threadListener2, event2 );
        assertDidntAnnounceToThread( threadListener3 );

        request1.clear();
        watch1.append( event2 );
        watch1.append( event3 );

        block2.clear();
        watch2.clear();
        request2.append( event2 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        assertAnnounceWasntCalled();

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener1, request1, watch1, block1 ) );
        assertAnnouncedToThread( threadListener1, event2 );
        assertAnnouncedToThread( threadListener2, event2 );
        assertDidntAnnounceToThread( threadListener3 );

        request2.clear();
        request2.append( event4 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        assertAnnounceWasntCalled();

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener1, request1, watch1, block1 ) );

        assertAnnouncedToThread( threadListener2, event4 );
        assertAnnouncedToThread( threadListener3, event4 );
        assertDidntAnnounceToThread( threadListener1 );
        */
    }

    void test_bSync__consecutive_syncs__a_thread_is_removed()
    {
        /*
        Weaver weaver( *selector );

        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.handleAcceptedSocket( *socket3 ) );

        Event event1( 1 );
        Event event2( 2 );
        Event event3( 3 );
        Event event4( 4 );

        Vector<Event> request1;
        Vector<Event> request2;
        Vector<Event> request3;

        Vector<Event> watch1;
        Vector<Event> watch2;
        Vector<Event> watch3;

        Vector<Event> block1;
        Vector<Event> block2;
        Vector<Event> block3;

        watch3.append( event4 );
        block3.append( event3 );

        request1.append( event1 );
        request1.append( event2 );

        watch2.append( event2 );
        block2.append( event1 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener1, request1, watch1, block1 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );
        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener3, request3, watch3, block3 ) );

        assertAnnouncedToThread( threadListener1, event2 );
        assertAnnouncedToThread( threadListener2, event2 );
        assertDidntAnnounceToThread( threadListener3 );

        block2.clear();
        watch2.clear();
        request2.append( event2 );

        TS_ASSERT_THROWS_NOTHING( weaver.bSync( threadListener2, request2, watch2, block2 ) );

        assertAnnounceWasntCalled();
        TS_ASSERT_THROWS_NOTHING( weaver.removeThread( threadListener1 ) );

        TS_ASSERT( threadListener1->wasDiscarded );

        assertAnnouncedToThread( threadListener2, event2 );
        assertDidntAnnounceToThread( threadListener3 );
        */
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

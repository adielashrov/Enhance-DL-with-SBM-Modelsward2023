#include <cxxtest/TestSuite.h>

#include "ConstSimpleData.h"
#include "Event.h"
#include "RealMalloc.h"
#include "SyncRequest.h"

class MockForSyncRequest :
    public RealMalloc
{
};

class SyncRequestTestSuite : public CxxTest::TestSuite
{
public:
    MockForSyncRequest *mock;

    void setUp()
    {
        TS_ASSERT( mock = new MockForSyncRequest );
    }

    void tearDown()
    {
        TS_ASSERT_THROWS_NOTHING( delete mock );
    }

    void test_single_list_serialize()
    {
        Event event1( 0x04030201 );
        Event event2( 0xd4c3b2a1 );

        Vector<Event> requested;
        requested.append( event1 );
        requested.append( event2 );

        ListOfEvents listOfEvents( requested );

        HeapData serialized;

        TS_ASSERT_THROWS_NOTHING( listOfEvents.serialize( serialized ) );

        char expectedSerialized[] = {
            0x02, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x01, 0x02, 0x03, 0x04,
            0x04, 0x00, 0x00, 0x00, // Length of second
            0xa1, 0xb2, 0xc3, 0xd4 // Second
        };

        // TS_ASSERT_EQUALS( serialized.size(), sizeof(expectedSerialized) );
        // TS_ASSERT_SAME_DATA( serialized.data(), expectedSerialized, serialized.size() );
    }

    void test_sync_request_serialize()
    {
        Event event1( 0x04030201 );
        Event event2( 0xd4c3b2a1 );
        Event event3( 0x32547698 );
        Event event4( 0xcdcdfefe );

        Vector<Event> requested;
        requested.append( event1 );

        Vector<Event> watchedFor;
        watchedFor.append( event2 );
        watchedFor.append( event3 );

        Vector<Event> blocked;
        blocked.append( event4 );

        StringTag label = "hello world";

        AtomicPropositions atomicPropositions;
        atomicPropositions.add( "ap1" );
        atomicPropositions.add( "ap2" );

        SyncRequest syncRequest( requested, watchedFor, blocked, atomicPropositions, label );

        HeapData serialized;

        TS_ASSERT_THROWS_NOTHING( syncRequest.serialize( serialized ) );

        char expectedSerialized[] = {
            // Label
            0x0b, 0x00, 0x00, 0x00, // Length of label
            0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64,

            // Requested
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0x01, 0x02, 0x03, 0x04, // First

            // WatchedFor
            0x02, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0xa1, 0xb2, 0xc3, 0xd4, // First
            0x04, 0x00, 0x00, 0x00, // Length of second
            0x98, 0x76, 0x54, 0x32, // Second

            // Blocked
            0x01, 0x00, 0x00, 0x00, // Number of events
            0x04, 0x00, 0x00, 0x00, // Length of first
            0xfe, 0xfe, 0xcd, 0xcd, // First

            // Atomic Propositions
            0x02, 0x00, 0x00, 0x00, // Number of Properties

            0x03, 0x00, 0x00, 0x00, // Length of first
            0x61, 0x70, 0x31,       // First AP

            0x03, 0x00, 0x00, 0x00, // Length of second
            0x61, 0x70, 0x32,       // second AP
        };

        // TS_ASSERT_EQUALS( serialized.size(), sizeof(expectedSerialized) );
        // TS_ASSERT_SAME_DATA( serialized.data(), expectedSerialized, serialized.size() );
    }

    void test_constructor_from_serialized()
    {
        char serialized[] = {
            // Label
            0x0b, 0x00, 0x00, 0x00, // Length of label
            0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64,

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
            0x02, 0x00, 0x00, 0x00, // Number of Properties

            0x03, 0x00, 0x00, 0x00, // Length of first
            0x61, 0x70, 0x31,       // First AP

            0x03, 0x00, 0x00, 0x00, // Length of second
            0x61, 0x70, 0x32,       // second AP
        };

        SyncRequest syncRequest( ConstSimpleData( serialized, sizeof(serialized) ) );

        Vector<Event> requested = syncRequest.getRequested();
        Vector<Event> watched = syncRequest.getWatched();
        Vector<Event> blocked = syncRequest.getBlocked();
        AtomicPropositions atomicPropositions = syncRequest.getAtomicPropositions();

        TS_ASSERT_EQUALS( requested.size(), 1U );
        TS_ASSERT_EQUALS( requested[0].type(), 0x0e0c0b0aU );

        TS_ASSERT_EQUALS( watched.size(), 2U );
        TS_ASSERT_EQUALS( watched[0].type(), 0x1a332211U );
        TS_ASSERT_EQUALS( watched[1].type(), 0x02005466U );

        TS_ASSERT_EQUALS( blocked.size(), 1U );
        TS_ASSERT_EQUALS( blocked[0].type(), 0xa5941133U );

        StringTag label = syncRequest.getLabel();
        StringTag expectedLabel( "hello world" );

        TS_ASSERT_EQUALS( label.length(), expectedLabel.length() );
        TS_ASSERT_EQUALS( expectedLabel, label );

        TS_ASSERT_EQUALS( atomicPropositions.size(), 2U );
        TS_ASSERT_EQUALS( atomicPropositions.get()[0], "ap1" );
        TS_ASSERT_EQUALS( atomicPropositions.get()[1], "ap2" );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

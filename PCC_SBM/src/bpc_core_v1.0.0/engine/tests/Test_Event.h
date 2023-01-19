#include <cxxtest/TestSuite.h>

#include "Event.h"
#include "RealMalloc.h"
#include <cstdio>

class MockForEvent :
    public RealMalloc
{
public:
};

class EventTestSuite : public CxxTest::TestSuite
{
public:
	MockForEvent *mock;

	void setUp()
	{
		TS_ASSERT( mock = new MockForEvent );
	}

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete mock );
	}

    void test_type_and_equality()
    {
        Event event1( 1U );
        Event event2( 2U );
        Event event3( 1U );

        TS_ASSERT_EQUALS( event1.type(), 1U );
        TS_ASSERT_EQUALS( event2.type(), 2U );
        TS_ASSERT_EQUALS( event3.type(), 1U );

        TS_ASSERT( event1 == event1 );
        TS_ASSERT( event1 == event3 );
        TS_ASSERT( !( event2 == event1 ) );
        TS_ASSERT( !( event2 == event3 ) );

        TS_ASSERT( !(event1 != event1 ) );
        TS_ASSERT( !(event1 != event3 ) );
        TS_ASSERT( event2 != event1 );
        TS_ASSERT( event2 != event3 );

        Event event4;
        TS_ASSERT_EQUALS( event4.type(), 0xffffffff );
    }

    // TODO: enable test
    void test_serialize()
    {
    	/*
        Event event1( 0x1a2b3c4d );

        HeapData serialized;
        event1.serialize( serialized );

        char expectedSeriazlied[] = { 0x4d, 0x3c, 0x2b, 0x1a };

        TS_ASSERT_EQUALS( serialized.size(), sizeof(expectedSeriazlied) );
        TS_ASSERT_SAME_DATA( serialized.data(), expectedSeriazlied, sizeof(expectedSeriazlied) );
        */
    }

    // TODO: enable test
    void test_deserialize()
    {
    	/*
        char serialized[] = { 0x4d, 0x3c, 0x2b, 0x1a };

        Event event1;
        event1.deserialize( ConstSimpleData( serialized, sizeof(serialized) ) );

        TS_ASSERT_EQUALS( event1.type(), 0x1a2b3c4du );

        Event event2( 0x1a2b3c4d );

        TS_ASSERT( event1 == event2 );
        */
    }

    void test_smaller()
    {
        Event event1( 1 );
        Event event2( 2 );
        Event event3( 3 );

        TS_ASSERT( event1 < event2 );
        TS_ASSERT( event1 < event3 );
        TS_ASSERT( event2 < event3 );

        TS_ASSERT( !( event2 < event1 ) );
        TS_ASSERT( !( event3 < event1 ) );
        TS_ASSERT( !( event3 < event2 ) );
    }

    void test_assignment()
    {
        Event event1( 1 );

        Event event2;
        TS_ASSERT_EQUALS( event2.type(), (unsigned)-1 );

        event2 = event1;
        TS_ASSERT_EQUALS( event2.type(), 1U );
    }

    void test_copy_constructor()
    {
        Event event1( 1 );
        Event event2( event1 );
        TS_ASSERT_EQUALS( event2.type(), 1U );
    }   

};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

#include <cxxtest/TestSuite.h>

#include "AtomicPropositions.h"

class MockForAtomicPropositions
{
};

class AtomicPropositionsTestSuite : public CxxTest::TestSuite
{
public:
    MockForAtomicPropositions *mock;

    void setUp()
    {
        TS_ASSERT( mock = new MockForAtomicPropositions );
    }

    void tearDown()
    {
        TS_ASSERT_THROWS_NOTHING( delete mock );
    }

    void test_add()
    {
        AtomicPropositions atomicPropositions;

        TS_ASSERT_THROWS_NOTHING( atomicPropositions.add( "hi" ) );
        TS_ASSERT_EQUALS( atomicPropositions.size(), 1U );

        TS_ASSERT_THROWS_NOTHING( atomicPropositions.add( "there" ) );
        TS_ASSERT_EQUALS( atomicPropositions.size(), 2U );

        TS_ASSERT_EQUALS( atomicPropositions.at( 0 ), "hi" );
        TS_ASSERT_EQUALS( atomicPropositions.at( 1 ), "there" );

        Vector<StringTag> result;

        TS_ASSERT_THROWS_NOTHING( result = atomicPropositions.get() );

        TS_ASSERT_EQUALS( result.size(), 2U );
        TS_ASSERT_EQUALS( result[0], "hi" );
        TS_ASSERT_EQUALS( result[1], "there" );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

#include <cxxtest/TestSuite.h>

#include "Error.h"
#include "MockErrno.h"
#include "StringTag.h"
#include "Vector.h"

class VectorTestSuite : public CxxTest::TestSuite
{
public:
    MockErrno *mockErrno;

    void setUp()
    {
        TS_ASSERT( mockErrno = new MockErrno );
    }

    void tearDown()
    {
        TS_ASSERT_THROWS_NOTHING( delete mockErrno );
    }

    void test_brackets()
    {
        Vector<StringTag> vector;

        vector.append( "Apple" );
        vector.append( "Red" );
        vector.append( "Tasty" );

        TS_ASSERT_EQUALS( vector[0], "Apple" );
        TS_ASSERT_EQUALS( vector[1], "Red" );
        TS_ASSERT_EQUALS( vector[2], "Tasty" );
    }

    void test_size_and_exists()
    {
        Vector<StringTag> vector;

        TS_ASSERT_EQUALS( vector.size(), 0u );
        TS_ASSERT( vector.empty() );

        vector.append( "Apple" );
        TS_ASSERT_EQUALS( vector.size(), 1u );
        TS_ASSERT( vector.exists( "Apple" ) );
        TS_ASSERT( !vector.exists( "Red" ) );

        TS_ASSERT( !vector.empty() );

        vector.append( "Red" );
        TS_ASSERT_EQUALS( vector.size(), 2u );
        TS_ASSERT( vector.exists( "Apple" ) );
        TS_ASSERT( vector.exists( "Red" ) );

        vector.append( "Tasty" );
        TS_ASSERT_EQUALS( vector.size(), 3u );
        TS_ASSERT( vector.exists( "Apple" ) );
        TS_ASSERT( vector.exists( "Red" ) );
        TS_ASSERT( vector.exists( "Tasty" ) );
    }

    void test_erase()
    {
        Vector<StringTag> vector;

        vector.append( "Apple" );
        TS_ASSERT( vector.exists( "Apple" ) );
        TS_ASSERT_THROWS_NOTHING( vector.erase( "Apple" ) );
        TS_ASSERT( !vector.exists( "Apple" ) );
        TS_ASSERT_EQUALS( vector.size(), 0U );

        vector.append( "Red" );
        vector.append( "Tasty" );
        TS_ASSERT( !vector.exists( "Apple" ) );
        TS_ASSERT( vector.exists( "Red" ) );
        TS_ASSERT( vector.exists( "Tasty" ) );

        TS_ASSERT_THROWS_NOTHING( vector.erase( "Tasty" ) );
        TS_ASSERT( !vector.exists( "Apple" ) );
        TS_ASSERT( vector.exists( "Red" ) );
        TS_ASSERT( !vector.exists( "Tasty" ) );

        TS_ASSERT_THROWS_EQUALS( vector.erase( "Bla" ),
                                 const Error &e,
                                 e.code(),
                                 Error::VALUE_DOESNT_EXIST_IN_VECTOR );

        Vector<StringTag> anotherVector;
        anotherVector.append( "1" );

        StringTag toDelete = anotherVector[0];

        TS_ASSERT_THROWS_NOTHING( anotherVector.erase( toDelete ) );
        TS_ASSERT_EQUALS( anotherVector.size(), 0u );
    }

    void test_eraseByValue()
    {
        Vector<StringTag> vector;

        vector.append( "Apple" );
        TS_ASSERT( vector.exists( "Apple" ) );
        TS_ASSERT_THROWS_NOTHING( vector.eraseByValue( "Apple" ) );
        TS_ASSERT( !vector.exists( "Apple" ) );
        TS_ASSERT_EQUALS( vector.size(), 0U );

        vector.append( "Red" );
        vector.append( "Tasty" );
        TS_ASSERT( !vector.exists( "Apple" ) );
        TS_ASSERT( vector.exists( "Red" ) );
        TS_ASSERT( vector.exists( "Tasty" ) );

        TS_ASSERT_THROWS_NOTHING( vector.eraseByValue( "Tasty" ) );
        TS_ASSERT( !vector.exists( "Apple" ) );
        TS_ASSERT( vector.exists( "Red" ) );
        TS_ASSERT( !vector.exists( "Tasty" ) );

        TS_ASSERT_THROWS_EQUALS( vector.eraseByValue( "Bla" ),
                                 const Error &e,
                                 e.code(),
                                 Error::VALUE_DOESNT_EXIST_IN_VECTOR );

        Vector<StringTag> anotherVector;
        anotherVector.append( "1" );

        StringTag toDelete = anotherVector[0];

        TS_ASSERT_THROWS_NOTHING( anotherVector.eraseByValue( toDelete ) );
        TS_ASSERT_EQUALS( anotherVector.size(), 0u );
    }

    void test_erase_at()
    {
        Vector<int> vector;

        vector.append( 2 );
        vector.append( 4 );
        vector.append( 6 );
        vector.append( 8 );

        vector.eraseAt( 0 );

        TS_ASSERT_EQUALS( vector[0], 4 );
        TS_ASSERT_EQUALS( vector[1], 6 );
        TS_ASSERT_EQUALS( vector[2], 8 );

        vector.eraseAt( 1 );

        TS_ASSERT_EQUALS( vector[0], 4 );
        TS_ASSERT_EQUALS( vector[1], 8 );

        TS_ASSERT_THROWS_EQUALS( vector.eraseAt( 17 ),
                                 const Error &e,
                                 e.code(),
                                 Error::VECTOR_OUT_OF_BOUND );
    }

    void test_clear()
    {
        Vector<unsigned> vector;

        vector.append( 5 );
        vector.append( 10 );

        TS_ASSERT_EQUALS( vector.size(), 2U );

        TS_ASSERT_THROWS_NOTHING( vector.clear() );

        TS_ASSERT_EQUALS( vector.size(), 0U );

        TS_ASSERT( !vector.exists( 5 ) );
        TS_ASSERT( !vector.exists( 10 ) );

        vector.append( 10 );

        TS_ASSERT_EQUALS( vector.size(), 1U );
        TS_ASSERT( vector.exists( 10 ) );
    }

    void test_concatenation()
    {
        Vector<unsigned> one;
        Vector<unsigned> two;
        Vector<unsigned> output;

        one.append( 1 );
        one.append( 15 );

        two.append( 27 );
        two.append( 13 );

        output = one + two;

        TS_ASSERT_EQUALS( output.size(), 4U );

        TS_ASSERT_EQUALS( output[0], 1U );
        TS_ASSERT_EQUALS( output[1], 15U );
        TS_ASSERT_EQUALS( output[2], 27U );
        TS_ASSERT_EQUALS( output[3], 13U );

        TS_ASSERT_EQUALS( one.size(), 2U );

        one += two;

        TS_ASSERT_EQUALS( one.size(), 4U );

        TS_ASSERT_EQUALS( one[0], 1U );
        TS_ASSERT_EQUALS( one[1], 15U );
        TS_ASSERT_EQUALS( one[2], 27U );
        TS_ASSERT_EQUALS( one[3], 13U );
    }

    void test_unique_elements()
    {
        Vector<int> vector;

        vector.append( 1 );
        vector.append( 2 );
        vector.append( 3 );
        vector.append( 1 );
        vector.append( 1 );
        vector.append( 2 );
        vector.append( 1 );
        vector.append( 3 );

        TS_ASSERT_EQUALS( vector.size(), 8U );

        TS_ASSERT_THROWS_NOTHING( vector.uniqueElements() );

        TS_ASSERT_EQUALS( vector.size(), 3U );

        TS_ASSERT_EQUALS( vector[0], 1 );
        TS_ASSERT_EQUALS( vector[1], 2 );
        TS_ASSERT_EQUALS( vector[2], 3 );
    }

    void test_pop_first()
    {
        Vector<int> vector;

        vector.append( 1 );
        vector.append( 2 );
        vector.append( 3 );

        TS_ASSERT_EQUALS( vector.popFirst(), 1 );
        TS_ASSERT_EQUALS( vector.size(), 2U );

        TS_ASSERT_EQUALS( vector.popFirst(), 2 );
        TS_ASSERT_EQUALS( vector.size(), 1U );

        TS_ASSERT_EQUALS( vector.popFirst(), 3 );
        TS_ASSERT_EQUALS( vector.size(), 0U );

        TS_ASSERT_THROWS_EQUALS( vector.popFirst(),
                                 const Error &e,
                                 e.code(),
                                 Error::POPPING_FROM_EMPTY_VECTOR );
    }

    void test_pop()
    {
        Vector<int> vector;

        vector.append( 1 );
        vector.append( 2 );
        vector.append( 3 );

        TS_ASSERT_EQUALS( vector.pop(), 3 );
        TS_ASSERT_EQUALS( vector.size(), 2U );

        TS_ASSERT_EQUALS( vector.pop(), 2 );
        TS_ASSERT_EQUALS( vector.size(), 1U );

        TS_ASSERT_EQUALS( vector.pop(), 1 );
        TS_ASSERT_EQUALS( vector.size(), 0U );

        TS_ASSERT_THROWS_EQUALS( vector.pop(),
                                 const Error &e,
                                 e.code(),
                                 Error::POPPING_FROM_EMPTY_VECTOR );
    }

    void test_last()
    {
        Vector<int> vector;

        TS_ASSERT_THROWS_EQUALS( vector.last(),
                                 const Error &e,
                                 e.code(),
                                 Error::VECTOR_OUT_OF_BOUND );

        vector.append( 1 );
        TS_ASSERT_EQUALS( vector.last(), 1 );

        vector.append( 2 );
        TS_ASSERT_EQUALS( vector.last(), 2 );

        vector.append( 3 );
        TS_ASSERT_EQUALS( vector.last(), 3 );
    }

    void test_insert_head()
    {
        Vector<int> vector;

        vector.append( 1 );
        vector.append( 2 );

        vector.insertHead( 3 );

        TS_ASSERT_EQUALS( vector.size(), 3U );
        TS_ASSERT_EQUALS( vector[0], 3 );
        TS_ASSERT_EQUALS( vector[1], 1 );
        TS_ASSERT_EQUALS( vector[2], 2 );

        vector.insertHead( 4 );

        TS_ASSERT_EQUALS( vector.size(), 4U );
        TS_ASSERT_EQUALS( vector[0], 4 );
        TS_ASSERT_EQUALS( vector[1], 3 );
        TS_ASSERT_EQUALS( vector[2], 1 );
        TS_ASSERT_EQUALS( vector[3], 2 );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

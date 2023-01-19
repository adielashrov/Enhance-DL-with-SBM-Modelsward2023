#include <cxxtest/TestSuite.h>

#include "Error.h"
#include "MockErrno.h"
#include "StringTag.h"
#include "UniqueVector.h"

class UniqueVectorTestSuite : public CxxTest::TestSuite
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

    void test_append()
    {
        UniqueVector<int> vector;

        vector.append( 1 );
        vector.append( 2 );

        TS_ASSERT_EQUALS( vector.size(), 2U );

        vector.append( 1 );
        vector.append( 2 );

        TS_ASSERT_EQUALS( vector.size(), 2U );
        TS_ASSERT_EQUALS( vector[0], 1 );
        TS_ASSERT_EQUALS( vector[1], 2 );

        vector.append( 3 );
        TS_ASSERT_EQUALS( vector.size(), 3U );

        vector.append( 3 );
        TS_ASSERT_EQUALS( vector.size(), 3U );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

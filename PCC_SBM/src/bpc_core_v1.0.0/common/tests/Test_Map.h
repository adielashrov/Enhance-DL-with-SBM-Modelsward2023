#include <cxxtest/TestSuite.h>

#include "Error.h"
#include "Map.h"
#include "MockErrno.h"
#include "StringTag.h"

class MapTestSuite : public CxxTest::TestSuite
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
        Map<unsigned, StringTag> map;

        map[1] = "Apple";
        map[2] = "Red";
        map[3] = "Tasty";

        TS_ASSERT_EQUALS( map[1], "Apple" );
        TS_ASSERT_EQUALS( map[2], "Red" );
        TS_ASSERT_EQUALS( map[3], "Tasty" );
    }

    void test_size_and_exists()
    {
        Map<unsigned, StringTag> map;

        TS_ASSERT_EQUALS( map.size(), 0u );
        TS_ASSERT( map.empty() );
        TS_ASSERT( !map.exists( 1 ) );

        map[1] = "Apple";
        TS_ASSERT_EQUALS( map.size(), 1u );
        TS_ASSERT( !map.empty() );
        TS_ASSERT( map.exists( 1 ) );

        map[2] = "Red";
        TS_ASSERT_EQUALS( map.size(), 2u );
        TS_ASSERT( map.exists( 1 ) );
        TS_ASSERT( map.exists( 2 ) );
        TS_ASSERT( !map.exists( 3 ) );

        map[3] = "Tasty";
        TS_ASSERT_EQUALS( map.size(), 3u );
        TS_ASSERT( map.exists( 1 ) );
        TS_ASSERT( map.exists( 2 ) );
        TS_ASSERT( map.exists( 3 ) );
        TS_ASSERT( !map.exists( 4 ) );

        map[2] = "OtherRed";
        TS_ASSERT_EQUALS( map.size(), 3u );
        TS_ASSERT( map.exists( 1 ) );
        TS_ASSERT( map.exists( 2 ) );
        TS_ASSERT( map.exists( 3 ) );
        TS_ASSERT( !map.exists( 4 ) );
    }

    void test_erase()
    {
        Map<unsigned, StringTag> map;

        map[1] = "Apple";
        TS_ASSERT( map.exists( 1 ) );
        TS_ASSERT_THROWS_NOTHING( map.erase( 1 ) );
        TS_ASSERT( !map.exists( 1 ) );
        TS_ASSERT_EQUALS( map.size(), 0U );

        map[2] = "Red";
        map[3] = "Tasty";
        TS_ASSERT( !map.exists( 1 ) );
        TS_ASSERT( map.exists( 2 ) );
        TS_ASSERT( map.exists( 3 ) );

        TS_ASSERT_THROWS_NOTHING( map.erase( 3 ) );
        TS_ASSERT( !map.exists( 1 ) );
        TS_ASSERT( map.exists( 2 ) );
        TS_ASSERT( !map.exists( 3 ) );

        TS_ASSERT_EQUALS( map[2], "Red" );

        TS_ASSERT_THROWS_EQUALS( map.erase( 1 ),
                                 const Error &e,
                                 e.code(),
                                 Error::KEY_DOESNT_EXIST_IN_MAP );
    }

    void test_iterating()
    {
        Map<unsigned, StringTag> map;

        map[1] = "Apple";
        map[2] = "Red";
        map[3] = "Tasty";

        Map<unsigned, StringTag>::iterator it;
        it = map.begin();

        TS_ASSERT_EQUALS( it->first, 1U );
        TS_ASSERT_EQUALS( it->second, "Apple" );

        ++it;

        TS_ASSERT_EQUALS( it->first, 2U );
        TS_ASSERT_EQUALS( it->second, "Red" );

        ++it;

        TS_ASSERT_EQUALS( it->first, 3U );
        TS_ASSERT_EQUALS( it->second, "Tasty" );

        ++it;

        TS_ASSERT_EQUALS( it, map.end() );
    }

    void test_clear()
    {
        Map<unsigned, StringTag> map;

        map[1] = "Apple";
        map[2] = "Red";
        map[3] = "Tasty";

        TS_ASSERT_EQUALS( map.size(), 3U );

        TS_ASSERT_THROWS_NOTHING( map.clear() );
        TS_ASSERT_EQUALS( map.size(), 0U );

        map[1] = "Apple";

        TS_ASSERT_THROWS_NOTHING( map.clear() );
        TS_ASSERT_EQUALS( map.size(), 0U );

        TS_ASSERT_THROWS_NOTHING( map.clear() );
        TS_ASSERT_EQUALS( map.size(), 0U );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

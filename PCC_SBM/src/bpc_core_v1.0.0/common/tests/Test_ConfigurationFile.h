#include <cxxtest/TestSuite.h>

#include "ConfigurationFile.h"
#include "MockErrno.h"
#include "MockFileFactory.h"

class MockForConfigurationFile :
    public MockErrno,
    public MockFileFactory
{
public:
};

class ConfigurationFileTestSuite : public CxxTest::TestSuite
{
public:
    MockForConfigurationFile *mock;
    MockFile *file;

    void setUp()
    {
        TS_ASSERT( mock = new MockForConfigurationFile );

        file = &( mock->mockFile );
    }

    void tearDown()
    {
        TS_ASSERT_THROWS_NOTHING( delete mock );
    }

    void test_constructor()
    {
        ConfigurationFile *configFile;

        TS_ASSERT( configFile = new ConfigurationFile( "/home/config.ini" ) );

        TS_ASSERT( file->wasCreated );
        TS_ASSERT_EQUALS( file->lastPath, "/home/config.ini" );

        TS_ASSERT_THROWS_NOTHING( delete configFile );

        TS_ASSERT( file->wasDiscarded );
    }

    void test_getValue()
    {
        file->nextReadLines.append( "Title\n" );
        file->nextReadLines.append( "-------------\n" );
        file->nextReadLines.append( "  a = 1\n" );
        file->nextReadLines.append( "color =red\n" );
        file->nextReadLines.append( "blabla\n" );
        file->nextReadLines.append( "name= jason  \n" );
        file->nextReadLines.append( "!!@@!\n" );

        ConfigurationFile configurationFile( "config.ini" );

        TS_ASSERT_EQUALS( configurationFile.getValue( "a" ), "1" );
        TS_ASSERT_EQUALS( configurationFile.getValue( "color" ), "red" );
        TS_ASSERT_EQUALS( configurationFile.getValue( "name" ), "jason" );

        TS_ASSERT_THROWS_EQUALS( configurationFile.getValue( "doesnt exist" ),
                                 const Error &e,
                                 e.code(),
                                 Error::VALUE_DOESNT_EXIST_IN_CONFIG_FILE );

        TS_ASSERT_THROWS_EQUALS( configurationFile.getValue( "blabla" ),
                                 const Error &e,
                                 e.code(),
                                 Error::VALUE_DOESNT_EXIST_IN_CONFIG_FILE );

        TS_ASSERT_THROWS_EQUALS( configurationFile.getValue( "!!@@!" ),
                                 const Error &e,
                                 e.code(),
                                 Error::VALUE_DOESNT_EXIST_IN_CONFIG_FILE );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

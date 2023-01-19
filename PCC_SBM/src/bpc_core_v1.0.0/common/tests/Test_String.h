#include <cxxtest/TestSuite.h>

#include "StringTag.h"

class StringTestSuite : public CxxTest::TestSuite
{
public:
	void test_ascii()
	{
		char data[] = { "hello world" };

		StringTag string( data );

		TS_ASSERT_EQUALS( strlen( data ), strlen( string.ascii() ) );
		TS_ASSERT_SAME_DATA( data, string.ascii(), strlen( data ) );
	}

	void test_bracket_operator()
	{
	  	StringTag myString( "red apple" );

		TS_ASSERT_EQUALS( myString[0], 'r' );
		TS_ASSERT_EQUALS( myString[3], ' ' );
		TS_ASSERT_EQUALS( myString[8], 'e' );
	}

	void test_empty_constructor()
	{
		StringTag string;

		TS_ASSERT_EQUALS( string.length(), 0U );
	}

	void test_assignemnt_from_c_str()
	{
		StringTag myString;

		TS_ASSERT_EQUALS( myString.length(), 0U );

		myString = "apple";

		TS_ASSERT_EQUALS( myString.length(), strlen( "apple" ) );
		TS_ASSERT_SAME_DATA( myString.ascii(), "apple", myString.length() );
	}

	void test_equality_operator()
	{
		StringTag string( "apple" );
		StringTag otherString( "apple" );
		StringTag different( "banana" );

		TS_ASSERT( string == otherString );
		TS_ASSERT( !( string == different ) );
		TS_ASSERT( string != different );

		TS_ASSERT_EQUALS( string, "apple" );
		TS_ASSERT_DIFFERS( string, "banana" );

        StringTag otherApple( "apple" );

        TS_ASSERT_EQUALS( string, otherString );
        TS_ASSERT_EQUALS( string, otherString.ascii() );
    }

    void test_constructor_limited_length()
    {
        char data[] = "hello world";

        StringTag string( data, 6 );

        StringTag expected( "hello " );
        TS_ASSERT_EQUALS( string, expected );
    }

    void test_comparison_operator()
    {
        TS_ASSERT( StringTag( "a" ) < StringTag( "b" ) );
        TS_ASSERT( StringTag( "aa" ) < StringTag( "ab" ) );
    }

    void test_concatenation()
    {
        StringTag line1 = "hello";
        StringTag line2 = " world";
        StringTag expected = "hello world";

        TS_ASSERT_EQUALS( line1 + line2, expected );

        line1 += line2;

        TS_ASSERT_EQUALS( line1, expected );

        line1 = "hello";

        StringTag line3 = line1 + " world";

        TS_ASSERT_EQUALS( line3, expected );

        line1 += " world";

        TS_ASSERT_EQUALS( line1, expected );
    }

    void test_tokenize()
    {
        StringTag string = "This is a string with several    space  separated words";

        Vector<StringTag> tokens = string.tokenize();

        TS_ASSERT_EQUALS( tokens.size(), 9U );

        TS_ASSERT_EQUALS( tokens[0], "This" );
        TS_ASSERT_EQUALS( tokens[1], "is" );
        TS_ASSERT_EQUALS( tokens[2], "a" );
        TS_ASSERT_EQUALS( tokens[3], "string" );
        TS_ASSERT_EQUALS( tokens[4], "with" );
        TS_ASSERT_EQUALS( tokens[5], "several" );
        TS_ASSERT_EQUALS( tokens[6], "space" );
        TS_ASSERT_EQUALS( tokens[7], "separated" );
        TS_ASSERT_EQUALS( tokens[8], "words" );

        StringTag empty;

        tokens = empty.tokenize();
        TS_ASSERT_EQUALS( tokens.size(), 0U );
    }

    void test_contains()
    {
        StringTag string( "This is a simple string" );

        TS_ASSERT( string.contains( "This is" ) );
        TS_ASSERT( string.contains( "a simple" ) );
        TS_ASSERT( string.contains( " string" ) );
        TS_ASSERT( string.contains( "" ) );
        TS_ASSERT( string.contains( " " ) );

        TS_ASSERT( !string.contains( "Thisis" ) );
        TS_ASSERT( !string.contains( "crazy" ) );
    }

    void test_find()
    {
        StringTag string( "This is a simple string" );

        TS_ASSERT_EQUALS( string.find( "is a" ), 5U );
        TS_ASSERT_EQUALS( string.find( " " ), 4U );
        TS_ASSERT_EQUALS( string.find( "simple string" ), 10U );

        TS_ASSERT_EQUALS( string.find( "crazy" ), StringTag::Super::npos );
    }

    void test_substring()
    {
        StringTag string( "This is a simple string" );

        TS_ASSERT_EQUALS( string.substring( 0, 4 ), "This" );
        TS_ASSERT_EQUALS( string.substring( 4, 3 ), " is" );
        TS_ASSERT_EQUALS( string.substring( 7, 3 ), " a " );
    }

    void test_repalce()
    {
        StringTag string( "As I was going to st Ives" );

        string.replace( "going", "running" );

        TS_ASSERT_EQUALS( string, StringTag( "As I was running to st Ives" ) );

        string.replace( "I", "you" );

        TS_ASSERT_EQUALS( string, StringTag( "As you was running to st Ives" ) );

        TS_ASSERT_THROWS_NOTHING( string.replace( "bla", "other bla" ) );

        TS_ASSERT_EQUALS( string, StringTag( "As you was running to st Ives" ) );
    }

    void test_trim()
    {
        TS_ASSERT_EQUALS( StringTag( "   hello      world     \n" ).trim() , "hello      world" );
        TS_ASSERT_EQUALS( StringTag( "              \n" ).trim() , "" );
        TS_ASSERT_EQUALS( StringTag( "   a" ).trim() , "a" );
        TS_ASSERT_EQUALS( StringTag( "   ab\n" ).trim() , "ab" );
        TS_ASSERT_EQUALS( StringTag( "ab\n   \n" ).trim() , "ab" );
        TS_ASSERT_EQUALS( StringTag( "hey there   \n" ).trim() , "hey there" );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

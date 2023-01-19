#include <cxxtest/TestSuite.h>

#include "Error.h"
#include "MockErrno.h"

class ErrorTestSuite : public CxxTest::TestSuite
{
public:
	void test_code_and_errno()
	{
        MockErrno mockErrno;

        mockErrno.nextErrno = 21;

		Error error1( Error::NOT_ENOUGH_MEMORY );
		TS_ASSERT_EQUALS( error1.code(), Error::NOT_ENOUGH_MEMORY );
        TS_ASSERT_EQUALS( error1.errnoValue(), 21 );

        mockErrno.nextErrno = 23;

		Error error2( (Error::Code)166 );
		TS_ASSERT_EQUALS( error2.code(), (Error::Code)166 );
        TS_ASSERT_EQUALS( error2.errnoValue(), 23 );
	}
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __AutomatonContainmentChecker_h__
#define __AutomatonContainmentChecker_h__

#include "File.h"
#include "Stringf.h"

const char *GOAL_EXECUTABLE = "/home/guy/projects/prime_multiples/core/tools/GOAL-20120409/goal ";
const char *TEMP_FILE = "/home/guy/temp.data";

class AutomatonContainmentChecker
{
public:
    static bool contained( const StringTag &containee, const StringTag &container )
    {
        Stringf flags( "containment -m modifiedsafra %s %s > %s", containee.ascii(), container.ascii(), TEMP_FILE );
        system( ( StringTag( GOAL_EXECUTABLE ) + flags ).ascii() );

        return readResult();
    }

    static bool readResult()
    {
        File resultFile( TEMP_FILE );

        resultFile.open( IFile::MODE_READ );

        HeapData buffer;

        resultFile.read( buffer, 10 );

        char POSITIVE[] = "(true";
        char NEGATIVE[] = "(false";

        if ( memcmp( buffer.data(), POSITIVE, strlen( POSITIVE ) ) == 0 )
        {
            return true;
        }
        else if ( memcmp( buffer.data(), NEGATIVE, strlen( NEGATIVE ) ) == 0 )
        {
            return false;
        }

        throw Error( Error::UNKNOWN_GOAL_RESULT );
    }
};

#endif // __AutomatonContainmentChecker_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __TestLog_h__
#define __TestLog_h__

#include <cxxtest/TestSuite.h>

#include "StringTag.h"
#include "Vector.h"

class TestLog
{
public:
    static Vector<StringTag> _log;

    static void log( const StringTag &message )
    {
        _log.append( message );
    }

    static bool happened( const StringTag &message )
    {
        return _log.exists( message );
    }

    static void clear()
    {
        _log.clear();
    }

    static bool preceded( const StringTag &before, const StringTag &after )
    {
        TS_ASSERT( happened( before ) );
        TS_ASSERT( happened( after ) );

        for ( unsigned i = 0; i < _log.size(); ++i )
        {
            if ( _log[i] == before )
            {
                return true;
            }
            if ( _log[i] == after )
            {
                return false;
            }
        }

        return true;
    }
};

#endif // __TestLog_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

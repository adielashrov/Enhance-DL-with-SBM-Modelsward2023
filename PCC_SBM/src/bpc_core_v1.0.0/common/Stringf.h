#ifndef __Stringf_h__
#define __Stringf_h__

#include "StringTag.h"
#include <cstdarg>
#include <cstdio>

class Stringf : public StringTag
{
public:
    enum {
        MAX_STRING_LENGTH = 512,
    };

    Stringf( const char *format, ... )
    {
        va_list argList;
        va_start( argList, format );

        char buffer[MAX_STRING_LENGTH];

        vsprintf( buffer, format, argList );

        va_end( argList );

        _super = Super( buffer );
    }
};

// #ifdef CXXTEST_RUNNING
// namespace CxxTest { CXXTEST_COPY_TRAITS( Stringf, std::string ); }
// #endif // CXXTEST_RUNNING

#endif // __Stringf_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

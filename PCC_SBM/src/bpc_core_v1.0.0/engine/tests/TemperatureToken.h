#ifndef __TemperatureToken_h__
#define __TemperatureToken_h__

#include "ThreadState.h"

class TemperatureToken
{
public:
    virtual bool hot( const ThreadState &state ) = 0;
};

#endif // __TemperatureToken_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

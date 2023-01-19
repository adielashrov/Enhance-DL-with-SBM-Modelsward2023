#ifndef __IEngine_h__
#define __IEngine_h__

#include "IThreaded.h"
#include "Vector.h"

class StringTag;


class IEngine : public IThreaded
{
public:
    virtual ~IEngine() {}
    virtual void addSensorThread( const StringTag &threadName ) = 0;
};

#endif // __IEngine_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

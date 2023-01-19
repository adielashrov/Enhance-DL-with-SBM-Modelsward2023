#ifndef __IBThread_h__
#define __IBThread_h__

#include "IThreaded.h"
#include "StringTag.h"
#include "Vector.h"

class Event;

class IBThread : public IThreaded
{
public:
    virtual void bSync( const Vector<Event> &requested,
                        const Vector<Event> &watched,
                        const Vector<Event> &blocked,
                        StringTag = "" ) = 0;

    virtual void done() = 0;
};

#endif // __IBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

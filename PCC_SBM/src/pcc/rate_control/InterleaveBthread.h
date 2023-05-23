#ifndef __interleaveBThread_h__
#define __interleaveBThread_h__

#include "Event.h"
#include "BThread.h"
#include <cstdlib>
#include <unistd.h>

class InterleaveBThread : public BThread
{
    public:
        InterleaveBThread();
        ~InterleaveBThread();
        void entryPoint();
};

#endif // __monitorIntervalSensorBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
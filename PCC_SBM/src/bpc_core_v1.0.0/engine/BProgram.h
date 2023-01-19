#ifndef __BProgram_h__
#define __BProgram_h__

#include "AutoEngine.h"
#include "IBThread.h"
#include "Vector.h"

#include <unistd.h>

class BProgram
{
public:
    void addThread( IBThread &thread )
    {
        _threads.append( &thread );
    }

    void addSensorThread( const StringTag &threadName )
    {
        _engine->addSensorThread( threadName );
    }

    void runProgram()
    {
        runThreads();
    }

    void runEngine()
    {
        _engine->run();
    }

private:
    AutoEngine _engine;
    Vector<IBThread *> _threads;

    void runThreads()
    {
        for ( unsigned i = 0; i < _threads.size(); ++i )
        {
            _threads[i]->run();
        }
    }
};

#endif // __BProgram_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

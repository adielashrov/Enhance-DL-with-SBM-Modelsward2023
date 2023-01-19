#ifndef __ProcessRunner_h__
#define __ProcessRunner_h__

#include "Error.h"
#include "IProcessRunner.h"
#include "T/unistd.h"

class ProcessRunner : public IProcessRunner
{
public:
    ProcessRunner( IProcessRunner::IRunable &child ) : _child( &child )
    {
    }

    pid_t run()
    {
        fork();

        if ( returnedAsChild() )
        {
            _child->run();
            T::_exit( 0 );
            return 0;
        }
        else
        {
            return _returnedPid;
        }
    }

private:
    IProcessRunner::IRunable *_child;
    pid_t _returnedPid;

    void fork()
    {
        if ( ( _returnedPid = T::fork() ) < 0 )
        {
            throw Error( Error::FORK_FAILED );
        }
    }

    bool returnedAsChild() const
    {
        return _returnedPid == 0;
    }
};

#endif // __ProcessRunner_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

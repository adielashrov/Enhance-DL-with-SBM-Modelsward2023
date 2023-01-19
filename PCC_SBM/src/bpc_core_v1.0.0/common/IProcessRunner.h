#ifndef __IProcessRunner_h__
#define __IProcessRunner_h__

#include <unistd.h>

class IProcessRunner
{
public:

    class IRunable
    {
    public:

        virtual void run() = 0;
        virtual ~IRunable() {}
    };

    virtual pid_t run() = 0;
    virtual ~IProcessRunner() {}
};

#endif // __IProcessRunner_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

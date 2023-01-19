#ifndef __AutoProcessRunner_h__
#define __AutoProcessRunner_h__

#include "IProcessRunner.h"
#include "T/ProcessRunnerFactory.h"

class AutoProcessRunner
{
public:
	AutoProcessRunner( IProcessRunner::IRunable &child )
	{
		_processRunner = T::createProcessRunner( child );
	}

	~AutoProcessRunner()
	{
		T::discardProcessRunner( _processRunner );
		_processRunner = 0;
	}

	operator IProcessRunner &()
	{
		return *_processRunner;
	}

	IProcessRunner *operator->()
	{
		return _processRunner;
	}

private:
	IProcessRunner *_processRunner;
};

#endif // __AutoProcessRunner_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __AutoEngine_h__
#define __AutoEngine_h__

#include "IEngine.h"
#include "T/EngineFactory.h"

class AutoEngine
{
public:
	AutoEngine()
	{
		_engine = T::createEngine();
	}

	~AutoEngine()
	{
		T::discardEngine( _engine );
		_engine = 0;
	}

	operator IEngine &()
	{
		return *_engine;
	}

	IEngine *operator->()
	{
		return _engine;
	}

private:
	IEngine *_engine;
};

#endif // __AutoEngine_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

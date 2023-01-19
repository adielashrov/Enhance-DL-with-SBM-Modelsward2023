#ifndef __AutoWeaver_h__
#define __AutoWeaver_h__

#include "IWeaver.h"
#include "T/WeaverFactory.h"

class AutoWeaver
{
public:
	AutoWeaver( ISelector &selector )
	{
		_weaver = T::createWeaver( selector );
	}

	~AutoWeaver()
	{
		T::discardWeaver( _weaver );
		_weaver = 0;
	}

	operator IWeaver &()
	{
		return *_weaver;
	}

	IWeaver *operator->()
	{
		return _weaver;
	}

private:
	IWeaver *_weaver;
};

#endif // __AutoWeaver_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __AutoThreadListener_h__
#define __AutoThreadListener_h__

#include "IThreadListener.h"
#include "T/ThreadListenerFactory.h"

class AutoThreadListener
{
public:
	AutoThreadListener( IWeaver &weaver, ISelector &selector, ISocket &socket )
	{
		_threadListener = T::createThreadListener( weaver, selector, socket );
	}

	~AutoThreadListener()
	{
		T::discardThreadListener( _threadListener );
		_threadListener = 0;
	}

	operator IThreadListener &()
	{
		return *_threadListener;
	}

	IThreadListener *operator->()
	{
		return _threadListener;
	}

private:
	IThreadListener *_threadListener;
};

#endif // __AutoThreadListener_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

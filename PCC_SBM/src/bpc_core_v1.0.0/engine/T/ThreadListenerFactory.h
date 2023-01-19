#ifndef __T__ThreadListenerFactory_h__
#define __T__ThreadListenerFactory_h__

#include "cxxtest/Mock.h"

class ISelector;
class ISocket;
class IThreadListener;
class IWeaver;

namespace T
{
	IThreadListener *createThreadListener( IWeaver &weaver, ISelector &selector, ISocket &socket );
	void discardThreadListener( IThreadListener *threadListener );
}

CXXTEST_SUPPLY( createThreadListener,
				IThreadListener *,
				createThreadListener,
				( IWeaver &weaver, ISelector &selector, ISocket &socket ),
				T::createThreadListener,
				( weaver, selector, socket ) );

CXXTEST_SUPPLY_VOID( discardThreadListener,
					 discardThreadListener,
					 ( IThreadListener *threadListener ),
					 T::discardThreadListener,
					 ( threadListener ) );

#endif // __T__ThreadListenerFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

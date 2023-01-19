#ifndef __MockServerSocket_h__
#define __MockServerSocket_h__

#include "ConstSimpleData.h"
#include "HeapData.h"
#include "IServerSocket.h"

class MockServerSocket : public IServerSocket
{
public:
	MockServerSocket()
	{
		wasCreated = false;
		wasDiscarded = false;
	}
	
	bool wasCreated;
	bool wasDiscarded;

	ISelector *lastSelector;
	unsigned lastPort;
	IServerSocket::ISocketAccepter *lastAccepter;
	
	void mockConstructor( ISelector &selector, unsigned port, IServerSocket::ISocketAccepter &accepter )
	{
		TS_ASSERT( !wasCreated );
		wasCreated = true;

		lastSelector = &selector;
		lastPort = port;
		lastAccepter = &accepter;
	}
	
	void mockDestructor()
	{
		TS_ASSERT( wasCreated );
		TS_ASSERT( !wasDiscarded );
		wasDiscarded = true;
	}
	
	void canRead()
	{
	}

	int descriptor()
	{
		return 0;
	}
};

#endif // __MockServerSocket_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __MockSocketFactory_h__
#define __MockSocketFactory_h__

#include "MockSocket.h"
#include "T/SocketFactory.h"

class MockSocketFactory :
	public T::Base_createSocket,
    public T::Base_createSocketIpPort,
	public T::Base_discardSocket
{
public:
	MockSocket mockSocket;

	~MockSocketFactory()
	{
		if ( mockSocket.wasCreatedIpPort )
		{
			TS_ASSERT( mockSocket.wasDiscarded );
		}
	}

	ISocket *createSocket( int descriptor )
	{
		mockSocket.mockConstructor( descriptor );
		return &mockSocket;
	}

	ISocket *createSocketIpPort( unsigned ip, unsigned port )
	{
		mockSocket.mockConstructor( ip, port );
		return &mockSocket;
	}

	void discardSocket( ISocket *socket )
	{
		TS_ASSERT_EQUALS( socket, &mockSocket );
		mockSocket.mockDestructor();
	}
};

#endif // __MockSocketFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

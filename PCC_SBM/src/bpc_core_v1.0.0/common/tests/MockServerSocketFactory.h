#ifndef __MockServerSocketFactory_h__
#define __MockServerSocketFactory_h__

#include "MockServerSocket.h"
#include "T/ServerSocketFactory.h"

class MockServerSocketFactory : 
	public T::Base_createServerSocket,
	public T::Base_discardServerSocket
{
public:
	MockServerSocket mockServerSocket;

	~MockServerSocketFactory()
	{
		if ( mockServerSocket.wasCreated )
		{
			TS_ASSERT( mockServerSocket.wasDiscarded );
		}
	}
	
	IServerSocket *createServerSocket( ISelector &selector, unsigned port, IServerSocket::ISocketAccepter &accepter )
	{
		mockServerSocket.mockConstructor( selector, port, accepter );
		return &mockServerSocket;
	}
	
	void discardServerSocket( IServerSocket *serverSocket )
	{
		TS_ASSERT_EQUALS( serverSocket, &mockServerSocket );
		mockServerSocket.mockDestructor();
	}
};

#endif // __MockServerSocketFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __AutoServerSocket_h__
#define __AutoServerSocket_h__

#include "ISelector.h"
#include "IServerSocket.h"
#include "T/ServerSocketFactory.h"

class AutoServerSocket
{
public:
	AutoServerSocket( ISelector &selector, unsigned port, IServerSocket::ISocketAccepter &accepter ) 
	{
		_serverSocket = T::createServerSocket( selector, port, accepter );
	}

	~AutoServerSocket()
	{
		T::discardServerSocket( _serverSocket );
		_serverSocket = 0;
	}
	
	operator IServerSocket &()
	{
		return *_serverSocket;
	}

	IServerSocket &operator->()
	{
		return *_serverSocket;
	}
	
private:
	IServerSocket *_serverSocket;
};

#endif // __AutoServerSocket_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

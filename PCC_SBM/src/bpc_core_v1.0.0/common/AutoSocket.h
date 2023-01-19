#ifndef __AutoSocket_h__
#define __AutoSocket_h__

#include "ISelector.h"
#include "ISocket.h"
#include "T/SocketFactory.h"

class AutoSocket
{
public:
	AutoSocket( unsigned ip, unsigned port )
	{
		_socket = T::createSocketIpPort( ip, port );
	}

	~AutoSocket()
	{
		T::discardSocket( _socket );
		_socket = 0;
	}

	operator ISocket &()
	{
		return *_socket;
	}

	ISocket *operator->()
	{
		return _socket;
	}

private:
	ISocket *_socket;
};

#endif // __AutoSocket_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

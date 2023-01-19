#ifndef __ServerSocket_h__
#define __ServerSocket_h__

#include "Error.h"
#include "ISelector.h"
#include "IServerSocket.h"
#include "T/SocketFactory.h"
#include "T/sys/socket.h"
#include "T/unistd.h"
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>

#include <cstdio>

class ServerSocket : public IServerSocket
{
public:
	enum {
		MAX_NUMBER_OF_PENDING_CONNECTIONS = 10,
	};

	ServerSocket( ISelector &selector, unsigned port, IServerSocket::ISocketAccepter &accepter ) :
		_port( port ), _descriptor( 0 ), _selector( selector ), _accepter( accepter )
	{
		createSocket();
        allowPortReuse();
		callBind();
		callListen();
		registerToSelector();
	}

	~ServerSocket()
	{
		unregisterFromSelector();
		closeSocketIfNeeded();
	}

	void canRead()
	{
        _accepter.handleAcceptedSocket( *T::createSocket( accept() ) );
	}

	int descriptor()
	{
		return _descriptor;
	}

private:
	unsigned _port;
	int _descriptor;
	ISelector &_selector;
	IServerSocket::ISocketAccepter &_accepter;

	void registerToSelector()
	{
		_selector.registerToSelector( *this );
	}

	void unregisterFromSelector()
	{
		_selector.unregisterFromSelector( *this );
	}

	void createSocket()
	{
		const int SINGLE_PROTOCOL_IN_USE = 0;

		if ( ( _descriptor = T::socket( AF_INET, SOCK_STREAM, SINGLE_PROTOCOL_IN_USE ) ) == -1 )
		{
			throw Error( Error::SOCKET_CREATION_FAILED );
		}
	}

	void callBind()
	{
		struct sockaddr_in socketAddress;
		memset( &socketAddress, 0, sizeof(struct sockaddr_in) );

		socketAddress.sin_family = AF_INET;
		socketAddress.sin_addr.s_addr = INADDR_ANY;
		socketAddress.sin_port = htons( _port );

		if ( T::bind( _descriptor, (struct sockaddr *)&socketAddress, sizeof(struct sockaddr_in) ) == -1 )
		{
			closeSocket();
			throw Error( Error::BIND_FAILED );
		}
	}

	void callListen()
	{
		if ( T::listen( _descriptor, MAX_NUMBER_OF_PENDING_CONNECTIONS ) == -1 )
		{
			closeSocket();
			throw Error( Error::LISTEN_FAILED );
		}
	}

	int accept()
	{
    	// printf( "Enter Accept - ServerSocket...\n" );

		struct sockaddr_in clientAddress;
		memset( &clientAddress, 0, sizeof(struct sockaddr_in) );
		socklen_t addressLength;
		memset( &addressLength, 0, sizeof(addressLength) );

		int descriptor;

		if ( ( descriptor = T::accept( _descriptor, (struct sockaddr *)&clientAddress, &addressLength ) ) == -1 )
		{
			printf( "Caught error in method - ServerSocket - Accept...\n" );
			closeSocket();
			throw Error( Error::ACCEPT_FAILED );
		}

		// printf( "Accept - descriptor value: %d\n", descriptor );

		// printf( "Leave Accept - ServerSocket...\n" );

		return descriptor;
	}

	void closeSocketIfNeeded()
	{
		if ( _descriptor > 0 )
		{
			closeSocket();
		}
	}

	void closeSocket()
	{
		T::close( _descriptor );
		_descriptor = 0;
	}

    void allowPortReuse()
    {
        int setReuseToOn = 1;

        if ( T::setsockopt( _descriptor, SOL_SOCKET, SO_REUSEADDR, &setReuseToOn, sizeof(setReuseToOn) ) != 0 )
        {
			closeSocket();
            throw Error( Error::SET_SOCK_OPT_FAILED );
        }
    }
};

#endif // __ServerSocket_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

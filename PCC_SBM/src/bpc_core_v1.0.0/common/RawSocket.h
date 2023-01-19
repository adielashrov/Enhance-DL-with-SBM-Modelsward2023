#ifndef __RawSocket_h__
#define __RawSocket_h__

#include "ConstSimpleData.h"
#include "StringTag.h"
#include "T/sys/socket.h"
#include "T/unistd.h"
#include <arpa/inet.h>

class RawSocket
{
public:
    RawSocket( const StringTag &ipAddress ) : _ipAddress( ipAddress )
    {
    }

    ~RawSocket()
    {
        close();
    }

    void activate()
    {
		createSocket();
        callBind();
    }

    void close()
    {
        if ( _descriptor > 0 )
        {
            closeSocket();
        }
    }

    void receive( HeapData &data, struct sockaddr_in &sourceAddress )
    {
        static const int MAX_DATA_SIZE = 4096;
        static const int NO_FLAGS = 0;

        static char buffer[MAX_DATA_SIZE];
        memset( buffer, 0, sizeof(buffer) );

        size_t actualReceivedBytes;

        memset( &sourceAddress, 0, sizeof(sourceAddress) );
        socklen_t addressLength = sizeof(sourceAddress);

        if ( ( actualReceivedBytes = T::recvfrom( _descriptor, buffer, sizeof(buffer), NO_FLAGS, (sockaddr *)&sourceAddress, &addressLength ) ) == -1 )
        {
            throw Error( Error::RECV_FAILED );
        }

        data = ConstSimpleData( buffer, actualReceivedBytes );
    }

    void send( const ConstSimpleData &data, const StringTag &address )
    {
        const int NO_FLAGS = 0;

        struct sockaddr_in destinationAddress;
        memset( &destinationAddress, 0, sizeof(sockaddr_in) );
        inet_aton( address.ascii(), &destinationAddress.sin_addr );

        if ( T::sendto( _descriptor, data.data(), data.size(), NO_FLAGS, (struct sockaddr *)&destinationAddress, sizeof(destinationAddress) ) != (int)data.size() )
        {
            throw Error( Error::SEND_FAILED );
        }
    }

private:
    StringTag _ipAddress;
    int _descriptor;

    void createSocket()
	{
        if ( ( _descriptor = T::socket( AF_INET, SOCK_RAW, IPPROTO_TCP ) ) == -1 )
		{
			throw Error( Error::SOCKET_CREATION_FAILED );
		}
	}

    void callBind()
	{
		struct sockaddr_in socketAddress;
        prepareSocketAddress( socketAddress );

        if ( T::bind( _descriptor, (struct sockaddr *)&socketAddress, sizeof(struct sockaddr_in) ) == -1 )
		{
            closeSocket();
			throw Error( Error::BIND_FAILED );
		}
	}

    void prepareSocketAddress( struct sockaddr_in &socketAddress )
    {
        memset( &socketAddress, 0, sizeof(struct sockaddr_in) );

		socketAddress.sin_family = AF_INET;

        if ( inet_aton( _ipAddress.ascii(), &socketAddress.sin_addr ) == 0 )
        {
            closeSocket();
            throw Error( Error::INET_ATON_FAILED );
        }
    }

    void closeSocket()
    {
        T::close( _descriptor );
        _descriptor = 0;
    }
};

#endif // __RawSocket_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

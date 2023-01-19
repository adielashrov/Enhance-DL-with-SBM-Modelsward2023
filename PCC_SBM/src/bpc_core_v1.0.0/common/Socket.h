#ifndef __Socket_h__
#define __Socket_h__

#include "ConstSimpleData.h"
#include "HeapData.h"
#include "ISocket.h"
#include "T/sys/socket.h"
#include "T/unistd.h"
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>

class Socket : public ISocket
{
public:
    Socket( int descriptor ) : _descriptor( descriptor )
    {
    }

    Socket( unsigned ip, unsigned port ) : _descriptor( -1 )
    {
        createSocket();
        connect( ip, port );
    }

    ~Socket()
    {
        closeSocket();
    }

    int descriptor()
    {
        return _descriptor;
    }

    void send( const ConstSimpleData &data )
    {
        const int NO_FLAGS = 0;

        if ( T::send( _descriptor, data.data(), data.size(), NO_FLAGS ) != (int)data.size() )
        {
            throw Error( Error::SEND_FAILED );
        }
    }

    void receive( HeapData &data )
    {
        const int MAX_DATA_SIZE = 2048;
        const int NO_FLAGS = 0;

        char buffer[MAX_DATA_SIZE];
        size_t actualReceivedBytes = sizeof(buffer);

        actualReceivedBytes = T::recv( _descriptor, buffer, sizeof(buffer), NO_FLAGS );

        if ( actualReceivedBytes == 0 )
        {
            throw Error( Error::SOCKET_WAS_CLOSED );
        }
        else if ( (int)actualReceivedBytes == -1 )
        {
            throw Error( Error::RECV_FAILED );
        }

        data = ConstSimpleData( buffer, actualReceivedBytes );
    }

    void allowPortReuse()
    {
        int setReuseToOn = 1;

        if ( T::setsockopt( _descriptor, SOL_SOCKET, SO_REUSEADDR, &setReuseToOn, sizeof(setReuseToOn) ) != 0 )
        {
            throw Error( Error::SET_SOCK_OPT_FAILED );
        }
    }

    void closeSocket()
    {
        if ( _descriptor != -1 )
        {
            T::close( _descriptor );
            _descriptor = -1;
        }
    }

private:
    int _descriptor;

    static void setIpAndPort( struct sockaddr_in &address, unsigned ip, unsigned port )
    {
        memset( &address, 0, sizeof(address) );
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = ip;
        address.sin_port = htons( port );
    }

    void createSocket()
    {
        const int SINGLE_PROTOCOL_IN_USE = 0;

        if ( ( _descriptor = T::socket( PF_INET, SOCK_STREAM, SINGLE_PROTOCOL_IN_USE ) ) == -1 )
        {
            throw Error( Error::SOCKET_CREATION_FAILED );
        }
    }

    void connect( unsigned ip, unsigned port )
    {
        struct sockaddr_in address;

        setIpAndPort( address, ip, port );
        if ( T::connect( _descriptor, (struct sockaddr *)&address, sizeof(address) ) == -1 )
        {
            closeSocket();
            throw Error( Error::CONNECT_FAILED );
        }
    }
};

#endif // __Socket_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

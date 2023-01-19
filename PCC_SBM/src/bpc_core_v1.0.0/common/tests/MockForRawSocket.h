#ifndef __MockForRawSocket_h__
#define __MockForRawSocket_h__

#include "RealMalloc.h"
#include "T/sys/socket.h"
#include "T/unistd.h"
#include <arpa/inet.h>

class MockForRawSocket :
    public RealMalloc,
    public MockErrno,
	public T::Base_socket,
	public T::Base_bind,
	public T::Base_close,
    public T::Base_recvfrom,
    public T::Base_sendto
{
public:
	MockForRawSocket()
	{
		nextSocketDescriptor = 117;
		socketWasCalled = false;
        closeWasCalled = false;
        nextBindResult = 0;
        recvShouldFail = false;
        sendShouldFail = false;

        memset( &nextSourceAddress, 0, sizeof(struct sockaddr_in) );
    }

	bool socketWasCalled;
	int nextSocketDescriptor;

	int socket( int domain, int type, int protocol )
	{
		TS_ASSERT_EQUALS( domain, AF_INET );
		TS_ASSERT_EQUALS( type, SOCK_RAW );
        TS_ASSERT_EQUALS( protocol, 6 //IPPROTO_TCP
                          );

		socketWasCalled = true;

		return nextSocketDescriptor;
	}

	struct sockaddr_in lastBindSockaddr;
	int nextBindResult;
	bool bindWasCalled;

	int bind( int sockfd, const struct sockaddr *addr, socklen_t addrlen )
	{
		bindWasCalled = true;

		TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );
		TS_ASSERT_EQUALS( addrlen, sizeof(struct sockaddr_in) );
		memcpy( &lastBindSockaddr, addr, sizeof(lastBindSockaddr) );

		return nextBindResult;
	}

	bool closeWasCalled;

	int close( int sockfd )
	{
		TS_ASSERT( !closeWasCalled );
		TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );
		closeWasCalled = true;
		return 0;
	}

    HeapData nextReceiveData;
    struct sockaddr_in nextSourceAddress;
    bool recvShouldFail;

    ssize_t recvfrom( int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen )
    {
        if ( recvShouldFail )
        {
            return -1;
        }

        int NO_FLAGS = 0;

        TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );
        TS_ASSERT_EQUALS( flags, NO_FLAGS );

        TS_ASSERT_LESS_THAN_EQUALS( nextReceiveData.size(), len );
        memcpy( buf, nextReceiveData.data(), nextReceiveData.size() );

        TS_ASSERT_EQUALS( *addrlen, sizeof(struct sockaddr_in) );
        memcpy( src_addr, &nextSourceAddress, sizeof(sockaddr_in) );

        return nextReceiveData.size();
    }

    HeapData lastSentData;
    struct sockaddr_in lastSendAddress;
    bool sendShouldFail;

    ssize_t sendto( int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen )
    {
        if ( sendShouldFail )
        {
            return len - 1;
        }

        int NO_FLAGS = 0;

        TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );
        TS_ASSERT_EQUALS( flags, NO_FLAGS );

        lastSentData = ConstSimpleData( buf, len );

        TS_ASSERT_EQUALS( addrlen, sizeof(lastSendAddress) );
        memcpy( &lastSendAddress, dest_addr, sizeof(lastSendAddress) );

        return len;
    }
};

#endif // __MockForRawSocket_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

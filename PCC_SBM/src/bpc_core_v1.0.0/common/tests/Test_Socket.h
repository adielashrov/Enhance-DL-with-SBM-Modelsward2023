#include <cxxtest/TestSuite.h>

#include "MockErrno.h"
#include "RealMalloc.h"
#include "Socket.h"

class MockForSocket :
	public RealMalloc,
    public MockErrno,
	public T::Base_socket,
	public T::Base_connect,
	public T::Base_close,
	public T::Base_send,
	public T::Base_recv,
    public T::Base_setsockopt
{
public:
	MockForSocket()
	{
		nextSocketDescriptor = 117;
		socketWasCalled = false;
		nextConnectResult = 0;
		connectWasCalled = false;
		closeWasCalled = false;
		lastClosedDescriptor = 0;
		lastSendDescriptor = 0;
		lastRecieveDescriptor = 0;
		sendShouldFail = false;
		receiveShouldFail = false;
        setSockOptCalled = false;
        setSockOptShouldFail = false;
	}

	bool socketWasCalled;
	int nextSocketDescriptor;

	int socket( int domain, int type, int protocol )
	{
		TS_ASSERT_EQUALS( domain, PF_INET );
		TS_ASSERT_EQUALS( type, SOCK_STREAM );
		TS_ASSERT_EQUALS( protocol, 0 );

		socketWasCalled = true;

		return nextSocketDescriptor;
	}

	struct sockaddr_in lastConnectSockaddr;
	int nextConnectResult;
	bool connectWasCalled;

	int connect( int sockfd, const struct sockaddr *addr, socklen_t addrlen )
	{
		connectWasCalled = true;

		TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );
		TS_ASSERT_EQUALS( addrlen, sizeof(struct sockaddr_in) );
		memcpy( &lastConnectSockaddr, addr, sizeof(lastConnectSockaddr) );

		return nextConnectResult;
	}

	bool closeWasCalled;
	int lastClosedDescriptor;

	int close( int sockfd )
	{
		TS_ASSERT( !closeWasCalled );
		lastClosedDescriptor = sockfd;
		closeWasCalled = true;
		return 0;
	}

	int lastRecieveDescriptor;
	HeapData nextReceiveData;
	bool receiveShouldFail;
	int specialReceiveValue;

	ssize_t recv( int sockfd, void *buf, size_t len, int flags )
	{
		const int NO_FLAGS = 0;

		TS_ASSERT_EQUALS( flags, NO_FLAGS );
		TS_ASSERT_EQUALS( len, (size_t)2048 );

		lastRecieveDescriptor = sockfd;

		int bytesRead = std::min( (unsigned)len, nextReceiveData.size() );
		memcpy( buf, nextReceiveData.data(), bytesRead );

		if ( receiveShouldFail )
		{
			return specialReceiveValue;
		}

		return bytesRead;
	}

	int lastSendDescriptor;
	bool sendShouldFail;
	int specialSendValue;
	HeapData lastSentData;

	ssize_t send( int sockfd, const void *buf, size_t len, int flags )
	{
		const int NO_FLAGS = 0;

		TS_ASSERT_EQUALS( flags, NO_FLAGS );

		lastSendDescriptor = sockfd;

		lastSentData = ConstSimpleData( buf, len );

		if ( sendShouldFail )
		{
			return specialSendValue;
		}

		return len;
	}

    bool setSockOptCalled;
    bool setSockOptShouldFail;

    int setsockopt( int sockfd, int level, int optname, const void *optval, socklen_t optlen )
    {
        TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );
        TS_ASSERT_EQUALS( level, SOL_SOCKET );
        TS_ASSERT_EQUALS( optname, SO_REUSEADDR );
        TS_ASSERT_EQUALS( optlen, sizeof(int) );

        int value = *(int *)optval;
        const int REUSE_ON = 1;

        TS_ASSERT_EQUALS( value, REUSE_ON );

        setSockOptCalled = true;

        return setSockOptShouldFail ? -1 : 0;
    }
};

class SocketTestSuite : public CxxTest::TestSuite
{
public:
	MockForSocket *mock;

	void setUp()
	{
		TS_ASSERT( mock = new MockForSocket );
	}

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete mock );
	}

	void test_constructor__with_descriptor()
	{
		Socket *socket;

		TS_ASSERT( socket = new Socket( 165 ) );

		TS_ASSERT( !mock->socketWasCalled );
		TS_ASSERT( !mock->closeWasCalled );

        TS_ASSERT_EQUALS( socket->descriptor(), 165 );

		TS_ASSERT_THROWS_NOTHING( delete socket );

		TS_ASSERT( mock->closeWasCalled );
		TS_ASSERT_EQUALS( mock->lastClosedDescriptor, 165 );
	}

	void test_constructor__ip_and_port()
	{
		Socket *socket;

		TS_ASSERT_THROWS_NOTHING( socket = new Socket( 123321, 34 ) );

		TS_ASSERT( mock->socketWasCalled );

		struct sockaddr_in expectedAddress;
		memset( &expectedAddress, 0, sizeof(struct sockaddr_in) );

		expectedAddress.sin_family = AF_INET;
		expectedAddress.sin_addr.s_addr = 123321;
		expectedAddress.sin_port = htons( 34 );

		TS_ASSERT( mock->connectWasCalled );
		TS_ASSERT_SAME_DATA( &mock->lastConnectSockaddr, &expectedAddress, sizeof(expectedAddress) );

        TS_ASSERT_EQUALS( socket->descriptor(), mock->nextSocketDescriptor );

		TS_ASSERT_THROWS_NOTHING( delete socket );

		TS_ASSERT( mock->closeWasCalled );
		TS_ASSERT_EQUALS( mock->lastClosedDescriptor, mock->nextSocketDescriptor );
	}

    void test_close()
	{
		Socket *socket;

		TS_ASSERT( socket = new Socket( 165 ) );

		TS_ASSERT( !mock->closeWasCalled );

        TS_ASSERT_THROWS_NOTHING( socket->closeSocket() );

		TS_ASSERT( mock->closeWasCalled );
		TS_ASSERT_EQUALS( mock->lastClosedDescriptor, 165 );

		TS_ASSERT_THROWS_NOTHING( delete socket );
    }

	void test_constructor__ip_and_port__socket_fails()
	{
		Socket *socket;

		mock->nextSocketDescriptor = -1;

		TS_ASSERT_THROWS_EQUALS( socket = new Socket( 123321, 34 ),
								 const Error &e,
								 e.code(),
								 Error::SOCKET_CREATION_FAILED );

		TS_ASSERT( !mock->closeWasCalled );
	}

	void test_constructor__ip_and_port__connect_fails()
	{
		Socket *socket;

		mock->nextConnectResult = -1;

		TS_ASSERT_THROWS_EQUALS( socket = new Socket( 123321, 34 ),
								 const Error &e,
								 e.code(),
								 Error::CONNECT_FAILED );

		TS_ASSERT( mock->closeWasCalled );
		TS_ASSERT_EQUALS( mock->lastClosedDescriptor, mock->nextSocketDescriptor );
	}

	void test_send()
	{
		Socket *socket;

		TS_ASSERT( socket = new Socket( 165 ) );

		char buffer[] = { 'a', static_cast<char>(1), '5', 'b', static_cast<char>(2), static_cast<char>(194) };

		TS_ASSERT_THROWS_NOTHING( socket->send( ConstSimpleData( buffer, sizeof(buffer) ) ) );

		TS_ASSERT_EQUALS( mock->lastSendDescriptor, 165 );
		TS_ASSERT_EQUALS( mock->lastSentData.size(), sizeof(buffer) );
		TS_ASSERT_SAME_DATA( mock->lastSentData.data(), buffer, sizeof(buffer) );

		TS_ASSERT_THROWS_NOTHING( delete socket );
	}

	void test_send_fails()
	{
		Socket *socket;

		TS_ASSERT( socket = new Socket( 165 ) );

		char buffer[] = { 'a', static_cast<char>(1), '5', 'b', static_cast<char>(2), static_cast<char>(194) };

		mock->sendShouldFail = true;
		mock->specialSendValue = -1;

		TS_ASSERT_THROWS_EQUALS( socket->send( ConstSimpleData( buffer, sizeof(buffer) ) ),
								 const Error &e,
								 e.code(),
								 Error::SEND_FAILED );

		TS_ASSERT_THROWS_NOTHING( delete socket );
	}

	void test_send_fails__not_enough_data_sent()
	{
		Socket *socket;

		TS_ASSERT( socket = new Socket( 165 ) );

		char buffer[] = { 'a', static_cast<char>(1), '5', 'b', static_cast<char>(2), static_cast<char>(194) };

		mock->sendShouldFail = true;
		mock->specialSendValue = 3;

		TS_ASSERT_THROWS_EQUALS( socket->send( ConstSimpleData( buffer, sizeof(buffer) ) ),
								 const Error &e,
								 e.code(),
								 Error::SEND_FAILED );

		TS_ASSERT_THROWS_NOTHING( delete socket );
	}

	void test_receive()
	{
		Socket *socket;

		TS_ASSERT( socket = new Socket( 165 ) );

		char buffer[] = { 'a', static_cast<char>(1), '5', 'b', static_cast<char>(2), static_cast<char>(194) };
		mock->nextReceiveData = ConstSimpleData( buffer, sizeof(buffer) );

		HeapData receivedData;

		TS_ASSERT_THROWS_NOTHING( socket->receive( receivedData ) );

		TS_ASSERT_EQUALS( receivedData.size(), sizeof(buffer) );
		TS_ASSERT_SAME_DATA( receivedData.data(), buffer, sizeof(buffer) );

		TS_ASSERT_THROWS_NOTHING( delete socket );
	}

	void test_receive__error()
	{
		Socket *socket;

		TS_ASSERT( socket = new Socket( 165 ) );

		char buffer[] = { 'a', static_cast<char>(1), '5', 'b', static_cast<char>(2), static_cast<char>(194) };
		mock->nextReceiveData = ConstSimpleData( buffer, sizeof(buffer) );

		mock->receiveShouldFail = true;
		mock->specialReceiveValue = -1;

		HeapData receivedData;

		TS_ASSERT_THROWS_EQUALS( socket->receive( receivedData ),
								 const Error &e,
								 e.code(),
								 Error::RECV_FAILED );

		TS_ASSERT_THROWS_NOTHING( delete socket );
	}

	void test_receive__socket_closed()
	{
		Socket *socket;

		TS_ASSERT( socket = new Socket( 165 ) );

		char buffer[] = { 'a', static_cast<char>(1), '5', 'b', static_cast<char>(2), static_cast<char>(194) };
		mock->nextReceiveData = ConstSimpleData( buffer, sizeof(buffer) );

		mock->receiveShouldFail = true;
		mock->specialReceiveValue = 0;

		HeapData receivedData;

		TS_ASSERT_THROWS_EQUALS( socket->receive( receivedData ),
								 const Error &e,
								 e.code(),
								 Error::SOCKET_WAS_CLOSED );

		TS_ASSERT_THROWS_NOTHING( delete socket );
	}

    void test_allow_port_reuse()
    {
        Socket socket( 1654, 23 );

        TS_ASSERT_THROWS_NOTHING( socket.allowPortReuse() );

        TS_ASSERT( mock->setSockOptCalled );
    }

    void test_allow_port_reuse__set_sock_opt_fails()
    {
        Socket socket( 1654, 23 );

        mock->setSockOptShouldFail = true;

        TS_ASSERT_THROWS_EQUALS( socket.allowPortReuse(),
                                 const Error &e,
                                 e.code(),
                                 Error::SET_SOCK_OPT_FAILED );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

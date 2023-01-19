#include <cxxtest/TestSuite.h>

#include "Error.h"
#include "ISocket.h"
#include "MockErrno.h"
#include "MockSelector.h"
#include "MockSocketFactory.h"
#include "ServerSocket.h"

class SocketAccepterForTests : public IServerSocket::ISocketAccepter
{
public:
	SocketAccepterForTests()
	{
		lastAcceptedSocket = NULL;
	}

	ISocket *lastAcceptedSocket;

	void handleAcceptedSocket( ISocket &socket )
	{
		lastAcceptedSocket = &socket;
	}
};

class MockForServerSocket :
    public MockErrno,
	public T::Base_socket,
	public T::Base_bind,
	public T::Base_listen,
	public T::Base_accept,
	public T::Base_close,
    public T::Base_setsockopt,
	public MockSocketFactory
{
public:
	MockForServerSocket()
	{
		nextSocketDescriptor = 117;
		socketWasCalled = false;
		nextListenResult = 0;
		listenWasCalled = false;
		nextAcceptResult = 0;
		acceptWasCalled = false;
		nextBindResult = 0;
		bindWasCalled = false;
		closeWasCalled = false;
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

	bool listenWasCalled;
	int nextListenResult;

	int listen( int sockfd, int backlog )
	{
		TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );
		TS_ASSERT_EQUALS( backlog, 10 );

		TS_ASSERT( bindWasCalled );

		listenWasCalled = true;
		return nextListenResult;
	}

	bool acceptWasCalled;
	int nextAcceptResult;
	sockaddr_in nextAcceptSockaddr;

	int accept( int sockfd, struct sockaddr *addr, socklen_t *addrlen )
	{
		TS_ASSERT( listenWasCalled );
		TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );

		*addrlen = sizeof(sockaddr_in);
		memcpy( addr, &nextAcceptSockaddr, sizeof(sockaddr_in) );

		acceptWasCalled = true;

		return nextAcceptResult;
	}

	bool closeWasCalled;

	int close( int sockfd )
	{
		TS_ASSERT( !closeWasCalled );
		TS_ASSERT_EQUALS( sockfd, nextSocketDescriptor );
		closeWasCalled = true;
		return 0;
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

class ServerSocketTestSuite : public CxxTest::TestSuite
{
public:
	MockForServerSocket *mock;
	MockSelector *selector;
	MockSocket *socket;
	SocketAccepterForTests *accepter;

	void setUp()
	{
		TS_ASSERT( mock = new MockForServerSocket );
		TS_ASSERT( selector = new MockSelector );
		TS_ASSERT( accepter = new SocketAccepterForTests );

		socket = &( mock->mockSocket );
	}

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete accepter );
		TS_ASSERT_THROWS_NOTHING( delete selector );
		TS_ASSERT_THROWS_NOTHING( delete mock );
	}

	void test_construcor_and_accept()
	{
		ServerSocket *serverSocket;

		TS_ASSERT( serverSocket = new ServerSocket( *selector, 555, *accepter ) );

		mock->nextAcceptSockaddr.sin_family = AF_INET;
		mock->nextAcceptSockaddr.sin_port = htons( 444 );

		TS_ASSERT( selector->registerWasCalled );
		TS_ASSERT_EQUALS( selector->lastRegistetedDescriptor, mock->nextSocketDescriptor );
		TS_ASSERT( !selector->unregisterWasCalled );

		TS_ASSERT( mock->socketWasCalled );
		TS_ASSERT( mock->bindWasCalled );
		TS_ASSERT( mock->listenWasCalled );
        TS_ASSERT( mock->setSockOptCalled );
		TS_ASSERT( !mock->acceptWasCalled );

		sockaddr_in expectedBindSockaddr;
		memset( &expectedBindSockaddr, 0, sizeof(sockaddr_in ) );

		expectedBindSockaddr.sin_family = AF_INET;
		expectedBindSockaddr.sin_addr.s_addr = INADDR_ANY;
		expectedBindSockaddr.sin_port = htons( 555 );

		TS_ASSERT_SAME_DATA( &mock->lastBindSockaddr, &expectedBindSockaddr, sizeof(sockaddr_in) );

		mock->nextAcceptResult = 732;

		TS_ASSERT_THROWS_NOTHING( serverSocket->canRead() );

		TS_ASSERT( mock->acceptWasCalled );
		TS_ASSERT( !mock->closeWasCalled );

		TS_ASSERT( socket->wasCreated );
		TS_ASSERT_EQUALS( socket->lastDescriptor, 732 );
		TS_ASSERT_EQUALS( accepter->lastAcceptedSocket, socket );

		TS_ASSERT_THROWS_NOTHING( delete serverSocket );

		TS_ASSERT( mock->closeWasCalled );

		TS_ASSERT( selector->unregisterWasCalled );
		TS_ASSERT_EQUALS( selector->lastUnregistetedDescriptor, mock->nextSocketDescriptor );
	}

	void test_constructor__socket_fails()
	{
		mock->nextSocketDescriptor = -1;

		ServerSocket *serverSocket;

		TS_ASSERT_THROWS_EQUALS( serverSocket = new ServerSocket( *selector, 555, *accepter ),
								 const Error &e,
								 e.code(),
								 Error::SOCKET_CREATION_FAILED );

		TS_ASSERT( !selector->registerWasCalled );
		TS_ASSERT( !mock->closeWasCalled );
	}

	void test_constructor__setsockopt_fails()
	{
		mock->setSockOptShouldFail = true;

		ServerSocket *serverSocket;

		TS_ASSERT_THROWS_EQUALS( serverSocket = new ServerSocket( *selector, 555, *accepter ),
								 const Error &e,
								 e.code(),
								 Error::SET_SOCK_OPT_FAILED );

		TS_ASSERT( !selector->registerWasCalled );
		TS_ASSERT( mock->closeWasCalled );
	}

	void test_constructor__bind_fails()
	{
		mock->nextBindResult = -1;

		ServerSocket *serverSocket;

		TS_ASSERT_THROWS_EQUALS( serverSocket = new ServerSocket( *selector, 555, *accepter ),
								 const Error &e,
								 e.code(),
								 Error::BIND_FAILED );

		TS_ASSERT( !selector->registerWasCalled );
		TS_ASSERT( !selector->unregisterWasCalled );
		TS_ASSERT( mock->closeWasCalled );
	}

	void test_constructor__listen_fails()
	{
		mock->nextListenResult = -1;

		ServerSocket *serverSocket;

		TS_ASSERT_THROWS_EQUALS( serverSocket = new ServerSocket( *selector, 555, *accepter ),
								 const Error &e,
								 e.code(),
								 Error::LISTEN_FAILED );

		TS_ASSERT( !selector->registerWasCalled );
		TS_ASSERT( !selector->unregisterWasCalled );
		TS_ASSERT( mock->closeWasCalled );
	}

	void test_can_read__accept_fails()
	{
		mock->nextAcceptResult = -1;

		ServerSocket *serverSocket;

		TS_ASSERT( serverSocket = new ServerSocket( *selector, 555, *accepter ) );

		TS_ASSERT_THROWS_EQUALS( serverSocket->canRead(),
								 const Error &e,
								 e.code(),
								 Error::ACCEPT_FAILED );

		TS_ASSERT( !selector->unregisterWasCalled );

		TS_ASSERT_THROWS_NOTHING( delete serverSocket );

		TS_ASSERT( selector->unregisterWasCalled );
		TS_ASSERT( mock->closeWasCalled );
	}

	void test_close()
	{
		ServerSocket *serverSocket;

		TS_ASSERT( serverSocket = new ServerSocket( *selector, 555, *accepter ) );

		TS_ASSERT_THROWS_NOTHING( delete serverSocket );

		TS_ASSERT( mock->closeWasCalled );
	}
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

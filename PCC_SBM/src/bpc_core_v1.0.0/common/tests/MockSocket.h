#ifndef __MockSocket_h__
#define __MockSocket_h__

#include "ConstSimpleData.h"
#include "HeapData.h"
#include "ISocket.h"
#include "TestLog.h"

class MockSocket : public ISocket
{
public:
    class ReceiveCallBack
    {
    public:
        virtual void receiveWasCalled() = 0;
        virtual ~ReceiveCallBack() {}
    };

	MockSocket()
	{
		wasCreated = false;
		wasDiscarded = false;
        wasCreatedIpPort = false;
		lastDescriptor = 726;
        receiveWasCalled = false;
        allowPortReuseWasCalled = false;

        socketShouldThrow = false;
        socketShouldClose = false;
        closeSocketWasCalled = false;
        receiveIndex = 0;
        receiveCallBack = 0;
    }

	bool wasCreated;
    bool wasCreatedIpPort;
	bool wasDiscarded;
	int lastDescriptor;

	void mockConstructor( int descriptor )
	{
		TS_ASSERT( !wasCreated );
        TS_ASSERT( !wasCreatedIpPort );
		wasCreated = true;

		lastDescriptor = descriptor;
	}

    unsigned lastIp;
    unsigned lastPort;

	void mockConstructor( unsigned ip, unsigned port )
	{
		TS_ASSERT( !wasCreated );
        TS_ASSERT( !wasCreatedIpPort );
		wasCreatedIpPort = true;

        lastIp = ip;
        lastPort = port;
	}

	void mockDestructor()
	{
		TS_ASSERT( wasCreated || wasCreatedIpPort );
		TS_ASSERT( !wasDiscarded );
		wasDiscarded = true;
	}

    int descriptor()
    {
        return lastDescriptor;
    }

    HeapData lastSentData;

	void send( const ConstSimpleData &data )
	{
        TestLog::log( "Socket send called" );
        lastSentData = data;
	}

    Vector<HeapData> nextReceivedData;
    bool receiveWasCalled;
    bool socketShouldThrow;
    bool socketShouldClose;

    unsigned receiveIndex;
    ReceiveCallBack *receiveCallBack;

	void receive( HeapData &data )
	{
        if ( receiveCallBack )
        {
            receiveCallBack->receiveWasCalled();
        }

        TestLog::log( "Socket receive called" );

        receiveWasCalled = true;

        if ( socketShouldClose )
        {
            throw Error( Error::SOCKET_WAS_CLOSED );
        }
        if ( socketShouldThrow )
        {
            throw Error( Error::RECV_FAILED );
        }

        TSM_ASSERT_LESS_THAN( receiveIndex, receiveIndex, nextReceivedData.size() );

        data = nextReceivedData[receiveIndex];
        ++receiveIndex;
    }

    bool allowPortReuseWasCalled;

    void allowPortReuse()
    {
        allowPortReuseWasCalled = true;
    }

    bool closeSocketWasCalled;

    void closeSocket()
    {
        closeSocketWasCalled = true;
    }
};

#endif // __MockSocket_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

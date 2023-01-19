#ifndef __BThread_h__
#define __BThread_h__

#include "AutoSocket.h"
#include "EngineConfiguration.h"
#include "IBThread.h"
#include "SyncRequest.h"
#include "Vector.h"
#include <arpa/inet.h>

class BThread : public IBThread
{
public:
    BThread() : _socket( inet_addr( EngineConfiguration::LOOPBACK_IP ), EngineConfiguration::BTHREAD_MANAGEMENT_PORT )
    {
        static const StringTag DEFAULT_NAME = "BThread";
        _socket->send( ConstSimpleData( DEFAULT_NAME.ascii(), DEFAULT_NAME.length() ) );
    }

    BThread( const StringTag &threadName ) :
        _socket( inet_addr( EngineConfiguration::LOOPBACK_IP ), EngineConfiguration::BTHREAD_MANAGEMENT_PORT )
    {
        _socket->send(ConstSimpleData( threadName.ascii(), threadName.length()));
    }

    void bSync( const Vector<Event> &requested,
                const Vector<Event> &watched,
                const Vector<Event> &blocked,
                StringTag label = "" )
    {
        AtomicPropositions noPropositions;
        // printf("BThread -> bSync.size() with label '%s' and descriptor %u \n", label.ascii(), _socket->descriptor() );
        sendSyncRequest( requested, watched, blocked, noPropositions, label );
        waitForSyncResponse();
    }

    void bSync( const Vector<Event> &requested,
                const Vector<Event> &watched,
                const Vector<Event> &blocked,
                const AtomicPropositions &atomicPropositions,
                StringTag label = "" )
    {
        sendSyncRequest( requested, watched, blocked, atomicPropositions, label );
        waitForSyncResponse();
    }

    Event lastEvent() const
    {
        return _lastEvent;
    }

    Event getLastEvent() const
    {
        return _lastEvent;
    }

    void done()
    {
        _socket->closeSocket();
    }

    void setThreadAsSensor()
    {

    }

private:
    AutoSocket _socket;
    Event _lastEvent;

    void waitForSyncResponse()
    {
        HeapData receivedData;
        _socket->receive( receivedData );
        _lastEvent.deserialize( ConstSimpleData( receivedData.data(), receivedData.size() ) );
    }

    void sendSyncRequest( const Vector<Event> &requested,
                          const Vector<Event> &watched,
                          const Vector<Event> &blocked,
                          const AtomicPropositions &atomicPropositions,
                          StringTag &label )
    {
        SyncRequest syncRequest( requested, watched, blocked, atomicPropositions, label );

        HeapData serialized;
        syncRequest.serialize( serialized );

        _socket->send( serialized );
    }
};

#endif // __BThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

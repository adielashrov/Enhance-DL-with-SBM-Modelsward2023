#ifndef __ThreadListener_h__
#define __ThreadListener_h__

#include "AutoSocket.h"
#include "ISelector.h"
#include "ISocket.h"
#include "IThreadListener.h"
#include "IWeaver.h"
#include "SyncRequest.h"

class ThreadListener : public IThreadListener
{
public:
    ThreadListener( IWeaver &weaver, ISelector &selector, ISocket &socket ) : _weaver( weaver ),
                                                                              _selector( selector ),
                                                                              _socket( &socket )
    {
        registerToSelector();
    }

    ~ThreadListener()
    {
        unregisterFromSelector();
        discardSocket();
    }

    int descriptor()
    {
        return _socket->descriptor();
    }

    void canRead()
    {
        HeapData receivedData;

        if ( readSocketData( receivedData ) )
        {
            informWeaver( receivedData );
        }
    }

    void announceChosenEvent( Event &event )
    {
        HeapData serialized;
        event.serialize( serialized ) ;

        _socket->send( serialized );
    }

private:
    IWeaver &_weaver;
    ISelector &_selector;
    ISocket *_socket;

    void registerToSelector()
    {
        _selector.registerToSelector( *this );
    }

    void unregisterFromSelector()
    {
        _selector.unregisterFromSelector( *this );
    }

    void discardSocket()
    {
        T::discardSocket( _socket );
        _socket = 0;
    }

    bool readSocketData( HeapData &receivedData )
    {
        try
        {
            _socket->receive( receivedData );
        }
        catch ( const Error &e )
        {
            if ( e.code() == Error::SOCKET_WAS_CLOSED )
            {
                _weaver.removeThread( this );
                return false;
            }
            else
            {
                throw;
            }
        }

        return true;
    }

    void informWeaver( HeapData &receivedData )
    {
        SyncRequest syncRequest( receivedData );
        _weaver.bSync( this, syncRequest.getRequested(), syncRequest.getWatched(), syncRequest.getBlocked() );
    }
};

#endif // __ThreadListener_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

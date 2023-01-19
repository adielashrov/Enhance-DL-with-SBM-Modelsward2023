#ifndef __MockThreadListenerFactory_h__
#define __MockThreadListenerFactory_h__

#include "Map.h"
#include "MockThreadListener.h"
#include "T/ThreadListenerFactory.h"

class MockThreadListenerFactory :
	public T::Base_createThreadListener,
	public T::Base_discardThreadListener
{
public:
    Map<ISocket *, MockThreadListener *> _mockThreadListeners;

    ~MockThreadListenerFactory()
	{
        Map<ISocket *, MockThreadListener *>::iterator it;

        for ( it = _mockThreadListeners.begin(); it != _mockThreadListeners.end(); ++it )
        {
            if ( it->second->wasCreated )
            {
                TS_ASSERT( it->second->wasDiscarded );
                delete it->second;
            }
        }
	}

	IThreadListener *createThreadListener( IWeaver &weaver, ISelector &selector, ISocket &socket )
	{
        MockThreadListener *newThreadListener;

        if ( !_mockThreadListeners.exists( &socket ) )
        {
            newThreadListener = registerThreadListener( &socket );
        }
        else
        {
            newThreadListener = _mockThreadListeners[&socket];
        }

        newThreadListener->mockConstructor( weaver, selector, socket );
		return newThreadListener;
	}

    MockThreadListener *registerThreadListener( ISocket *socket )
    {
        if ( _mockThreadListeners.exists( socket ) )
        {
            TS_FAIL( "Trying to create an already existing object" );
        }

        return ( _mockThreadListeners[socket] = new MockThreadListener );
    }

	void discardThreadListener( IThreadListener *threadListener )
	{
        ((MockThreadListener *)threadListener)->mockDestructor();
    }
};

#endif // __MockThreadListenerFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

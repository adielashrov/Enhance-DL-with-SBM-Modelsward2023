#ifndef __IWeaver_h__
#define __IWeaver_h__

class Event;
class StringTag;
class IThreadListener;

#include "IServerSocket.h"
#include "Vector.h"

class IWeaver : public IServerSocket::ISocketAccepter
{
public:
    virtual void bSync( IThreadListener *thread,
                        const Vector<Event> &requested,
                        const Vector<Event> &watched,
                        const Vector<Event> &blocked ) = 0;
    virtual void removeThread( IThreadListener *thread ) = 0;
    virtual void addSensorThread( const StringTag &threadName ) = 0;

    virtual ~IWeaver() {}
};

#endif // __IWeaver_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

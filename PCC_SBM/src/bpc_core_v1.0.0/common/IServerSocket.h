#ifndef __IServerSocket_h__
#define __IServerSocket_h__

#include "ISelector.h"

class ISocket;

class IServerSocket : public ISelector::ISelectable
{
public:
	class ISocketAccepter
	{
	public:
		virtual void handleAcceptedSocket( ISocket &socket ) = 0;

		virtual ~ISocketAccepter() {}
	};

	virtual ~IServerSocket() {}
};

#endif // __IServerSocket_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

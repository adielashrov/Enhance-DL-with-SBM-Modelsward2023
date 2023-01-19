#ifndef __T__ServerSocketFactory_h__
#define __T__ServerSocketFactory_h__

#include "cxxtest/Mock.h"

#include "IServerSocket.h"


class ISelector;

namespace T
{
	IServerSocket *createServerSocket( ISelector &selector, unsigned port, IServerSocket::ISocketAccepter &accepter );
	void discardServerSocket( IServerSocket *serverSocket );
}

#ifndef CXXTEST_MOCK_REAL_SOURCE_FILE

CXXTEST_SUPPLY( createServerSocket,
				IServerSocket *,
				createServerSocket,
				( ISelector &selector, unsigned port, IServerSocket::ISocketAccepter &accepter ),
				T::createServerSocket,
				( selector, port, accepter ) );
     
CXXTEST_SUPPLY_VOID( discardServerSocket,
					 discardServerSocket,
					 ( IServerSocket *serverSocket ),
					 T::discardServerSocket,
					 ( serverSocket ) );

#endif

#endif // __T__ServerSocketFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

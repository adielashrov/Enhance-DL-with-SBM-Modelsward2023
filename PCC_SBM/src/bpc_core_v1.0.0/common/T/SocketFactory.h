#ifndef __T__SocketFactory_h__
#define __T__SocketFactory_h__

#include "cxxtest/Mock.h"

class ISocket;

namespace T
{
	ISocket *createSocket( int descriptor );
    ISocket *createSocketIpPort( unsigned ip, unsigned port );
	void discardSocket( ISocket *socket );
}

// ISocket *createSocket( int descriptor );
// void discardSocket( ISocket *socket );

CXXTEST_SUPPLY( createSocket,        /* => T::Base_AllocateIrp */
				ISocket *,           /* Return type            */
				createSocket,        /* Name of mock member    */
				( int descriptor ),  /* Prototype              */
				T::createSocket,        /* Name of real function  */
				( descriptor )       /* Parameter list         */ );

CXXTEST_SUPPLY( createSocketIpPort,        /* => T::Base_AllocateIrp */
				ISocket *,           /* Return type            */
				createSocketIpPort,        /* Name of mock member    */
				( unsigned ip, unsigned port ),  /* Prototype              */
				T::createSocketIpPort,        /* Name of real function  */
				( ip, port )       /* Parameter list         */ );

CXXTEST_SUPPLY_VOID( discardSocket,        /* => T::Base_AllocateIrp */
					 discardSocket,        /* Name of mock member    */
					 ( ISocket *socket ),  /* Prototype              */
					 T::discardSocket,        /* Name of real function  */
					 ( socket )            /* Parameter list         */ );

#endif // __T__SocketFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

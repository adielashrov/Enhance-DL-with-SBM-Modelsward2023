#include "ServerSocket.h"

namespace T
{
	IServerSocket *createServerSocket( ISelector &selector, unsigned port, IServerSocket::ISocketAccepter &accepter )
	{
		return new ServerSocket( selector, port, accepter );
	}

	void discardServerSocket( IServerSocket *serverSocket )
	{
		delete serverSocket;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

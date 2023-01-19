// #define CXXTEST_MOCK_REAL_SOURCE_FILE
// #include "T/SocketFactory.h"

#include "Socket.h"

namespace T
{
	ISocket *createSocket( int descriptor )
	{
		return new Socket( descriptor );
	}

    ISocket *createSocketIpPort( unsigned ip, unsigned port )
    {
        return new Socket( ip, port );
    }

	void discardSocket( ISocket *socket )
	{
		delete socket;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

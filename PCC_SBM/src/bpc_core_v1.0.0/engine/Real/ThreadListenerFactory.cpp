#include "ThreadListener.h"

namespace T
{
	IThreadListener *createThreadListener( IWeaver &weaver, ISelector &selector, ISocket &socket )
	{
		return new ThreadListener( weaver, selector, socket );
	}

	void discardThreadListener( IThreadListener *threadListener )
	{
		delete threadListener;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

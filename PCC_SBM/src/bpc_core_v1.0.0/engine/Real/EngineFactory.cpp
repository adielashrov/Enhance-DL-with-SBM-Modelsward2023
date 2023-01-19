#include "Engine.h"

namespace T
{
	IEngine *createEngine()
	{
		return new Engine();
	}

	void discardEngine( IEngine *engine )
	{
		delete engine;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

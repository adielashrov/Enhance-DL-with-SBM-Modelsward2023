#include "Weaver.h"

namespace T
{
	IWeaver *createWeaver( ISelector &selector )
	{
		return new Weaver( selector );
	}

	void discardWeaver( IWeaver *weaver )
	{
		delete weaver;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

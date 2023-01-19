#include "Selector.h"

namespace T
{
	ISelector *createSelector()
	{
		return new Selector();
	}

	void discardSelector( ISelector *selector )
	{
		delete selector;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

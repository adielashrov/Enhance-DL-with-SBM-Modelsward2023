#include "ProcessRunner.h"

namespace T
{
	IProcessRunner *createProcessRunner( IProcessRunner::IRunable &child )
	{
		return new ProcessRunner( child );
	}

	void discardProcessRunner( IProcessRunner *processRunner )
	{
		delete processRunner;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __MockTruth_h__
#define __MockTruth_h__

#include "T/Truth.h"

class MockTruth : 
	public T::Base_alwaysTrue
{
public:
	MockTruth()
	{
		numberOfTruths = 0;
		index = 0;
	}

	unsigned numberOfTruths;
	unsigned index;

	bool alwaysTrue()
	{
		++index;
		return index <= numberOfTruths;
	}
};

#endif // __MockTruth_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

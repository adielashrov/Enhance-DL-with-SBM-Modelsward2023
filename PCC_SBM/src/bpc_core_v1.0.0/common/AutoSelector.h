#ifndef __AutoSelector_h__
#define __AutoSelector_h__

#include "ISelector.h"
#include "T/SelectorFactory.h"

class AutoSelector
{
public:
	AutoSelector() 
	{
		_selector = T::createSelector();
	}

	~AutoSelector()
	{
		T::discardSelector( _selector );
		_selector = 0;
	}
	
	operator ISelector &()
	{
		return *_selector;
	}

	ISelector *operator->()
	{
		return _selector;
	}
	
private:
	ISelector *_selector;
};

#endif // __AutoSelector_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

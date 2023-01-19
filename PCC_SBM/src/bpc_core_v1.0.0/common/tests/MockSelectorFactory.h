#ifndef __MockSelectorFactory_h__
#define __MockSelectorFactory_h__

#include "MockSelector.h"
#include "T/SelectorFactory.h"

class MockSelectorFactory : 
	public T::Base_createSelector,
	public T::Base_discardSelector
{
public:
	MockSelector mockSelector;

	~MockSelectorFactory()
	{
		if ( mockSelector.wasCreated )
		{
			TS_ASSERT( mockSelector.wasDiscarded );
		}
	}
	
	ISelector *createSelector()
	{
		mockSelector.mockConstructor();
		return &mockSelector;
	}
	
	void discardSelector( ISelector *selector )
	{
		TS_ASSERT_EQUALS( selector, &mockSelector );
		mockSelector.mockDestructor();
	}
};

#endif // __MockSelectorFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

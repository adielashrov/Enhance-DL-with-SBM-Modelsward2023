#ifndef __MockSelector_h__
#define __MockSelector_h__

#include "ISelector.h"

class MockSelector : public ISelector
{
public:
	MockSelector()
	{
		selectWasCalled = false;
		registerWasCalled = false;
		unregisterWasCalled = false;

		wasCreated = false;
		wasDiscarded = false;

		numberOfSelectCalls = 0;
	}

	bool wasCreated;
	bool wasDiscarded;

	void mockConstructor()
	{
		TS_ASSERT( !wasCreated );
		wasCreated = true;
	}

	void mockDestructor()
	{
		TS_ASSERT( wasCreated );
		TS_ASSERT( !wasDiscarded );
		wasDiscarded = true;
	}

	ISelector::ISelectable *lastRegisteredSelectable;
	bool registerWasCalled;
	int lastRegistetedDescriptor;

	void registerToSelector( ISelector::ISelectable &selectable )
	{
		TS_ASSERT( !registerWasCalled );
		lastRegisteredSelectable = &selectable;
		lastRegistetedDescriptor = selectable.descriptor();
		registerWasCalled = true;
	}

	ISelector::ISelectable *lastUnregisteredSelectable;
	bool unregisterWasCalled;
	int lastUnregistetedDescriptor;

	void unregisterFromSelector( ISelector::ISelectable &selectable )
	{
		TS_ASSERT( !unregisterWasCalled );
		lastUnregisteredSelectable = &selectable;
		lastUnregistetedDescriptor = selectable.descriptor();
		unregisterWasCalled = true;
	}

	bool selectWasCalled;
	unsigned numberOfSelectCalls;

	void select()
	{
		selectWasCalled = true;
		++numberOfSelectCalls;
	}
};

#endif // __MockSelector_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

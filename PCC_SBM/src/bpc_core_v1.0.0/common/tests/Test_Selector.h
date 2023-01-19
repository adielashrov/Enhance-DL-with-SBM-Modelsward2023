#include <cxxtest/TestSuite.h>

#include "MockErrno.h"
#include "Selector.h"
#include "T/sys/select.h"
#include <string.h>

class MockForSelector :
    public MockErrno,
	public T::Base_select
{
public:
	MockForSelector()
	{
		lastNfds = -1;
		memset( &lastReadDescritpors, 0, sizeof(fd_set) );
		selectShouldFail = false;
	}

	int lastNfds;
	fd_set lastReadDescritpors;
	fd_set nextSelectDescriptors;
	bool selectShouldFail;

	int select( int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout )
	{

		const timeval *WAIT_FOREVER = NULL;
		const fd_set *NO_DESCRIPTORS = NULL;

		lastNfds = nfds;

		TS_ASSERT_EQUALS( writefds, NO_DESCRIPTORS );
		TS_ASSERT_EQUALS( exceptfds, NO_DESCRIPTORS );
		// TS_ASSERT_EQUALS( timeout, WAIT_FOREVER );

		memcpy( &lastReadDescritpors, readfds, sizeof(fd_set) );
		memcpy( readfds, &nextSelectDescriptors, sizeof(fd_set) );

		if ( selectShouldFail )
		{
			return -1;
		}

		return 0;
	}
};

class SelectableForTests : public ISelector::ISelectable
{
public:
	SelectableForTests()
	{
		objectDescriptor = 166;
		canReadWasCalled = false;
	}

	int objectDescriptor;

	int descriptor()
	{
		return objectDescriptor;
	}

	bool canReadWasCalled;
	void canRead()
	{
		canReadWasCalled = true;
	}
};

class SelectorTestSuite : public CxxTest::TestSuite
{
public:
	MockForSelector *mock;
	SelectableForTests *selectable1;
	SelectableForTests *selectable2;

	void setUp()
	{
		TS_ASSERT( mock = new MockForSelector );

		TS_ASSERT( selectable1 = new SelectableForTests );
		TS_ASSERT( selectable2 = new SelectableForTests );

		selectable1->objectDescriptor = 10;
		selectable2->objectDescriptor = 22;
	}

	void tearDown()
	{
		TS_ASSERT_THROWS_NOTHING( delete selectable2 );
		TS_ASSERT_THROWS_NOTHING( delete selectable1 );

		TS_ASSERT_THROWS_NOTHING( delete mock );
	}

	void zeroEverything()
	{
		FD_ZERO( &mock->nextSelectDescriptors );
		selectable1->canReadWasCalled = false;
		selectable2->canReadWasCalled = false;
	}

	void addNextSelectDescriptor( int descriptor )
	{
		FD_SET( descriptor, &mock->nextSelectDescriptors );
	}

	void assertSelectedOnNone()
	{
		fd_set expectedSet;
		FD_ZERO( &expectedSet );
		TS_ASSERT_SAME_DATA( &mock->lastReadDescritpors, &expectedSet, sizeof(fd_set) );
		TS_ASSERT_EQUALS( mock->lastNfds, 0 );
	}

	void assertSelectedOnFirst()
	{
		fd_set expectedSet;
		FD_ZERO( &expectedSet );
		FD_SET( selectable1->objectDescriptor, &expectedSet );
		TS_ASSERT_SAME_DATA( &mock->lastReadDescritpors, &expectedSet, sizeof(fd_set) );
		TS_ASSERT_EQUALS( mock->lastNfds, selectable1->objectDescriptor + 1 );
	}

	void assertSelectedOnSecond()
	{
		fd_set expectedSet;
		FD_ZERO( &expectedSet );
		FD_SET( selectable2->objectDescriptor, &expectedSet );
		TS_ASSERT_SAME_DATA( &mock->lastReadDescritpors, &expectedSet, sizeof(fd_set) );
		TS_ASSERT_EQUALS( mock->lastNfds, selectable2->objectDescriptor + 1 );
	}

	void assertSelectedOnBoth()
	{
		fd_set expectedSet;
		FD_ZERO( &expectedSet );
		FD_SET( selectable1->objectDescriptor, &expectedSet );
		FD_SET( selectable2->objectDescriptor, &expectedSet );
		TS_ASSERT_SAME_DATA( &mock->lastReadDescritpors, &expectedSet, sizeof(fd_set) );
		TS_ASSERT_EQUALS( mock->lastNfds, selectable2->objectDescriptor + 1 );
	}

	void test_select()
	{
		Selector selector;

		TS_ASSERT_THROWS_NOTHING( selector.registerToSelector( *selectable1 ) );

		zeroEverything();

		TS_ASSERT_THROWS_NOTHING( selector.select() );
		assertSelectedOnFirst();
		TS_ASSERT( !selectable1->canReadWasCalled );

		zeroEverything();
		addNextSelectDescriptor( selectable1->objectDescriptor );

		TS_ASSERT_THROWS_NOTHING( selector.select() );
		assertSelectedOnFirst();
		TS_ASSERT( selectable1->canReadWasCalled );
		TS_ASSERT( !selectable2->canReadWasCalled );

		zeroEverything();

		TS_ASSERT_THROWS_NOTHING( selector.registerToSelector( *selectable2 ) );
		addNextSelectDescriptor( selectable2->objectDescriptor );

		TS_ASSERT_THROWS_NOTHING( selector.select() );
		assertSelectedOnBoth();

		TS_ASSERT( !selectable1->canReadWasCalled );
		TS_ASSERT( selectable2->canReadWasCalled );

		zeroEverything();

		addNextSelectDescriptor( selectable1->objectDescriptor );
		addNextSelectDescriptor( selectable2->objectDescriptor );

		TS_ASSERT_THROWS_NOTHING( selector.select() );
		assertSelectedOnBoth();

		TS_ASSERT( selectable1->canReadWasCalled );
		TS_ASSERT( selectable2->canReadWasCalled );

		zeroEverything();

		TS_ASSERT_THROWS_NOTHING( selector.unregisterFromSelector( *selectable1 ) );
		addNextSelectDescriptor( selectable2->objectDescriptor );

		TS_ASSERT_THROWS_NOTHING( selector.select() );
		assertSelectedOnSecond();

		TS_ASSERT( !selectable1->canReadWasCalled );
		TS_ASSERT( selectable2->canReadWasCalled );

		zeroEverything();

		TS_ASSERT_THROWS_NOTHING( selector.unregisterFromSelector( *selectable2 ) );
		TS_ASSERT_THROWS_NOTHING( selector.registerToSelector( *selectable1 ) );
		addNextSelectDescriptor( selectable1->objectDescriptor );

		TS_ASSERT_THROWS_NOTHING( selector.select() );
		assertSelectedOnFirst();

		TS_ASSERT( selectable1->canReadWasCalled );
		TS_ASSERT( !selectable2->canReadWasCalled );
	}

	void test_select_fails()
	{
		mock->selectShouldFail = true;

		Selector selector;

		TS_ASSERT_THROWS_NOTHING( selector.registerToSelector( *selectable1 ) );

		zeroEverything();

		TS_ASSERT_THROWS_EQUALS( selector.select(),
								 const Error &e,
								 e.code(),
								 Error::SELECT_FAILED );
	}

	void test_one_selectable_removes_another()
	{
		class SpecialSelectable : public ISelector::ISelectable
		{
		public:
			Selector *_selector;
			ISelector::ISelectable *_toRemove;

			SpecialSelectable( Selector *selector, ISelector::ISelectable *toRemove ) : _selector( selector ),
																						_toRemove( toRemove )
			{
				objectDescriptor = 13;
				canReadWasCalled = false;
				firstRound = true;
			}

			int objectDescriptor;

			int descriptor()
			{
				return objectDescriptor;
			}

			bool canReadWasCalled;
			bool firstRound;

			void canRead()
			{
				canReadWasCalled = true;

				if ( firstRound )
				{
					firstRound = false;
					_selector->unregisterFromSelector( *_toRemove );
				}
			}
		};

		Selector selector;
		SpecialSelectable *selectable3 = new SpecialSelectable( &selector, selectable2 );

		TS_ASSERT_THROWS_NOTHING( selector.registerToSelector( *selectable3 ) );
		TS_ASSERT_THROWS_NOTHING( selector.registerToSelector( *selectable2 ) );

		zeroEverything();
		addNextSelectDescriptor( selectable3->objectDescriptor );
		addNextSelectDescriptor( selectable2->objectDescriptor );

		TS_ASSERT_THROWS_NOTHING( selector.select() );
		TS_ASSERT( selectable3->canReadWasCalled );
		TS_ASSERT( selectable2->canReadWasCalled );

		zeroEverything();
		selectable3->canReadWasCalled = false;

		addNextSelectDescriptor( selectable3->objectDescriptor );

		TS_ASSERT_THROWS_NOTHING( selector.select() );
		TS_ASSERT( selectable3->canReadWasCalled );
	}
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

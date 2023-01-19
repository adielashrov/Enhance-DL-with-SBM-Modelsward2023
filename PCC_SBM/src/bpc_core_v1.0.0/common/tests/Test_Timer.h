#include <cxxtest/TestSuite.h>

#include "MockErrno.h"
#include "MockSelector.h"
#include "Timer.h"

class ExpiredForTests : public ITimer::IExpired
{
public:
    ExpiredForTests()
    {
        expiredWasCalled = false;
    }

    bool expiredWasCalled;

    void expired()
    {
        expiredWasCalled = true;
    }
};

class MockForTimer :
    public MockErrno,
    public T::Base_timerfd_create,
    public T::Base_close,
    public T::Base_timerfd_settime,
    public T::Base_read
{
public:

    MockForTimer()
    {
        nextDescriptor = 343;
        timerWasCreated = false;
        timerWasClosed = false;
        readWasCalled = false;
        createShouldFail = false;
        settimeShouldFail = false;
    }

    int nextDescriptor;
    bool timerWasCreated;
    bool createShouldFail;

    int timerfd_create( int clockid, int flags )
    {
        if ( createShouldFail )
        {
            return -1;
        }

        TS_ASSERT( !timerWasCreated );
        timerWasCreated = true;

        TS_ASSERT_EQUALS( clockid, CLOCK_REALTIME );
        TS_ASSERT_EQUALS( flags, 0 );

        return nextDescriptor;
    }

    bool timerWasClosed;

    int close( int fd )
    {
        TS_ASSERT( timerWasCreated );
        TS_ASSERT( !timerWasClosed );

        timerWasClosed = true;

        TS_ASSERT_EQUALS( fd, nextDescriptor );
        return 0;
    }

    time_t lastSetSeconds;
    bool settimeShouldFail;

    int timerfd_settime( int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value )
    {
        if ( settimeShouldFail )
        {
            return -1;
        }

        TS_ASSERT( timerWasCreated );
        TS_ASSERT( !timerWasClosed );

        TS_ASSERT_EQUALS( fd, nextDescriptor );

        static const int RELATIVE_TIMER = 0;
        TS_ASSERT_EQUALS( flags, RELATIVE_TIMER );

        TS_ASSERT_EQUALS( new_value->it_interval.tv_sec, 0 );
        TS_ASSERT_EQUALS( new_value->it_interval.tv_nsec, 0 );

        lastSetSeconds = new_value->it_value.tv_sec;
        TS_ASSERT_EQUALS( new_value->it_value.tv_nsec, 0 );

        memset( old_value, 0, sizeof(old_value) );

        return 0;
    }

    bool readWasCalled;

    ssize_t read( int fd, void *buf, size_t count )
    {
        TS_ASSERT( timerWasCreated );
        TS_ASSERT( !timerWasClosed );

        TS_ASSERT_EQUALS( fd, nextDescriptor );

        TS_ASSERT_EQUALS( count, sizeof(unsigned long long) );

        readWasCalled = true;

        *(unsigned long long *)buf = 1ull;

        return sizeof(unsigned long long);
    }
};

class TiemrTestSuite : public CxxTest::TestSuite
{
public:
	MockForTimer *mock;
    MockSelector *selector;
    ExpiredForTests *expired;

	void setUp()
	{
		TS_ASSERT( mock = new MockForTimer );
        TS_ASSERT( selector = new MockSelector );
        TS_ASSERT( expired = new ExpiredForTests );
	}

	void tearDown()
	{
        TS_ASSERT_THROWS_NOTHING( delete expired );
        TS_ASSERT_THROWS_NOTHING( delete selector );
		TS_ASSERT_THROWS_NOTHING( delete mock );
    }

    void test_constructor_destructor()
    {
        Timer *timer;

        TS_ASSERT( timer = new Timer( *selector, *expired ) );

        TS_ASSERT( mock->timerWasCreated );
        TS_ASSERT( selector->registerWasCalled );
        TS_ASSERT_EQUALS( selector->lastRegisteredSelectable, timer );
        TS_ASSERT_EQUALS( selector->lastRegistetedDescriptor, mock->nextDescriptor );

        TS_ASSERT_THROWS_NOTHING( delete timer );

        TS_ASSERT( selector->unregisterWasCalled );
        TS_ASSERT_EQUALS( selector->lastUnregisteredSelectable, timer );
        TS_ASSERT_EQUALS( selector->lastUnregistetedDescriptor, mock->nextDescriptor );

        TS_ASSERT( mock->timerWasClosed );
    }

    void test_constructor_destructor__create_fails()
    {
        mock->createShouldFail = true;

        Timer *timer;

        TS_ASSERT_THROWS_EQUALS( timer = new Timer( *selector, *expired ),
                                 const Error &e,
                                 e.code(),
                                 Error::TIMERFD_CREATE_FAILED );

        TS_ASSERT( !selector->registerWasCalled );
        TS_ASSERT( !mock->timerWasClosed );
    }

    void test_set()
    {
        Timer timer( *selector, *expired );

        TS_ASSERT_THROWS_NOTHING( timer.set( 5U ) );
        TS_ASSERT_EQUALS( mock->lastSetSeconds, 5 );

        TS_ASSERT_THROWS_NOTHING( timer.set( 122U ) );
        TS_ASSERT_EQUALS( mock->lastSetSeconds, 122 );
    }

    void test_set__settime_fails()
    {
        Timer timer( *selector, *expired );

        mock->settimeShouldFail = true;

        TS_ASSERT_THROWS_EQUALS( timer.set( 5U ),
                                 const Error &e,
                                 e.code(),
                                 Error::TIMERFD_SETTIME_FAILED );
    }

    void test_stop()
    {
        Timer timer( *selector, *expired );

        TS_ASSERT_THROWS_NOTHING( timer.stop() );
        TS_ASSERT_EQUALS( mock->lastSetSeconds, 0 );
    }

    void test_stop__settime_fails()
    {
        Timer timer( *selector, *expired );

        mock->settimeShouldFail = true;

        TS_ASSERT_THROWS_EQUALS( timer.stop(),
                                 const Error &e,
                                 e.code(),
                                 Error::TIMERFD_SETTIME_FAILED );
    }

    void test_canRead()
    {
        Timer timer( *selector, *expired );

        TS_ASSERT_THROWS_NOTHING( timer.canRead() );

        TS_ASSERT( mock->readWasCalled );
        TS_ASSERT( expired->expiredWasCalled );
    }
};

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

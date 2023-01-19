#ifndef __Timer_h__
#define __Timer_h__

#include "Error.h"
#include "ISelector.h"
#include "ITimer.h"
#include "T/sys/timerfd.h"
#include "T/unistd.h"
#include <cstring>

class Timer : public ITimer, public ISelector::ISelectable
{
public:
    Timer( ISelector &selector, ITimer::IExpired &expired ) : _selector( selector ), _expired( expired )
    {
        createTimer();
        registerToSelector();
    }

    void set( unsigned seconds )
    {
        static const int RELATIVE_TIMER = 0;

        struct itimerspec timerData;
        memset( &timerData, 0, sizeof(timerData) );
        timerData.it_value.tv_sec = seconds;

        struct itimerspec dontCareOldTime;

        if ( T::timerfd_settime( _descriptor, RELATIVE_TIMER, &timerData, &dontCareOldTime ) != 0 )
        {
            throw Error( Error::TIMERFD_SETTIME_FAILED );
        }
    }

    void stop()
    {
        enum {
            TIMER_RESET_VALUE = 0,
        };

        set( TIMER_RESET_VALUE );
    }

    int descriptor()
    {
        return _descriptor;
    }

    void canRead()
    {
        unsigned long long dontCare;
        T::read( _descriptor, &dontCare, sizeof(dontCare) );
        _expired.expired();
    }

    ~Timer()
    {
        unregisterFromSelector();
        closeTimer();
    }

private:
    ISelector &_selector;
    IExpired &_expired;
    int _descriptor;

    void createTimer()
    {
        enum {
            NO_FLAGS = 0,
        };

        if ( ( _descriptor = T::timerfd_create( CLOCK_REALTIME, NO_FLAGS ) ) == -1 )
        {
            throw Error( Error::TIMERFD_CREATE_FAILED );
        }
    }

    void registerToSelector()
    {
        _selector.registerToSelector( *this );
    }

    void unregisterFromSelector()
    {
        _selector.unregisterFromSelector( *this );
    }

    void closeTimer()
    {
        T::close( _descriptor );
    }
};

#endif // __Timer_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

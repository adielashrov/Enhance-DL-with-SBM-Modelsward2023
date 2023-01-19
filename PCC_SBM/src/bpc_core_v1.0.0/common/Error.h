#ifndef __Error_h__
#define __Error_h__

#include "T/Errno.h"
#include "T/stdlib.h"

class Error
{
public:
	enum Code {
		NOT_ENOUGH_MEMORY = 0,
		KEY_DOESNT_EXIST_IN_MAP = 1,
		SELECT_FAILED = 2,
		SOCKET_CREATION_FAILED = 3,
		BIND_FAILED = 4,
		LISTEN_FAILED = 5,
		ACCEPT_FAILED = 6,
		CONNECT_FAILED = 7,
		SEND_FAILED = 8,
		RECV_FAILED = 9,
		SOCKET_WAS_CLOSED = 10,
        VALUE_DOESNT_EXIST_IN_VECTOR = 11,
        SELECTION_DEADLOCKED = 12,
        FORK_FAILED = 13,
        PTHREAD_CREATE_FAILED = 14,
        SET_SOCK_OPT_FAILED = 15,
        OPEN_FAILED = 16,
        WRITE_FAILED = 17,
        VECTOR_OUT_OF_BOUND = 18,
        READ_FAILED = 19,
        UNKNOWN_GOAL_RESULT = 20,
        RENAME_FAILED = 21,
        UNLINK_FAILED = 22,
        COULDNT_FIND_NEW_STATE = 23,
        COULDNT_FIND_EXISTING_TRANSITION = 24,
        POPPING_FROM_EMPTY_VECTOR = 25,
        VALUE_DOESNT_EXIST_IN_CONFIG_FILE = 26,
        QUEUE_IS_EMPTY = 27,
        TIMERFD_CREATE_FAILED = 28,
        TIMERFD_SETTIME_FAILED = 29,
	};

	Error( Code code ) : _code( code )
	{
        _errno = T::errorNumber();
	}

    int errnoValue() const
    {
        return _errno;
    }

	Code code() const
	{
		return _code;
	}

private:
	Code _code;
    int _errno;
};

#endif // __Error_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

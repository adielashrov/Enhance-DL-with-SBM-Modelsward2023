#ifndef __Queue_h__
#define __Queue_h__

#include <cstdio>

#include "Error.h"
#include <queue>

template<class T>
class Queue
{
    typedef std::queue<T> Super;
public:

    virtual void push( T value )
    {
        _container.push( value );
    }

    bool empty() const
    {
        return _container.empty();
    }

    void clear()
    {
        while ( !empty() )
        {
            pop();
        }
    }

    void pop()
    {
        if ( empty() )
        {
            throw Error( Error::QUEUE_IS_EMPTY );
        }

        _container.pop();
    }

    T &peak()
    {
        if ( empty() )
        {
            throw Error( Error::QUEUE_IS_EMPTY );
        }

        return _container.front();
    }

protected:
    Super _container;
};

#endif // __Queue_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

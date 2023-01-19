#ifndef __UniqueVector_h__
#define __UniqueVector_h__

#include "Vector.h"
#include <cassert>

template<class T>
class UniqueVector : public Vector<T>
{
    typedef Vector<T> Super;
public:
    void append( T value )
    {
        if ( !Super::exists( value ) )
        {
            Super::append( value );
        }
    }

    void insertHead( T )
    {
        assert( 0 );
    }
};

#endif // __UniqueVector_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

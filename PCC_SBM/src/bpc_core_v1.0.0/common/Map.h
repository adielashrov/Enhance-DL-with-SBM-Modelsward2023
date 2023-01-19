#ifndef __Map_h__
#define __Map_h__

#include "Error.h"
#include <map>

template<class Key, class Value>
class Map
{
    typedef std::map<Key,Value> Super;
public:
    typedef typename Super::iterator iterator;
    typedef typename Super::const_iterator const_iterator;

    Value &operator[]( Key key )
    {
        return _container[key];
    }

    const Value &operator[]( Key key ) const
    {
        return _container[key];
    }

    bool empty() const
    {
        return _container.empty();
    }

    unsigned size() const
    {
        return _container.size();
    }

    bool exists( Key key )
    {
        return _container.find( key ) != _container.end();
    }

    void erase( Key key )
    {
        if ( !exists( key ) )
        {
            throw Error( Error::KEY_DOESNT_EXIST_IN_MAP );
        }

        _container.erase( key );
    }

    iterator begin()
    {
        return _container.begin();
    }

    iterator end()
    {
        return _container.end();
    }

    void clear()
    {
        _container.clear();
    }

private:
    Super _container;
};

#endif // __Map_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

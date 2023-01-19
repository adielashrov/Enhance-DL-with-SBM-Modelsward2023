#ifndef __StateCycle_h__
#define __StateCycle_h__

struct StateCycle
{
public:
    Vector<StringTag> _states;

    void append( StringTag state )
    {
        _states.append( state );
    }

    unsigned size()
    {
        return _states.size();
    }

    StringTag start()
    {
        if ( _states.size() == 0 )
        {
            return "";
        }

        return _states[0];
    }

    StringTag operator[]( unsigned index )
    {
        return _states[index];
    }
};

#endif // __StateCycle_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

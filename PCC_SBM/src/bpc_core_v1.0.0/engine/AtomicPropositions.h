#ifndef __AtomicPropositions_h__
#define __AtomicPropositions_h__

#include "StringTag.h"
#include "UniqueVector.h"

class AtomicPropositions
{
public:
    void add( const StringTag &proposition )
    {
        _propositions.append( proposition );
    }

    StringTag at( unsigned index )
    {
        return _propositions[index];
    }

    Vector<StringTag> get() const
    {
        return _propositions;
    }

    unsigned size() const
    {
        return _propositions.size();
    }

private:
    UniqueVector<StringTag> _propositions;
};

#endif // __AtomicPropositions_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

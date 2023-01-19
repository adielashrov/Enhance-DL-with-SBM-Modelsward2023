#ifndef __IThreadListener_h__
#define __IThreadListener_h__

#include "Event.h"
#include "ISelector.h"

class IThreadListener : public ISelector::ISelectable
{
public:
    virtual void announceChosenEvent( Event &event ) = 0;
};

#endif // __IThreadListener_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __T__SelectorFactory_h__
#define __T__SelectorFactory_h__

#include "cxxtest/Mock.h"

class ISelector;

namespace T
{
	ISelector *createSelector();
	void discardSelector( ISelector *selector );
}

// ISelector *createSelector( int descriptor );
// void discardSelector( ISelector *selector );

CXXTEST_SUPPLY( createSelector,        /* => T::Base_AllocateIrp */
				ISelector *,           /* Return type            */
				createSelector,        /* Name of mock member    */
				(),  /* Prototype              */
				T::createSelector,        /* Name of real function  */
				()       /* Parameter list         */ );
     
CXXTEST_SUPPLY_VOID( discardSelector,        /* => T::Base_AllocateIrp */
					 discardSelector,        /* Name of mock member    */
					 ( ISelector *selector ),  /* Prototype              */
					 T::discardSelector,        /* Name of real function  */
					 ( selector )            /* Parameter list         */ );

#endif // __T__SelectorFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

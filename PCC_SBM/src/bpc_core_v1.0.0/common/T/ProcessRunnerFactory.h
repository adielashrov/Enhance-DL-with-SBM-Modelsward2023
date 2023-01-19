#ifndef __T__ProcessRunnerFactory_h__
#define __T__ProcessRunnerFactory_h__

#include "cxxtest/Mock.h"

#include "IProcessRunner.h"

namespace T
{
	IProcessRunner *createProcessRunner( IProcessRunner::IRunable &child );
	void discardProcessRunner( IProcessRunner *processRunner );
}

// IProcessRunner *createProcessRunner( int descriptor );
// void discardProcessRunner( IProcessRunner *processRunner );

CXXTEST_SUPPLY( createProcessRunner,        /* => T::Base_AllocateIrp */
				IProcessRunner *,           /* Return type            */
				createProcessRunner,        /* Name of mock member    */
				( IProcessRunner::IRunable &child ),  /* Prototype              */
				T::createProcessRunner,        /* Name of real function  */
				( child )       /* Parameter list         */ );

CXXTEST_SUPPLY_VOID( discardProcessRunner,        /* => T::Base_AllocateIrp */
					 discardProcessRunner,        /* Name of mock member    */
					 ( IProcessRunner *processRunner ),  /* Prototype              */
					 T::discardProcessRunner,        /* Name of real function  */
					 ( processRunner )            /* Parameter list         */ );

#endif // __T__ProcessRunnerFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

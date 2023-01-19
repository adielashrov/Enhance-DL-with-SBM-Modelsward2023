#ifndef __MockProcessRunnerFactory_h__
#define __MockProcessRunnerFactory_h__

#include "Map.h"
#include "MockProcessRunner.h"
#include "T/ProcessRunnerFactory.h"

class MockProcessRunnerFactory :
	public T::Base_createProcessRunner,
	public T::Base_discardProcessRunner
{
public:
    Map<IProcessRunner::IRunable *, MockProcessRunner *> _mockProcessRunners;

    ~MockProcessRunnerFactory()
	{
        Map<IProcessRunner::IRunable *, MockProcessRunner *>::iterator it;

        for ( it = _mockProcessRunners.begin(); it != _mockProcessRunners.end(); ++it )
        {
            if ( it->second->wasCreated )
            {
                TS_ASSERT( it->second->wasDiscarded );
                delete it->second;
            }
        }
	}

	IProcessRunner *createProcessRunner( IProcessRunner::IRunable &child )
	{
        MockProcessRunner *newProcessRunner;

        if ( !_mockProcessRunners.exists( &child ) )
        {
            newProcessRunner = registerProcessRunner( &child );
        }
        else
        {
            newProcessRunner = _mockProcessRunners[&child];
        }

        newProcessRunner->mockConstructor( child );
		return newProcessRunner;
	}

    MockProcessRunner *registerProcessRunner( IProcessRunner::IRunable *child )
    {
        if ( _mockProcessRunners.exists( child ) )
        {
            TS_FAIL( "Trying to create an already existing process runner" );
        }

        return ( _mockProcessRunners[child] = new MockProcessRunner );
    }

	void discardProcessRunner( IProcessRunner *processRunner )
	{
        ((MockProcessRunner *)processRunner)->mockDestructor();
    }
};

#endif // __MockProcessRunnerFactory_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

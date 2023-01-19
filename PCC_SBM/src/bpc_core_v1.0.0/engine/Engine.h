#ifndef __Engine_h__
#define __Engine_h__

#include "AutoSelector.h"
#include "AutoServerSocket.h"
#include "AutoWeaver.h"
#include "EngineConfiguration.h"
#include "Error.h"
#include "IEngine.h"
#include "T/Truth.h"

class Engine : public IEngine
{
public:
	static void main()
	{
		// printf("Enter Engine::main... \n");
		Engine().entryPoint();
	}

	Engine() : _weaver( _selector ), _serverSocket( _selector, EngineConfiguration::BTHREAD_MANAGEMENT_PORT, _weaver )
	{
	}

	void addSensorThread( const StringTag &threadName )
    {
        _weaver->addSensorThread( threadName );
    }

	void entryPoint()
	{
		// printf("Enter Engine::entryPoint... \n");
		while ( T::alwaysTrue() )
		{
			_selector->select();
		}
	}

private:
	AutoSelector _selector;
	AutoWeaver _weaver;
	AutoServerSocket _serverSocket;
};

#endif // __Engine_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

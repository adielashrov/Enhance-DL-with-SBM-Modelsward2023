#include "BProgram.h"
#include "Interleaver.h"
#include "SayGoodEvening.h"
#include "SayGoodMorning.h"
#include <stdlib.h>
#include <cstdio>

int main()
{
    try
    {
        srand (0);
        BProgram bProgram;
        bProgram.runEngine();

        SayGoodMorning sayGoodMorning;
        SayGoodEvening sayGoodEvening;
        // Interleaver interleaver;

        bProgram.addThread( sayGoodMorning );
        bProgram.addSensorThread( "SayGoodMorning" ); //sayGoodMorning is a sensor thread
        bProgram.addSensorThread( "SayGoodEvening" ); //sayGoodMorning is a sensor thread

        bProgram.addThread( sayGoodEvening );
        // bProgram.addThread( interleaver );

        bProgram.runProgram();
        sleep( 100000 );
    }
    catch ( const Error &e )
    {
        printf( "Main testBasicGuardFlow: Error caught. Code: %u. Errno: %i\n", e.code(), e.errnoValue() );
        fflush( 0 );
    }

	return 0;
}

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

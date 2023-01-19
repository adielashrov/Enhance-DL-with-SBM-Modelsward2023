#ifndef __QptlFormulaToAutomaton_h__
#define __QptlFormulaToAutomaton_h__

#include "Stringf.h"

class QptlFormulaToAutomaton
{
public:
    static void writeFormula( const StringTag &formula, const StringTag &fileName )
    {
        const char *GOAL_EXECUTABLE = "/home/guy/projects/prime_multiples/core/tools/GOAL-20120409/goal ";

        Stringf flags( "translate QPTL -m tableau -o %s -se -sa -sf -sg -sp ", fileName.ascii() );
        system( ( StringTag( GOAL_EXECUTABLE ) + flags + "\" " + formula + "\" > /dev/null" ).ascii() );
    }
};

#endif // __QptlFormulaToAutomaton_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

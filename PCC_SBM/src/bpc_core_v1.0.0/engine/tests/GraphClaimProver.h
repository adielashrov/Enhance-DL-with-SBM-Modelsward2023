#ifndef __GraphClaimProver_h__
#define __GraphClaimProver_h__

#include "Map.h"
#include "ThreadState.h"

class GraphClaimProver
{
public:
    typedef Map<StringTag, ThreadState> StateGraph;
    typedef bool (* StatePredicate)( ThreadState &state );

    static bool invariantHolds( StateGraph &stateGraph, StatePredicate predicate )
    {
        StateGraph::iterator it;

        for ( it = stateGraph.begin(); it != stateGraph.end(); ++it )
        {
            if ( !( predicate( it->second ) ) )
            {
                return false;
            }
        }

        return true;
    }

    static bool predicateHoldsOnlyEveryNSyncs( StateGraph &/*stateGraph*/,
                                               StringTag /*initialState*/,
                                               StatePredicate /*predicate*/,
                                               unsigned /*n*/ )
    {
        // typedef Vector<StringTag> States;
        // Map<unsigned, States> stepsToStates;
        // StateGraph::iterator it;

        // for ( unsigned i = 1; i <= n; ++i )
        // {
        //     Vector<StringTag> newVector;
        //     stepsToStates[i] = newVector;
        // }

        // unsigned step = 1;
        // unsigned nextStep;
        // stepsToStates[1].append( initialState );
        // bool changeOccured = true;

        // while ( changeOccured )
        // {
        //     changeOccured = false;

        //     nextStep = step + 1;
        //     if ( nextStep > n )
        //     {
        //         nextStep = 1;
        //     }

        //     for ( unsigned j = 0; j < stepsToStates[step].size(); ++j )
        //     {
        //         ThreadState state = stateGraph[stepsToStates[step][j]];

        //         for ( unsigned i = 0; i < state._successors.size(); ++i )
        //         {
        //             if ( !stepsToStates[nextStep].exists( state._successors[i] ) )
        //             {
        //                 stepsToStates[nextStep].append( state._successors[i] );
        //                 changeOccured = true;
        //             }
        //         }
        //     }

        //     step = nextStep;
        // }

        // for ( unsigned i = 1; i <= n; ++i )
        // {
        //     printf( "States reachable in step %u: ", i );
        //     for ( unsigned j = 0; j < stepsToStates[i].size(); ++j )
        //     {
        //         printf( "%s ", stateGraph[stepsToStates[i][j]]._label.ascii() );
        //     }
        //     printf( "\n" );
        // }

        // for ( unsigned i = 1; i < n; ++i )
        // {
        //     for ( unsigned j = 0; j < stepsToStates[i].size(); ++j )
        //     {
        //         ThreadState state = stateGraph[stepsToStates[i][j]];

        //         if ( predicate( state ) )
        //         {
        //             printf( "failure\n" );
        //             return false;
        //         }
        //     }
        // }

        // for ( unsigned i = 0; i < stepsToStates[n].size(); ++i )
        // {
        //     if ( !( predicate( stateGraph[stepsToStates[n][i]] ) ) )
        //     {
        //         return false;
        //     }
        // }

        return true;
    }

private:
};

#endif // __GraphClaimProver_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

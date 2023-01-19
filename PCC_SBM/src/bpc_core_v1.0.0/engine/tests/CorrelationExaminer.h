#ifndef __CorrelationExaminer_h__
#define __CorrelationExaminer_h__

class CorrelationExaminer
{
public:
    float findCorrelation( ThreadStateGraph &stateGraph, const Event &event )
    {
        countStatesInvolvingEvent( stateGraph, event );
        return (float)( _statesInvolvingEvent ) / states( stateGraph );
    }

private:
    unsigned _statesInvolvingEvent;

    void countStatesInvolvingEvent( ThreadStateGraph &stateGraph, const Event &event )
    {
        _statesInvolvingEvent = 0;

        Map<StringTag, ThreadState>::iterator it;
        for ( it = stateGraph._graph.begin(); it != stateGraph._graph.end(); ++it )
        {
            examineSingleState( it->second, event );
        }
    }

    void examineSingleState( ThreadState &state, Event event )
    {
        if ( state._requestedEvents.exists( event ) ||
             state._watchedEvents.exists( event ) ||
             state._blockedEvents.exists( event ) )
        {
            ++_statesInvolvingEvent;
        }
    }

    static float states( ThreadStateGraph &stateGraph )
    {
        return stateGraph._graph.size();
    }
};

#endif // __CorrelationExaminer_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

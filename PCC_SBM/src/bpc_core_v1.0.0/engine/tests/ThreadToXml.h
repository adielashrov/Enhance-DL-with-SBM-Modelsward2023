#ifndef __ThreadToXml_h__
#define __ThreadToXml_h__

#include "File.h"
#include "Stringf.h"
#include "ThreadStateGraph.h"

class ThreadToXml
{
public:
    void generateXml( ThreadStateGraph &stateGraph, unsigned threadIndex, const StringTag &fileName )
    {
        handleInput( stateGraph, threadIndex, fileName );

        writeHeaders();
        writeAlphabet();
        writeStates();
        writeInitialState();
        writeTransitions();
        writeAcceptingStates();
        writeFooters();

        closeOutputFile();
    }

private:
    ThreadStateGraph *_stateGraph;
    unsigned _threadIndex;
    IFile *_output;
    unsigned _transitionCounter;

    void handleInput( ThreadStateGraph &stateGraph, unsigned threadIndex, const StringTag &fileName )
    {
        _stateGraph = &stateGraph;
        _threadIndex = threadIndex;

        _output = new File( fileName );
        openOutputFile();
    }

    void openOutputFile()
    {
        _output->open( IFile::MODE_WRITE );
    }

    void closeOutputFile()
    {
        delete _output;
        _output = NULL;
    }

    void writeLine( StringTag line )
    {
        _output->write( line + "\n" );
    }

    void writeHeaders()
    {
        writeLine( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" );
        writeLine( "<Structure label-on=\"Transition\" type=\"FiniteStateAutomaton\">" );
        writeLine( "\t<Name/>" );
        writeLine( "\t<Description/>" );
        writeLine( "\t<Formula/>" );
    }

    void writeAlphabet()
    {
        writeLine( "\t<Alphabet type=\"Propositional\">" );

        for ( unsigned i = 0; i < _stateGraph->_events.size(); ++i )
        {
            unsigned eventCode = _stateGraph->_events[i].type();

            writeLine( Stringf( "\t\t<Proposition>e%u</Proposition>", eventCode ) );
            writeLine( Stringf( "\t\t<Proposition>r%ue%u</Proposition>", _threadIndex, eventCode ) );
            writeLine( Stringf( "\t\t<Proposition>b%ue%u</Proposition>", _threadIndex, eventCode ) );
        }
        for ( unsigned i = 0; i < _stateGraph->_outerEvents.size(); ++i )
        {
            unsigned eventCode = _stateGraph->_outerEvents[i].type();

            writeLine( Stringf( "\t\t<Proposition>e%u</Proposition>", eventCode ) );
        }

        writeLine( "\t</Alphabet>" );
    }

    void writeStates()
    {
        writeLine( "\t<StateSet>" );

        for ( unsigned i = 1; i <= _stateGraph->_graph.size(); ++i )
        {
            writeLine( Stringf( "\t\t<State sid=\"%u\">", i ) );
            writeLine( "\t\t\t<Properties/>" );
            writeLine( "\t\t</State>" );
        }

        writeLine( "\t</StateSet>" );
    }

    void writeInitialState()
    {
        writeLine( "\t<InitialStateSet>" );
        writeLine( Stringf( "\t\t<StateID>%u</StateID>", findIndexOfInitial() ) );
        writeLine( "\t</InitialStateSet>" );
    }

    void writeTransitions()
    {
        writeLine( "\t<TransitionSet>" );

        _transitionCounter = 1;

        Map<StringTag, ThreadState>::iterator it;

        for ( it = _stateGraph->_graph.begin(); it != _stateGraph->_graph.end(); ++it )
        {
            writeTransitionsOfState( it->second._label );
        }

        writeLine( "\t</TransitionSet>" );
    }

    void writeTransitionsOfState( StringTag label )
    {
        ThreadState &state = _stateGraph->_graph[label];

        StringTag requestedAndBlocked;

        for ( unsigned i = 0; i < _stateGraph->_events.size(); ++i )
        {
            if ( state._requestedEvents.exists( _stateGraph->_events[i] ) )
            {
                requestedAndBlocked += Stringf( "r%ue%u ", _threadIndex, _stateGraph->_events[i].type() );
            }
            else
            {
                requestedAndBlocked += Stringf( "~r%ue%u ", _threadIndex, _stateGraph->_events[i].type() );
            }

            if ( state._blockedEvents.exists( _stateGraph->_events[i] ) )
            {
                requestedAndBlocked += Stringf( "b%ue%u ", _threadIndex, _stateGraph->_events[i].type() );
            }
            else
            {
                requestedAndBlocked += Stringf( "~b%ue%u ", _threadIndex, _stateGraph->_events[i].type() );
            }
        }

        for ( unsigned i = 0; i < state._successors.size(); ++i )
        {
            Event selectedEvent = state._successors[i]._event;
            StringTag transition = requestedAndBlocked + Stringf( "e%u ", selectedEvent.type() );

            for ( unsigned j = 0; j < _stateGraph->_allEvents.size(); ++j )
            {
                if ( _stateGraph->_allEvents[j] != selectedEvent )
                {
                    transition += Stringf( "~e%u ", _stateGraph->_allEvents[j].type() );
                }
            }

            writeLine( Stringf( "\t\t<Transition tid=\"%u\">", _transitionCounter++ ) );
            writeLine( Stringf( "\t\t\t<From>%u</From>", labelToStateIndex( label ) ) );
            writeLine( Stringf( "\t\t\t<To>%u</To>", labelToStateIndex( state._successors[i]._state ) ) );
            writeLine( Stringf( "\t\t\t<Label>%s</Label>", transition.ascii() ) );
            writeLine( "\t\t\t<Properties/>" );
            writeLine( "\t\t</Transition>" );
        }
    }

    unsigned findIndexOfInitial()
    {
        return labelToStateIndex( _stateGraph->_initialState );
    }

    unsigned labelToStateIndex( const StringTag &label )
    {
        Map<StringTag, ThreadState>::iterator it = _stateGraph->_graph.begin();

        for ( unsigned i = 1; i <= _stateGraph->_graph.size(); ++i )
        {
            if ( it->second._label == label )
            {
                return i;
            }

            ++it;
        }

        return 0;
    }

    void writeAcceptingStates()
    {
        writeLine( "\t<Acc type=\"Buchi\">" );

        for ( unsigned i = 1; i <= _stateGraph->_graph.size(); ++i )
        {
            writeLine( Stringf( "\t\t<StateID>%u</StateID>", i ) );
        }

        writeLine( "\t</Acc>" );
    }

    void writeFooters()
    {
        writeLine( "\t<Properties/>" );
        writeLine( "</Structure>" );
    }
};

#endif // __ThreadToXml_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

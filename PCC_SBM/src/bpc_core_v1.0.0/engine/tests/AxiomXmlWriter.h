#ifndef __AxiomXmlWriter_h__
#define __AxiomXmlWriter_h__

#include "Event.h"
#include "File.h"
#include "Stringf.h"
#include "Vector.h"

class AxiomXmlWriter
{
public:
    void generateXml( Vector<Event> &events, const StringTag &outputFile )
    {
        openOutputFile( outputFile );
        storeEvents( events );

        writeHeaders();
        writeAlphabet();
        writeSingleState();
        writeInitialState();
        writeTransitions();
        writeAcceptingStates();
        writeFooters();

        closeOutputFile();
    }

private:
    Vector<Event> _events;
    unsigned _numberOfThreads;
    IFile *_output;
    unsigned _transitionCounter;
    StringTag _directory;

    void storeEvents( Vector<Event> &events )
    {
        _events = events;
    }

    void openOutputFile( const StringTag &path )
    {
        _output = new File( path );
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

        for ( unsigned i = 0; i < _events.size(); ++i )
        {
            unsigned currentEvent = _events[i].type();

            writeLine( Stringf( "\t\t<Proposition>e%u</Proposition>", currentEvent ) );
            writeLine( Stringf( "\t\t<Proposition>r1e%u</Proposition>", currentEvent ) );
            writeLine( Stringf( "\t\t<Proposition>b1e%u</Proposition>", currentEvent ) );
        }

        writeLine( "\t</Alphabet>" );
    }

    void writeSingleState()
    {
        writeLine( "\t<StateSet>" );
        writeLine( "\t\t<State sid=\"1\">" );
        writeLine( "\t\t\t<Properties/>" );
        writeLine( "\t\t</State>" );
        writeLine( "\t</StateSet>" );
    }

    void writeInitialState()
    {
        writeLine( "\t<InitialStateSet>" );
        writeLine( "\t\t<StateID>1</StateID>" );
        writeLine( "\t</InitialStateSet>" );
    }

    void writeTransitions()
    {
        writeLine( "\t<TransitionSet>" );

        _transitionCounter = 1;
        for ( unsigned i = 0; i < _events.size(); ++i )
        {
            writeTransitionOfEvent( i );
        }

        writeLine( "\t</TransitionSet>" );
    }

    void writeTransitionOfEvent( unsigned eventIndex )
    {
        Stringf transition( "e%u r1e%u ~b1e%u ", eventIndex, eventIndex, eventIndex );

        for ( unsigned i = 0; i < _events.size(); ++i )
        {
            if ( i != eventIndex )
            {
                transition += Stringf( "~e%u ", _events[i].type() );
            }
        }

        writeSingleTransition( transition );
    }

    void writeSingleTransition( StringTag transition )
    {
        writeLine( Stringf( "\t\t<Transition tid=\"%u\">", _transitionCounter++ ) );
        writeLine( "\t\t\t<From>1</From>" );
        writeLine( "\t\t\t<To>1</To>" );
        writeLine( Stringf( "\t\t\t<Label>%s</Label>", transition.ascii() ) );
        writeLine( "\t\t\t<Properties/>" );
        writeLine( "\t\t</Transition>" );
    }

    void writeAcceptingStates()
    {
        writeLine( "\t<Acc type=\"Buchi\">" );
        writeLine( "\t\t<StateID>1</StateID>" );
        writeLine( "\t</Acc>" );
    }

    void writeFooters()
    {
        writeLine( "\t<Properties/>" );
        writeLine( "</Structure>" );
    }
};

#endif // __AxiomXmlWriter_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

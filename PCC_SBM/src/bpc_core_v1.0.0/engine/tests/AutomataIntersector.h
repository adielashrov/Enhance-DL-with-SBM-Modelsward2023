#ifndef __AutomataIntersector_h__
#define __AutomataIntersector_h__

#include "File.h"
#include "Stringf.h"

class AutomataIntersector
{
public:

    void intersect( const StringTag &inputOne, const StringTag &inputTwo, const StringTag &output )
    {
        const char *GOAL_EXECUTABLE = "/home/guy/projects/prime_multiples/core/tools/GOAL-20120409/goal ";

        Stringf flags( "intersection -o %s %s %s > /dev/null", output.ascii(), inputOne.ascii(), inputTwo.ascii() );
        system( ( StringTag( GOAL_EXECUTABLE ) + flags ).ascii() );
    }

    void intersectAndSimplify( const StringTag &inputOne, const StringTag &inputTwo, StringTag output )
    {
        intersect( inputOne, inputTwo, output + ".tmp" );
        simplifyIntersectedAutomaton( output + ".tmp", output );
        removeTempFile( output + ".tmp" );
    }

private:
    unsigned _threadOne;
    unsigned _threadTwo;
    Vector<unsigned> _eventCodes;
    Vector<unsigned> _requestedOrBlockedEventCodes;
    bool _inAlphabetSection;
    Vector<StringTag> _requestOrBlockLines;

    void reset()
    {
        _eventCodes.clear();
        _requestOrBlockLines.clear();
        _requestedOrBlockedEventCodes.clear();

        _threadOne = _threadTwo = 0;
        _inAlphabetSection = false;
    }

    void removeTempFile( const StringTag &path )
    {
        if ( unlink( path.ascii() ) == -1 )
        {
            throw Error( Error::UNLINK_FAILED );
        }
    }

    void simplifyIntersectedAutomaton( const StringTag &original, const StringTag &simplified )
    {
        File input( original );
        File output( simplified );

        input.open( IFile::MODE_READ );
        output.open( IFile::MODE_WRITE );

        StringTag line;

        reset();

        while ( ( line = input.readLine() ).length() > 0 )
        {
            handleInputLine( output, line );
        }
    }

    void handleInputLine( File &output, StringTag &line )
    {
        if ( enteringAlphabetSection( line ) )
        {
            _inAlphabetSection = true;
            output.write( line + "\n" );
            return;
        }
        if ( leavingAlphabetSection( line ) )
        {
            _inAlphabetSection = false;
            ensureOneIsSmaller();
            dumpEvents( output );
            dumpRequestAndBlockLines( output );
            output.write( line + "\n" );
            return;
        }

        if ( inAlphabetSection() )
        {
            extractThreadsAndEvents( line );
        }
        else if ( transitionLine( line ) )
        {
            output.write( StringTag( "\t\t<Label>" ) + reworkTransition( line ) + "</Label>\n" );
        }
        else
        {
            output.write( line + "\n" );
        }
    }

    void extractThreadsAndEvents( StringTag line )
    {
        if ( ( line[ line.find( "<Proposition>" ) + strlen( "<Proposition>" ) ] == 'b' ) ||
             ( line[ line.find( "<Proposition>" ) + strlen( "<Proposition>" ) ] == 'r' ) )
        {
            unsigned threadNumber = line[ line.find( "<Proposition>" ) + strlen( "<Proposition>" ) + 1 ] - '0';

            if ( _threadOne == 0 )
            {
                _threadOne = threadNumber;
            }
            else
            {
                if ( _threadOne != threadNumber )
                {
                    _threadTwo = threadNumber;
                }
            }

            markEventAsRequestedOrBlocked( line );
            _requestOrBlockLines.append( line );
        }
        else
        {
            unsigned eventCode = line[ line.find( "<Proposition>" ) + strlen( "<Proposition>" ) + 1 ] - '0';
            if ( !_eventCodes.exists( eventCode ) )
            {
                _eventCodes.append( eventCode );
            }
        }
    }

    void markEventAsRequestedOrBlocked( StringTag line )
    {
        unsigned eventCode = ( line[ line.find( "<Proposition>" ) + strlen( "<Proposition>" ) + 3 ] - '0' );

        if ( !_requestedOrBlockedEventCodes.exists( eventCode ) )
        {
            _requestedOrBlockedEventCodes.append( eventCode );
        }
    }

    bool enteringAlphabetSection( const StringTag &line ) const
    {
        return line.contains( "<Alphabet" );
    }

    bool leavingAlphabetSection( const StringTag &line ) const
    {
        return line.contains( "</Alphabet" );
    }

    bool transitionLine( const StringTag &line ) const
    {
        return line.contains( "<Label>" );
    }

    bool inAlphabetSection() const
    {
        return _inAlphabetSection;
    }

    void dumpEvents( File &output )
    {
        for ( unsigned i = 0; i < _eventCodes.size(); ++i )
        {
            output.write( Stringf( "\t\t<Proposition>e%u</Proposition>\n", _eventCodes[i] ) );
        }
    }

    void dumpRequestAndBlockLines( File &output )
    {
        for ( unsigned i = 0; i < _requestOrBlockLines.size(); ++i )
        {
            StringTag line = _requestOrBlockLines[i];

            line.replace( Stringf( "r%u", _threadTwo ), Stringf( "r%u", _threadOne ) );
            line.replace( Stringf( "b%u", _threadTwo ), Stringf( "b%u", _threadOne ) );

            output.write( StringTag( "\t" ) + line + "\n" );
        }
    }

    StringTag reworkTransition( const StringTag &line )
    {
        StringTag result;

        for ( unsigned i = 0; i < _eventCodes.size(); ++i )
        {
            unsigned eventCode = _eventCodes[i];

            if ( !line.contains( Stringf( "~e%u", eventCode ) ) )
            {
                result += Stringf( "e%u ", eventCode );

                for ( unsigned j = 0; j < _eventCodes.size(); ++j )
                {
                    if ( i != j )
                    {
                        result += Stringf( "~e%u ", _eventCodes[j] );
                    }
                }
            }
        }

        for ( unsigned i = 0; i < _requestedOrBlockedEventCodes.size(); ++i )
        {
            unsigned eventCode = _requestedOrBlockedEventCodes[i];

            if ( ( ( line.contains( Stringf( "~b%ue%u", _threadOne, eventCode ) ) ) &&
                   ( line.contains( Stringf( "~b%ue%u", _threadTwo, eventCode ) ) ) )
                 ||
                 ( ( line.contains( Stringf( "~b%ue%u", _threadOne, eventCode ) ) ) &&
                   ( !line.contains( Stringf( "b%ue%u", _threadTwo, eventCode ) ) ) )
                 ||
                 ( ( !line.contains( Stringf( "b%ue%u", _threadOne, eventCode ) ) ) &&
                   ( line.contains( Stringf( "~b%ue%u", _threadTwo, eventCode ) ) ) ) )
            {
                result += Stringf( "~b%ue%u ", _threadOne, eventCode );
            }
            else
            {
                result += Stringf( "b%ue%u ", _threadOne, eventCode );
            }

            if ( ( ( line.contains( Stringf( "~r%ue%u", _threadOne, eventCode ) ) ) &&
                   ( line.contains( Stringf( "~r%ue%u", _threadTwo, eventCode ) ) ) )
                 ||
                 ( ( line.contains( Stringf( "~r%ue%u", _threadOne, eventCode ) ) ) &&
                   ( !line.contains( Stringf( "r%ue%u", _threadTwo, eventCode ) ) ) )
                 ||
                 ( ( !line.contains( Stringf( "r%ue%u", _threadOne, eventCode ) ) ) &&
                   ( line.contains( Stringf( "~r%ue%u", _threadTwo, eventCode ) ) ) ) )
            {
                result += Stringf( "~r%ue%u ", _threadOne, eventCode );
            }
            else
            {
                result += Stringf( "r%ue%u ", _threadOne, eventCode );
            }
        }

        return result;
    }

    void ensureOneIsSmaller()
    {
        if ( _threadOne > _threadTwo )
        {
            unsigned temp = _threadTwo;
            _threadTwo = _threadOne;
            _threadOne = temp;
        }
    }
};

#endif // __AutomataIntersector_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

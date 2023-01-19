#ifndef __GraphSimplifier_h__
#define __GraphSimplifier_h__

#include "Error.h"
#include "Stringf.h"

class GraphSimplifier
{
public:
    void simplify( const StringTag &programFile )
    {
        renameProgram( programFile );

        File input( _oldProgramFile.ascii() );
        File output( _newProgramFile.ascii() );

        input.open( IFile::MODE_READ );
        output.open( IFile::MODE_WRITE );

        StringTag line;

        while ( ( line = input.readLine() ).length() > 0 )
        {
            handleInputLine( output, line );
        }

        removeTempFile();
    }

private:
    StringTag _oldProgramFile;
    StringTag _newProgramFile;

    void renameProgram( StringTag programFile )
    {
        _oldProgramFile = programFile + ".original";
        _newProgramFile = programFile;

        if ( rename( _newProgramFile.ascii(), _oldProgramFile.ascii() ) == -1 )
        {
            throw Error( Error::RENAME_FAILED );
        }
    }

    void removeTempFile()
    {
        if ( unlink( _oldProgramFile.ascii() ) == -1 )
        {
            throw Error( Error::UNLINK_FAILED );
        }
    }

    void handleInputLine( File &output, StringTag &line )
    {
        if ( alphabetLine( line ) )
        {
            if ( eventLine( line ) )
            {
                output.write( line + "\n" );
            }
        }
        else if ( transitionLine( line ) )
        {
            output.write( StringTag( "\t\t<Label>" ) + onlyKeepEvents( line ) + "</Label>\n" );
        }
        else
        {
            output.write( line + "\n" );
        }
    }

    bool eventLine( StringTag line )
    {
        return
            ( line[ line.find( "<Proposition>" ) + strlen( "<Proposition>" ) ] != 'b' ) &&
            ( line[ line.find( "<Proposition>" ) + strlen( "<Proposition>" ) ] != 'r' );
    }

    bool transitionLine( const StringTag &line ) const
    {
        return line.contains( "<Label>" );
    }

    bool alphabetLine( const StringTag &line ) const
    {
        return line.contains( "<Proposition>" ) && line.contains( "</Proposition>" );
    }

    StringTag onlyKeepEvents( StringTag line )
    {
        StringTag result;
        Vector<StringTag> tokens = ( stripLabelTags( line ) ).tokenize();

        for ( unsigned i = 0; i < tokens.size(); ++i )
        {
            StringTag token = tokens[i];

            if ( !token.contains( "r" ) && !token.contains( "b" ) )
            {
                result += token + " ";
            }
        }

        return result;
    }

    StringTag stripLabelTags( StringTag input )
    {
        unsigned start = input.find( "<Label>" ) + strlen( "<Label>" );
        unsigned howMany = input.find( "</Label>" ) - start;

        return input.substring( start, howMany );
    }
};

#endif // __GraphSimplifier_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

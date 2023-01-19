#ifndef __ConfigurationFile_h__
#define __ConfigurationFile_h__

#include "AutoFile.h"
#include "Map.h"
#include "StringTag.h"

class ConfigurationFile
{
public:
    ConfigurationFile( const StringTag &path ) : _file( path )
    {
        parseFile();
    }

    StringTag getValue( const StringTag &key )
    {
        if ( !_values.exists( key ) )
        {
            throw Error( Error::VALUE_DOESNT_EXIST_IN_CONFIG_FILE );
        }

        return _values[key];
    }

private:
    AutoFile _file;
    Map<StringTag, StringTag> _values;

    void parseFile()
    {
        openFile();

        StringTag line;

        while ( ( line = _file->readLine() ) != "" )
        {
            parseSingleLine( line );
        }
    }

    void openFile()
    {
        _file->open( IFile::MODE_READ );
    }

    void parseSingleLine( StringTag line )
    {
        static const StringTag ASSIGNMENT_CHAR = "=";

        if ( !line.contains( ASSIGNMENT_CHAR ) )
        {
            return;
        }

        unsigned splitIndex = line.find( ASSIGNMENT_CHAR );

        StringTag key = line.substring( 0, splitIndex ).trim();
        StringTag value = line.substring( splitIndex + 1, line.length() - splitIndex ).trim();

        _values[key] = value;
    }
};

#endif // __ConfigurationFile_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

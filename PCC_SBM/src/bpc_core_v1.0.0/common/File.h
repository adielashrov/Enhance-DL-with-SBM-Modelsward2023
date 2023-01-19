#ifndef __File_h__
#define __File_h__

#include "Error.h"
#include "HeapData.h"
#include "IFile.h"
#include "StringTag.h"
#include "T/sys/stat.h"
#include "T/unistd.h"

class File : public IFile
{
public:
    File( const StringTag &path ) : _path( path ), _descriptor( NO_DESCRIPTOR )
    {
    }

    ~File()
    {
        closeIfNeeded();
    }

    void open( Mode mode )
    {
        if ( mode == IFile::MODE_READ )
        {
            openForReading();
        }
        else
        {
            openForWriting();
        }
    }

    void write( const StringTag &line )
    {
        if ( T::write( _descriptor, line.ascii(), line.length() ) != (int)line.length() )
        {
            throw Error( Error::WRITE_FAILED );
        }
    }

    void read( HeapData &buffer, unsigned maxReadSize )
    {
        char readBuffer[maxReadSize];
        int bytesRead;

        if ( ( bytesRead = T::read( _descriptor, readBuffer, sizeof(readBuffer) ) ) == -1 )
        {
            throw Error( Error::READ_FAILED );
        }

        buffer = ConstSimpleData( readBuffer, bytesRead );
    }

    StringTag readLine()
    {
        char byte;
        StringTag result;

        while ( ( T::read( _descriptor, &byte, sizeof(byte) ) ) == 1 )
        {
            if ( byte == '\n' )
            {
                return result;
            }
            else
            {
                result += StringTag( &byte, sizeof(byte) );
            }
        }

        return result;
    }

private:
    enum {
        NO_DESCRIPTOR = -1,
    };

    StringTag _path;
    int _descriptor;

    void closeIfNeeded()
    {
        if ( _descriptor != NO_DESCRIPTOR )
        {
            T::close( _descriptor );
            _descriptor = NO_DESCRIPTOR;
        }
    }

    void openForWriting()
    {
        if ( ( _descriptor = T::open( _path.ascii(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR ) ) == NO_DESCRIPTOR )
        {
            throw Error( Error::OPEN_FAILED );
        }
    }

    void openForReading()
    {
        mode_t DONT_CARE = NULL;

        if ( ( _descriptor = T::open( _path.ascii(), O_RDONLY, DONT_CARE ) ) == NO_DESCRIPTOR )
        {
            throw Error( Error::OPEN_FAILED );
        }
    }
};

#endif // __File_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

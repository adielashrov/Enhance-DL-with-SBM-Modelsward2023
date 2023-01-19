#ifndef __AutoFile_h__
#define __AutoFile_h__

#include "ISelector.h"
#include "IFile.h"
#include "T/FileFactory.h"

class AutoFile
{
public:
	AutoFile( const StringTag &path )
	{
		_file = T::createFile( path );
	}

	~AutoFile()
	{
		T::discardFile( _file );
		_file = 0;
	}

	operator IFile &()
	{
		return *_file;
	}

	IFile *operator->()
	{
		return _file;
	}

private:
	IFile *_file;
};

#endif // __AutoFile_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

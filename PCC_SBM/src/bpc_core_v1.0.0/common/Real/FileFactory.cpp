// #define CXXTEST_MOCK_REAL_SOURCE_FILE
// #include "T/FileFactory.h"

#include "File.h"

namespace T
{
	IFile *createFile( StringTag path )
	{
		return new File( path );
	}

	void discardFile( IFile *file )
	{
		delete file;
	}
}

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

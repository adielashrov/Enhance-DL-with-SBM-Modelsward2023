#ifndef __MockFile_h__
#define __MockFile_h__

#include "ConstSimpleData.h"
#include "HeapData.h"
#include "IFile.h"
#include "TestLog.h"
#include "Vector.h"

class MockFile : public IFile
{
public:
    class ReceiveCallBack
    {
    public:
        virtual void receiveWasCalled() = 0;
        virtual ~ReceiveCallBack() {}
    };

	MockFile()
	{
		wasCreated = false;
		wasDiscarded = false;
        openWasCalled = false;
    }

	bool wasCreated;
	bool wasDiscarded;
    StringTag lastPath;

	void mockConstructor( const StringTag &path )
	{
		TS_ASSERT( !wasCreated );
        wasCreated = true;

        lastPath = path;
	}

    void mockDestructor()
    {
        TS_ASSERT( wasCreated );
        TS_ASSERT( !wasDiscarded );

        wasDiscarded = true;
    }

    bool openWasCalled;
    IFile::Mode lastOpenMode;

    void open( Mode mode )
    {
        openWasCalled = true;
        lastOpenMode = mode;
    }

    Vector<StringTag> writtenData;

    void write( const StringTag &line )
    {
        TS_ASSERT( openWasCalled );

        writtenData.append( line );
    }

    Vector<StringTag> nextReadLines;

    StringTag readLine()
    {
        TS_ASSERT( openWasCalled );
        TS_ASSERT_EQUALS( lastOpenMode, IFile::MODE_READ );

        if ( nextReadLines.empty() )
        {
            return "";
        }

        return nextReadLines.popFirst();
    }
};

#endif // __MockFile_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//

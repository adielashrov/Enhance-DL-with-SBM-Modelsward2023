#ifndef __IFile_h__
#define __IFile_h__

class StringTag;

class IFile
{
public:
    enum Mode {
        MODE_READ,
        MODE_WRITE,
    };

    virtual void open( Mode mode ) = 0;
    virtual void write( const StringTag &line ) = 0;
    virtual StringTag readLine() = 0;

	virtual ~IFile() {}
};

#endif // __IFile_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

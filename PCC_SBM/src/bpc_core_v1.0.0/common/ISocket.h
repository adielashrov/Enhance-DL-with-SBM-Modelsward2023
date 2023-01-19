#ifndef __ISocket_h__
#define __ISocket_h__

class ConstSimpleData;
class HeapData;

class ISocket
{
public:
    virtual void send( const ConstSimpleData &data ) = 0;
	virtual void receive( HeapData &data ) = 0;
    virtual int descriptor() = 0;
    virtual void allowPortReuse() = 0;
    virtual void closeSocket() = 0;

	virtual ~ISocket() {}
};

#endif // __ISocket_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

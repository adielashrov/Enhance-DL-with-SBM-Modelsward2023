#ifndef __IConstSimpleData_h__
#define __IConstSimpleData_h__

class IConstSimpleData
{
public:
	virtual const void *data() const = 0;
	virtual unsigned size() const = 0;
	
	virtual ~IConstSimpleData() {}
};

#endif // __IConstSimpleData_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

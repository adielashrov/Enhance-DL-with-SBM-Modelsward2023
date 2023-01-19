#ifndef __IHeapData_h__
#define __IHeapData_h__

class IHeapData
{
public:
    virtual void *data() = 0;
    virtual unsigned size() const = 0;

    virtual ~IHeapData() {}
};

#endif // __IHeapData_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

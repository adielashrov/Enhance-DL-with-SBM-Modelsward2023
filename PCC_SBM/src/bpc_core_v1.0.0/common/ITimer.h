#ifndef __ITimer_h__
#define __ITimer_h__

class ITimer
{
public:
    class IExpired
    {
    public:
        virtual void expired() = 0;

        virtual ~IExpired() {}
    };

    virtual void set( unsigned seconds ) = 0;
    virtual void stop() = 0;

    virtual ~ITimer() {}
};

#endif // __ITimer_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

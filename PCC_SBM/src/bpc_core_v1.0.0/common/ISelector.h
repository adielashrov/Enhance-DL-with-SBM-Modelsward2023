#ifndef __ISelector_h__
#define __ISelector_h__

class ISelector
{
public:
	class ISelectable
	{
	public:
		virtual int descriptor() = 0;
		virtual void canRead() = 0;

		virtual ~ISelectable() {}
	};

	virtual void registerToSelector( ISelector::ISelectable &selectable ) = 0;
	virtual void unregisterFromSelector( ISelector::ISelectable &selectable ) = 0;
	virtual void select() = 0;
	
	virtual ~ISelector() {}
};

#endif // __ISelector_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __Selector_h__
#define __Selector_h__

#include "ISelector.h"
#include "Map.h"
#include "T/sys/select.h"

class Selector : public ISelector
{
public:
	void registerToSelector( ISelector::ISelectable &selectable )
	{
		_selectables[selectable.descriptor()] = &selectable;
	}

	void unregisterFromSelector( ISelector::ISelectable &selectable )
	{
		_selectables.erase( selectable.descriptor() );
	}

	void select()
	{
		prepareDescriptors();
		callSelect();
		callHandlers();
	}

private:	
	Map<int, ISelector::ISelectable *> _selectables;
	fd_set _readDescriptors;
	int _maxDescriptor;
	
	void zeroDescriptors()
	{
		FD_ZERO( &_readDescriptors );
		_maxDescriptor = 0;
	}

	void addDescriptor( int descriptor )
	{
		if ( descriptor + 1 > _maxDescriptor )
		{
			_maxDescriptor = descriptor + 1;
		}
		
		FD_SET( descriptor, &_readDescriptors );
	}
	
	void prepareDescriptors()
	{
		zeroDescriptors();
		
		for ( Map<int, ISelector::ISelectable *>::iterator it = _selectables.begin(); it != _selectables.end(); ++it )
		{
			addDescriptor( it->first );
		}
	}
	
	void callSelect()
	{
		fd_set *NO_WRITE_DESCRIPTORS = NULL;
		fd_set *NO_EXCETPION_DESCRIPTORS = NULL;
		struct timeval *BLOCK_FOREVER = NULL;
 
		if ( T::select( _maxDescriptor, 
						&_readDescriptors, 
						NO_WRITE_DESCRIPTORS, 
						NO_EXCETPION_DESCRIPTORS, 
						BLOCK_FOREVER ) == -1 )
		{
			throw Error( Error::SELECT_FAILED );
		}
	}

	void callHandlers()
	{
		Map<int, ISelector::ISelectable *> copy = _selectables;
		for ( Map<int, ISelector::ISelectable *>::iterator it = copy.begin(); it != copy.end(); ++it )
		{
			if ( descriptorSet( it->first ) )
			{
				it->second->canRead();
			}
		}		
	}

	bool descriptorSet( int descriptor )
	{
		return FD_ISSET( descriptor, &_readDescriptors );
	}
};

#endif // __Selector_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

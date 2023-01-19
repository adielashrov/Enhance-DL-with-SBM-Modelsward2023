#ifndef __HeapData_h__
#define __HeapData_h__

#include "ConstSimpleData.h"
#include "Error.h"
#include "IHeapData.h"
#include "T/stdlib.h"
#include <string.h>
#include <cstdio>

class HeapData : public IHeapData
{
public:
	HeapData() : _data( 0 ), _size( 0 )
	{
	}

	HeapData( void *data, unsigned size ) : _data( 0 ), _size( 0 )
	{
		allocateMemory( size );
		memcpy( _data, data, size );
	}

	HeapData( HeapData &other ) : _data( 0 ), _size( 0 )
	{
		( *this )+=other;
	}

	HeapData( const IConstSimpleData &constSimpleData )
	{
		allocateMemory( constSimpleData.size() );
		memcpy( _data, constSimpleData.data(), constSimpleData.size() );
	}

	HeapData( const HeapData &other )
	{
		allocateMemory( other.size() );
		memcpy( _data, other.data(), other.size() );
	}

	~HeapData()
	{
		freeMemoryIfNeeded();
	}

	HeapData &operator=( const IConstSimpleData &data )
	{
		freeMemoryIfNeeded();
		allocateMemory( data.size() );
		memcpy( _data, data.data(), data.size() );
		return *this;
	}

	HeapData &operator=( const HeapData &other )
	{
		// Guard self assignment
		if (this == &other){
			return *this;
		}

		freeMemoryIfNeeded();
		allocateMemory( other.size() );
		memcpy( _data, other.data(), other.size() );
		return *this;
	}

	bool operator==( const HeapData &other ) const
	{
		// check self assignment
		if (this == &other){
			return true;
		}

		const char * thisDataAsChar = asChar();
		const char * otherDataAsChar = other.asChar();

		printf("HeapData.operator== this.asChar: {%s}, other.asChar: {%s} \n", thisDataAsChar, otherDataAsChar );

		if( strcmp(thisDataAsChar, otherDataAsChar) == 0 )
		{
			printf( "HeapData.operator== Values are equal! \n" );
			return true;
  		}

  		printf( "HeapData.operator== Values are not equal! \n" );
		return false;
	}

	HeapData &operator+=( const IConstSimpleData &data )
	{
		addMemory( data.size() );
		copyNewData( data.data(), data.size() );
		adjustSize( data.size() );

	return *this;
	}

	HeapData &operator+=( IHeapData &data )
	{
		( *this )+=( ConstSimpleData( data ) );
		return *this;
	}

	void *data()
	{
		return _data;
	}

	const void *data() const
	{
		return _data;
	}

	unsigned size() const
	{
		return _size;
	}

	void clear()
	{
		freeMemoryIfNeeded();
	}

	char *asChar()
	{
		return (char *)_data;
	}

	const char *asChar() const
	{
		return (const char *)_data;
	}

	void hexDump() const
	{
		for ( unsigned i = 0; i < size(); ++i )
		{
			printf( "%02x ", (unsigned char)asChar()[i] );
		}
		printf( "\n" );
		fflush( 0 );
	}

private:
	void *_data;
	unsigned _size;

	bool allocated() const
	{
		return _data != NULL;
	}

	void freeMemory()
	{
		T::free( _data );
		_data = NULL;
		_size = 0;
	}

	void freeMemoryIfNeeded()
	{
		if ( allocated() )
		{
			freeMemory();
		}
	}

	void allocateMemory( unsigned size )
	{
		_size = size;

		if ( ( _data = T::malloc( _size ) ) == NULL )
		{
			throw Error( Error::NOT_ENOUGH_MEMORY );
		}
	}

	void addMemory( unsigned size )
	{
		if ( ( _data = T::realloc( _data, _size + size ) ) == NULL )
		{
			throw Error( Error::NOT_ENOUGH_MEMORY );
		}
	}

	void copyNewData( const void *newData, unsigned size )
	{
		memcpy( (char *)_data + _size, newData, size );
	}

	void adjustSize( unsigned size )
	{
		_size += size;
	}
};

#endif // __HeapData_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

#ifndef __ConstSimpleData_h__
#define __ConstSimpleData_h__

#include "IConstSimpleData.h"
#include "IHeapData.h"
#include <cstring>
#include <cstdio>

class ConstSimpleData : public IConstSimpleData
{
public:
	ConstSimpleData( const void *data, unsigned size ) : _data( data ), _size( size )
	{
	}

    ConstSimpleData( IHeapData &data ) : _data( data.data() ), _size( data.size() )
    {
    }

    bool operator==( const ConstSimpleData &other ) const
	{
		// Guard self assignment
		if (this == &other){
			return true;
		}

		if((!this->data() && other.data()) || (this->data() && !other.data())) // if one of the data values is null
		{
			printf("\n ConstSimpleData -> one of the data is null: \n this.data(): %s, other.data(): %s \n", (char *) this->data(), (char *) other.data());
			return false;
		}
		else if(!this->data() && !other.data()) // if both data values are null
		{
			printf("\n ConstSimpleData -> both data members are null: \n this.data(): %s, other.data(): %s \n", (char *) this->data(), (char *) other.data());
			return true;
		}

		const char* thisDataAsChar = this->asChar();
		const char* otherDataAsChar = other.asChar();

		if(!thisDataAsChar)
		{
			printf("\n thisDataAsChar is null\n");
			return false;
		}
		if(!otherDataAsChar)
		{
			printf("\n otherDataAsChar is null\n");
			return false;	
		}

		// printf("\nConstSimpleData -> thisDataAsChar: %s\n otherDataAsChar: %s\n", thisDataAsChar, otherDataAsChar);

		if( strcmp(thisDataAsChar, otherDataAsChar) == 0 )
		{
			// printf( "ConstSimpleData -> operator== Values are equal! \n" );
			return true;
  		}

  		// printf( "ConstSimpleData -> operator== Values are not equal! \n" );
		return false;
	}

	/* TODO: maybe we need to use memcpy here
	ConstSimpleData &operator=( const ConstSimpleData &other )
	{
    	// Guard self assignment
    	if (this == &other){
    		return *this;
    	}

    	_data = other.data();
    	_size = other.size();

    	return *this;
	}
	*/

	const void *data() const
	{
		return _data;
	}

	unsigned size() const
	{
		return _size;
	}

    const char *asChar() const
    {
        return (char *)_data;
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
	const void *_data;
	unsigned _size;

	bool checkNullPayload( const ConstSimpleData &other ) const
	{
		/*
		if(!this->data() && other.data() || this->data() && !other.data))
		{
			printf("\n ConstSimpleData -> one of the data is null: \n this.data(): %s, other.data(): %s \n", this->data(), other.data());
			return false;
		}
		else if(!this->data() && !other.data())
		{
			printf("\n ConstSimpleData -> both(!) data members are null: \n this.data(): %s, other.data(): %s \n", this->data(), other.data());
			return true;
		}
		*/
		return true;
	}
};

#endif // __ConstSimpleData_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

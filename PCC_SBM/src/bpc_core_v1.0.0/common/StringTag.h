#ifndef __StringTag_h__
#define __StringTag_h__

#include "Vector.h"
#include <cstring>
#include <string>

class StringTag
{
public:
	typedef std::string Super;

	StringTag( Super super ) : _super( super )
	{
	}

	StringTag()
	{
	}

    StringTag( const char *string, unsigned length ) : _super( string, length )
    {
    }

	StringTag( const char *string ) : _super( string )
	{
	}

	StringTag( const StringTag &other ) : _super( other._super )
	{
		// printf( " Inside StringTag copy constructor:%s\n", other.ascii() );
	}

	unsigned length() const
	{
		return _super.length();
	}

	const char *ascii() const
	{
		return _super.c_str();
	}

	char operator[]( int index )
	{
		return _super[index];
	}

	StringTag &operator=( const char *string )
	{
		*this = StringTag( string );
		return *this;
	}

	StringTag &operator=( const StringTag &other )
	{
		_super = other._super;
		return *this;
	}

	bool operator==( const char *string )
	{
		return _super.compare( string ) == 0;
	}

	StringTag operator+( const StringTag &other )
	{
		return _super + other._super;
	}

	StringTag operator+( const char *other )
	{
		return _super + other;
	}

	StringTag operator+=( const StringTag &other )
	{
		return _super += other._super;
	}

	StringTag operator+=( const char *other )
	{
		return _super += other;
	}

	bool operator==( const StringTag &other )
	{
        return _super.compare( other._super ) == 0;
	}

    bool operator!=( const StringTag &other )
    {
        return _super.compare( other._super ) != 0;
    }

    bool operator<( const StringTag &other ) const
    {
        return _super < other._super;
    }

    Vector<StringTag> tokenize()
    {
        Vector<StringTag> tokens;
        char delimiter[] = " ";

        char copy[length()+1];
        memcpy( copy, ascii(), sizeof(copy) );

        char *token = strtok( copy, delimiter );

        while ( token != NULL )
        {
            tokens.append( StringTag( token ) );
            token = strtok( NULL, delimiter );
        }

        return tokens;
    }

    bool contains( const StringTag &substring ) const
    {
        bool retValue = false;
        if(find(substring) != Super::npos)
        {
            retValue = true;
        }
        
        return retValue;
    }

    size_t find( const StringTag &substring ) const
    {
        size_t retValue = _super.find(substring._super);    
        return retValue;
    }

    StringTag substring( unsigned fromIndex, unsigned howMany )
    {
        return _super.substr( fromIndex, howMany );
    }

    void replace( const StringTag &toReplace, const StringTag &replaceWith )
    {
        if ( find( toReplace ) != Super::npos )
        {
            _super.replace( find( toReplace ), toReplace.length(), replaceWith._super );
        }
    }

    StringTag trim()
    {
        unsigned firstNonSpace = 0;
        unsigned lastNonSpace = 0;

        bool firstNonSpaceFound = false;

        for ( unsigned i = 0; i < length(); ++i )
        {
            if ( ( !firstNonSpaceFound ) && ( _super[i] != ' ' ) )
            {
                firstNonSpace = i;
                firstNonSpaceFound = true;
            }

            if ( ( _super[i] != ' ' ) && ( _super[i] != '\n' ) )
            {
                lastNonSpace = i;
            }
        }

        if ( lastNonSpace == 0 )
        {
            return "";
        }

        return substring( firstNonSpace, lastNonSpace - firstNonSpace + 1 );
    }

protected:
	Super _super;
};

// #ifdef CXXTEST_RUNNING
// namespace CxxTest { CXXTEST_COPY_TRAITS( StringTag, std::string ); }
// #endif // CXXTEST_RUNNING

#endif // __StringTag_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

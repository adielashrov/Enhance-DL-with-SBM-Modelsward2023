#ifndef __SyncRequest_h__
#define __SyncRequest_h__

#include "AtomicPropositions.h"
#include "Event.h"
#include "Vector.h"

class ListOfEvents
{
public:
    ListOfEvents()
    {
    }

    ListOfEvents( const Vector<Event> &events )
    {
    	_events = events;
    }

    void serialize( HeapData &serialized )
    {
        addNumberOfEvents( serialized );
        for ( unsigned i = 0; i < _events.size(); ++i )
        {
            addEvent( serialized, i );
        }
    }

    void deserialize( const ConstSimpleData &serialized )
    {
        const char *asChar = serialized.asChar();

        unsigned eventCount = extractUnsigned( asChar );
        asChar += sizeof(unsigned);

        for ( unsigned i = 0; i < eventCount; ++i )
        {
            unsigned singleEventSize = extractUnsigned( asChar );
            asChar += sizeof(unsigned);

            createNewEvent( asChar, singleEventSize );
            asChar += singleEventSize;
        }

        _totalSize = asChar - ( (char*)serialized.data() );

        _createdEvents = true;
    }

    unsigned totalSize()
    {
        return _totalSize;
    }

    Vector<Event> _events;
private:
    unsigned _totalSize;
    bool _createdEvents;

    void addNumberOfEvents( HeapData &serialized )
    {
        unsigned numberOfEvents = _events.size();
        serialized = ConstSimpleData( &numberOfEvents, sizeof(numberOfEvents) );
    }

    void addEvent( HeapData &serialized, unsigned index )
    {
        HeapData serializedEvent;
        _events[index].serialize( serializedEvent );
        unsigned eventSize = serializedEvent.size();

        serialized += ConstSimpleData( &eventSize, sizeof(eventSize) );
        serialized += serializedEvent;
    }

    void createNewEvent( const char *serialized, unsigned size )
    {
        Event event;
        event.deserialize( ConstSimpleData( serialized, size ) );
        _events.append( event );
    }

    unsigned extractUnsigned( const char *serialized )
    {
        return *(unsigned *)serialized;
    }
};

class SyncRequest
{
public:
    SyncRequest( const Vector<Event> &requested,
                 const Vector<Event> &watched,
                 const Vector<Event> &blocked,
                 const AtomicPropositions &atomicPropositions,
                 StringTag label = "" ) :
        _requested( requested ),
        _watched( watched ),
        _blocked( blocked ),
        _atomicPropositions( atomicPropositions ),
        _label( label )
    {
    }

    void serialize( HeapData &serialized )
    {
        addLabel( serialized );
        addRequested( serialized );
        addWatched( serialized );
        addBlocked( serialized );
        addAtomicPropositions( serialized );
    }

    SyncRequest( const ConstSimpleData &serialized )
    {
        unsigned offset;

        extractLabel( serialized, offset );
        extractRequested( serialized, offset );
        extractWatched( serialized, offset );
        extractBlocked( serialized, offset );
        extractAtomicPropositions( serialized, offset );
    }

    Vector<Event> getRequested()
    {
        return _requested._events;
    }

    Vector<Event> getWatched()
    {
        return _watched._events;
    }

    Vector<Event> getBlocked()
    {
        return _blocked._events;
    }

    StringTag getLabel()
    {
        return _label;
    }

    AtomicPropositions getAtomicPropositions()
    {
        return _atomicPropositions;
    }

private:
    ListOfEvents _requested;
    ListOfEvents _watched;
    ListOfEvents _blocked;
    AtomicPropositions _atomicPropositions;
    StringTag _label;

    void addLabel( HeapData &serialized )
    {
        unsigned labelLength = _label.length();
        serialized = ConstSimpleData( &labelLength, sizeof(labelLength) );

        ConstSimpleData labelString( _label.ascii(), _label.length() );
        serialized += labelString;
    }

    void addAtomicPropositions( HeapData &serialized )
    {
        unsigned numberOfPropositions = _atomicPropositions.size();
        serialized += ConstSimpleData( &numberOfPropositions, sizeof(numberOfPropositions) );

        for ( unsigned i = 0; i < numberOfPropositions; ++i )
        {
            StringTag proposition = _atomicPropositions.at( i );
            unsigned propositionLength = proposition.length();
            serialized += ConstSimpleData( &propositionLength, sizeof(propositionLength) );

            ConstSimpleData propositionString( proposition.ascii(), propositionLength );
            serialized += propositionString;
        }
    }

    void addRequested( HeapData &serialized )
    {
        HeapData temp;
        _requested.serialize( temp );
        serialized += temp;
    }

    void addWatched( HeapData &serialized )
    {
        HeapData temp;
        _watched.serialize( temp );
        serialized += temp;
    }

    void addBlocked( HeapData &serialized )
    {
        HeapData temp;
        _blocked.serialize( temp );
        serialized += temp;
    }

    void extractLabel( const ConstSimpleData &serialized, unsigned &offset )
    {
        unsigned labelLength;
        memcpy( &labelLength, serialized.asChar(), sizeof(labelLength) );

        offset = sizeof(unsigned);

        _label = StringTag( serialized.asChar() + offset, labelLength );

        offset += labelLength;
    }

    void extractRequested( const ConstSimpleData &serialized, unsigned &offset )
    {
        _requested.deserialize( ConstSimpleData( serialized.asChar() + offset, serialized.size() - offset ) );
        offset += _requested.totalSize();
    }

    void extractWatched( const ConstSimpleData &serialized, unsigned &offset )
    {
        _watched.deserialize( ConstSimpleData( serialized.asChar() + offset, serialized.size() - offset ) );
        offset += _watched.totalSize();
    }

    void extractBlocked( const ConstSimpleData &serialized, unsigned &offset )
    {
        _blocked.deserialize( ConstSimpleData( serialized.asChar() + offset, serialized.size() - offset ) );
        offset += _blocked.totalSize();
    }

    void extractAtomicPropositions( const ConstSimpleData &serialized, unsigned &offset )
    {
        unsigned numberOfPropositions;
        memcpy( &numberOfPropositions, serialized.asChar() + offset, sizeof(numberOfPropositions) );
        offset += sizeof(numberOfPropositions);

        for ( unsigned i = 0; i < numberOfPropositions; ++i )
        {
            unsigned propositionLength;
            memcpy( &propositionLength, serialized.asChar() + offset, sizeof(propositionLength) );
            offset += sizeof(propositionLength);

            _atomicPropositions.add( StringTag( serialized.asChar() + offset, propositionLength ) );
            offset += propositionLength;
        }
    }
};

#endif // __SyncRequest_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

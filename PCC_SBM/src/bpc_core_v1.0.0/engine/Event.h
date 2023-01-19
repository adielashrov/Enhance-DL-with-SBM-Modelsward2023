#ifndef __Event_h__
#define __Event_h__

#include "IConstSimpleData.h"
#include "StringTag.h"
#include "HeapData.h"
#include <string.h>
#include <cstdio>
#include "../../pcc/monitor_interval/pcc_mi.h"

class Event
{
public:
    
    Event() : _type( -1 ), _id( 0 ), _monitorInterval(NULL), _next_sending_rate( -1.0 )
    {
    }

    Event( unsigned type ) : _type( type ), _id( 0 ), _monitorInterval(NULL), _next_sending_rate( -1.0 )
    {
    }

    Event( unsigned type, unsigned id ) : _type( type ), _id( id ), _monitorInterval(NULL), _next_sending_rate( -1.0 )
    {
    }

    Event( unsigned type, unsigned id ,MonitorInterval* monitorInterval ) : _type( type ), _id( id ), _monitorInterval(monitorInterval), _next_sending_rate( -1.0 )
    {
    }

    Event( unsigned type, unsigned id , MonitorInterval* monitorInterval, double next_sending_rate) : _type( type ), _id( id ), _monitorInterval(monitorInterval), _next_sending_rate( next_sending_rate )
    {
    }

    Event( const Event &other )
    {
    	_type = other.type();
        _id = other.id();
        _monitorInterval = other.monitorInterval();
        _next_sending_rate = other.nextSendingRate();
    }

   ~Event()
   {
   }

    void serialize( HeapData &serialized )
    {
        serializeType(serialized);
        serializeId(serialized);
        serializeMonitorInterval(serialized);
        serializeNextSendingRate(serialized);
    }

    void deserialize( const ConstSimpleData &serialized )
    {
        unsigned index = 0;
        deserializeType(serialized, index);
        deserializeId(serialized, index);
        deserializeMonitorInterval(serialized, index);
        deserializeNextSendingRate(serialized, index);
    }

    unsigned type() const
    {
        return _type;
    }

    unsigned id() const
    {
        return _id;
    }


    MonitorInterval* monitorInterval() const
    {
        return _monitorInterval;
    }

    double nextSendingRate() const
    {
        return _next_sending_rate;
    }


    unsigned totalSize() const
    {
        return sizeof(_type) + sizeof(_id) + sizeof(_monitorInterval) + sizeof(_next_sending_rate);  
    }

    bool operator==( const Event &other ) const
    {
    	return compareToEvent(other);
    }

    bool operator!=( const Event &other ) const
    {
    	return !compareToEvent(other);
    }

    bool operator<( const Event &other ) const
    {
        return type() < other.type();
    }

    Event &operator=( const Event &other )
    {
        _type = other.type();
        _id = other.id();
        _monitorInterval = other.monitorInterval();
        _next_sending_rate = other.nextSendingRate();

        return *this;
    }

    void printEvent() const
    {
        printf("Event[%d] is of type (%d), monitor_interval address: %p,_next_sending_rate: %f \n", id(), type(), monitorInterval(), nextSendingRate());
	}

protected:
    unsigned _type;
    unsigned _id;
    MonitorInterval* _monitorInterval;
    double _next_sending_rate;

private:
    void serializeType( HeapData &serialized )
    {
        serialized = ConstSimpleData( &_type, sizeof(_type) );
    }

    void serializeId( HeapData &serialized )
    {
        serialized += ConstSimpleData( &_id, sizeof(_id) );
    }

    void serializeMonitorInterval( HeapData &serialized )
    {
        serialized += ConstSimpleData( &_monitorInterval, sizeof(_monitorInterval) );
    }

    void serializeNextSendingRate( HeapData &serialized )
    {
        serialized += ConstSimpleData( &_next_sending_rate, sizeof(_next_sending_rate) );
    }

    
    void deserializeType( const ConstSimpleData &serialized, unsigned &index )
    {
        memcpy( &_type, serialized.asChar() + index, sizeof(_type));
        index += sizeof(_type);
    }

    void deserializeId( const ConstSimpleData &serialized, unsigned &index )
    {
        memcpy( &_id, serialized.asChar() + index, sizeof(_id) );
        index += sizeof(_id);
    }
    
    void deserializeMonitorInterval( const ConstSimpleData &serialized, unsigned &index )
    {
        memcpy( &_monitorInterval, serialized.asChar() + index, sizeof(_monitorInterval) );
        index += sizeof(_monitorInterval);
    }

    void deserializeNextSendingRate( const ConstSimpleData &serialized, unsigned &index )
    {
        memcpy( &_next_sending_rate, serialized.asChar() + index, sizeof(_next_sending_rate) );
        index += sizeof(_next_sending_rate);
    }

    bool compareToEvent( const Event &other ) const
    {
        // printCompareLogs(other);
        bool compareTypes = type() == other.type();
        //TODO: Remove comparison to ID as well?
        // bool compareId = id() ==  other.id();
        bool compareId = true;
        
        //TODO: Should we compare the _monitorInterval/nextSendingRate as well? 

        bool retValue = compareTypes && compareId;
        return retValue;
    }

    void printCompareLogs( const Event &other ) const
    {
        printf("Enter - compare to Event...\n");
        printf("Events compare -> this.type: {%d}, other.type: {%d} \n", type(), other.type());
        printf("Events compare -> this.id: {%d}, other.id: {%d} \n", id(), other.id());
        printf("Events compare -> this.monitorInterval: {%p}, other.monitorInterval: {%p} \n", monitorInterval(), other.monitorInterval());
        printf("Events compare -> this.nextSendingRate: {%f}, other.nextSendingRate: {%f} \n", nextSendingRate(), other.nextSendingRate());
    }

};

#endif // __Event_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

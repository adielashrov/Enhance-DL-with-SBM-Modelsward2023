#include "SendRateSensorBthread.h"


SendRateSensorBthread::SendRateSensorBthread() : BThread("SendRateSensorBthread")
{
    this->next_sending_rate = 6000000.0000000;
    this->num_of_get_next_sending_rate = 0;
}

SendRateSensorBthread::~SendRateSensorBthread()
{
}

void SendRateSensorBthread::entryPoint()
{
    printf("Enter SendRateSensorBthread...\n" );
    
    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;

    int id = 1;
    
    while(true)
    {
        if(!this->events_ids.empty())
        {
            int event_id = this->events_ids.back();
            this->events_ids.pop_back();
            Event queryNextSendingRateEvent(1, event_id); // 1 signals queryNextSendingRateEvent
            requested.clear();
            watched.clear();
            blocked.clear();
            requested.append(queryNextSendingRateEvent);
            bSync(requested, watched, blocked, "SendRateSensorBthread");
            Event lastEvent = this->lastEvent();
            id = id + 1;
        }
    }

    done();
    printf("Leave SendRateSensorBthread...\n" );
}

void SendRateSensorBthread::notifyOnGetNextSendingRate(int id)
{
    auto it = this->events_ids.begin();
    it = this->events_ids.insert(it, id);
}
 #include "SendRateBthread.h"


SendRateBthread::SendRateBthread() : BThread("SendRateBthread")
{
    this->next_sending_rate = 6000000.0000000;
    this->num_of_get_next_sending_rate = 0;
}

SendRateBthread::~SendRateBthread()
{
}

void SendRateBthread::entryPoint()
{
    printf("Enter SendRateBthread...\n" );
    
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
            Event getNextSendingRateEvent(1, event_id); // 1 signals GetNextSendingRateEvent
            requested.clear();
            watched.clear();
            blocked.clear();
            requested.append(getNextSendingRateEvent);
            // printf("SendRate: bSync(getNextSendingRateEvent, none, none), event.id: %d \n", event_id); // Stopeed here
            bSync(requested, watched, blocked, "SendRateBthread");
            Event lastEvent = this->lastEvent();
            // printf("SendRate: lastEvent.getNextSendingRateEvent, event.id: %d, type: %d\n", lastEvent.id(), lastEvent.type()); 
            id = id + 1;
            // usleep(1000);
        }
    }

    done();
    printf("Leave SendRateBthread...\n" );
}

void SendRateBthread::notifyOnGetNextSendingRate(int id)
{
    // printf("SendRateBthread::notifyOnGetNextSendingRate-> adding id: %d\n", id);
    auto it = this->events_ids.begin();
    it = this->events_ids.insert(it, id);
}
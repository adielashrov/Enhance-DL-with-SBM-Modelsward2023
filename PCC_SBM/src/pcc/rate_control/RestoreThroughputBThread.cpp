#include "RestoreThroughputBThread.h"


RestoreThroughputBThread::RestoreThroughputBThread() : BThread("RestoreThroughputBThread")
{
    this->restore_policy = 2;
    this->next_sending_rate_for_restore = -1.0;
    this->sending_rate_upper_limit = 300000000.0;
    this->time_counter = -1;
}

RestoreThroughputBThread::~RestoreThroughputBThread()
{
}

void RestoreThroughputBThread::entryPoint()
{
     printf("Enter RestoreThroughputBThread...\n" );
    
    int id = 1;
    bool enterRestoreThroughput = false;
    Event lastEvent_2;
    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;

    while(true)
    {
        Event updateSendingRate(2, id); // 2 signals updateSendingRate from the model output
        Event enterYieldEvent(5, id); // 5 signals yield bandwidth event
        Event enterRestoreEvent(6, id); // 6 signals restore bandwidth event
        requested.clear();
        watched.clear();
        blocked.clear();
        watched.append(updateSendingRate);
        watched.append(enterYieldEvent);
        watched.append(enterRestoreEvent);
        bSync(requested, watched, blocked, "RestoreThroughputBThread");
        Event lastEvent = this->lastEvent();
        
        if(lastEvent.type() == 2) // updateSendingRate
        {
            int last_event_id = lastEvent.id();
            double model_sending_rate = lastEvent.nextSendingRate();
            double updated_sending_rate = -1.0;
            if(enterRestoreThroughput)
            {
                updated_sending_rate = getRestoreSendingRate(model_sending_rate);
            }

            Event monitorIntervalEvent(0, id);
            Event queryNextSendingRateEvent(1, id);
            Event updateSendingRateReduceEvent(3, id); // 3 signals updateSendingRateReduceEvent
            Event updateSendingRateRestoreEvent(4, last_event_id, NULL, updated_sending_rate); // 4 signals updateSendingRateRestoreEvent - last event id is critical
        
            requested.clear();
            watched.clear();
            blocked.clear();

            if(!enterRestoreThroughput) // Indication for restoring bandwidth
            {
                watched.append(updateSendingRateReduceEvent);
            }
            else
            {
                blocked.append(monitorIntervalEvent);
                blocked.append(queryNextSendingRateEvent);
                blocked.append(updateSendingRateReduceEvent);
                requested.append(updateSendingRateRestoreEvent);
            }
        
            bSync(requested, watched, blocked, "RestoreThroughputBThread");
            lastEvent_2 = this->lastEvent();
        }
        else if(lastEvent.type() == 5) // enterYieldEvent
        {
            this->next_sending_rate_for_restore = -1.0;
            enterRestoreThroughput = false; 
        }
        else if(lastEvent.type() == 6) // enterRestoreEvent
        {
            if(this->next_sending_rate_for_restore == -1.0)
            {
                setInitialSendingRateForRestore(lastEvent_2.nextSendingRate());
            }
            enterRestoreThroughput = true;
        }
        
        ++id;
    }
    
    done();
    printf("Leave RestoreThroughputBThread...\n" );
}

void RestoreThroughputBThread::setInitialSendingRateForRestore(double sending_rate)
{
    this->next_sending_rate_for_restore = sending_rate;
    this->time_counter = 1;
}

double RestoreThroughputBThread::getRestoreSendingRate(double model_sending_rate)
{
    double updated_sending_rate = -1.0;
    if(this->restore_policy == 1) // 1 - Slow start - increase * 2 until max value
    {
        updated_sending_rate = this->next_sending_rate_for_restore;
        if((this->time_counter % 1000) == 0)
        {
            this->next_sending_rate_for_restore = this->next_sending_rate_for_restore * 2;
            if(this->next_sending_rate_for_restore >= this->sending_rate_upper_limit)
            {
                this->next_sending_rate_for_restore = this->sending_rate_upper_limit;
                if((this->time_counter > 3000) == 0)
                {
                    this->restore_policy = 2;
                }
            }
        }
    }
    else // 2 - model_sending_rate policy
    {
        updated_sending_rate = model_sending_rate;
    }
    
    this->time_counter = this->time_counter + 1;
    return updated_sending_rate;
}
     #include "RestoreThroughputBThread.h"


RestoreThroughputBThread::RestoreThroughputBThread() : BThread("RestoreThroughputBThread")
{
    this->restore_policy = 1;
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
        Event restoreThroughputEvent(6, id); // 6 signals restore bandwidth event
        requested.clear();
        watched.clear();
        blocked.clear();
        watched.append(updateSendingRate);
        watched.append(enterYieldEvent);
        watched.append(restoreThroughputEvent);
        // printf("RestoreThroughputBThread: bSync(none, {updateSendingRate, restoreThroughputEvent}, none)...\n");
        bSync(requested, watched, blocked, "RestoreThroughputBThread"); // Stopped here
        Event lastEvent = this->lastEvent();
        
        if(lastEvent.type() == 2) // updateSendingRate
        {
            // printf("RestoreThroughputBThread: lastEvent.updateSendingRateModel id: (%d), type: (%d), sending_rate: (%f)\n", lastEvent.id(), lastEvent.type(), lastEvent.nextSendingRate());
            int last_event_id = lastEvent.id();
            double model_sending_rate = lastEvent.nextSendingRate();
            double next_real_update_sending_rate = -1.0;
            if(enterRestoreThroughput)
            {
                next_real_update_sending_rate = getRestoreSendingRate(model_sending_rate);
            }

            Event monitorIntervalEvent(0, id);
            Event queryNextSendingRateEvent(1, id);
            Event updateSendingRateIdentifyThread(3, id); // 3 signals updateSendingRateReal
            Event updateSendingRateRestoreThread(4, last_event_id, NULL, next_real_update_sending_rate); // 4 signals updateSendingRateRestoreThread - last event id is critical
        
            requested.clear();
            watched.clear();
            blocked.clear();

            if(!enterRestoreThroughput) // Indication for restoring bandwidth
            {
                watched.append(updateSendingRateIdentifyThread);
                // printf("RestoreThroughputBThread: bSync(none, updateSendingRateIdentifyThread, none)...\n");
            }
            else
            {
                blocked.append(monitorIntervalEvent);
                blocked.append(queryNextSendingRateEvent);
                blocked.append(updateSendingRateIdentifyThread);
                requested.append(updateSendingRateRestoreThread);
                // printf("RestoreThroughputBThread: bSync(updateSendingRateRestoreThread, none, {monitorIntervalEvent, queryNextSendingRateEvent, updateSendingRateIdentifyThread})...\n");
            }
        
            bSync(requested, watched, blocked, "RestoreThroughputBThread");
            lastEvent_2 = this->lastEvent();
            if(enterRestoreThroughput)
            {
                // printf("RestoreThroughputBThread: lastEvent.updateSendingRate(Identify/Restore), id: (%d), type: (%d), sending_rate: (%f)\n", lastEvent_2.id(), lastEvent_2.type(), lastEvent_2.nextSendingRate());
            }
        }
        else if(lastEvent.type() == 5) // yield bandwidth event
        {
            // printf("****RestoreThroughputBThread: lastEvent: enterYieldEvent****\n");
            this->next_sending_rate_for_restore = -1.0;
            enterRestoreThroughput = false; 
        }
        else if(lastEvent.type() == 6) // restoreThroughputEvent
        {
            // printf("****RestoreThroughputBThread: lastEvent: restoreThroughputEvent****\n");
            if(this->next_sending_rate_for_restore == -1.0)
            {
                // printf("RestoreThroughputBThread: Setting next_sending_rate_for_restore: (%f)\n", lastEvent_2.nextSendingRate());
                setInitialSendingRateForRestore(lastEvent_2.nextSendingRate());
            }
            enterRestoreThroughput = true;
        }
        
        ++id;
        // usleep(100000);
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
    double next_real_update_sending_rate = -1.0;
    if(this->restore_policy == 1) // 1 - Slow start - increase * 2 until max value
    {
        next_real_update_sending_rate = this->next_sending_rate_for_restore;
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
        next_real_update_sending_rate = model_sending_rate;
    }
    
    this->time_counter = this->time_counter + 1;
    return next_real_update_sending_rate;
}
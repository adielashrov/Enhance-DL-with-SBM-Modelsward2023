 #include "YieldThroughputBThread.h"


YieldThroughputBThread::YieldThroughputBThread() : BThread("YieldThroughputBThread")
{
}

YieldThroughputBThread::~YieldThroughputBThread()
{
}

void YieldThroughputBThread::entryPoint()
{
    printf("Enter YieldThroughputBThread...\n" );
    
    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;

    int id = 1;
    int counter = 0;
    int exp = 1;
    bool yield_enabled = false;
    while(true)
    {
        // First - listen to the update from the model
        Event updateSendingRate(2, id); // 2 signals updateSendingRate
        requested.clear();
        watched.clear();
        blocked.clear();
        watched.append(updateSendingRate);
        // printf("YieldThroughputBThread: bSync(none, updateSendingRate, none)...\n");
        bSync(requested, watched, blocked, "YieldThroughputBThread");
        counter = counter + 1;
        Event lastEvent = this->lastEvent();
        // printf("YieldThroughputBThread: lastEvent.updateSendingRate, id: %d, type: %d, sending_rate: %f\n", lastEvent.id(), lastEvent.type(), lastEvent.nextSendingRate());            
        // printf("YieldThroughputBThread: counter: %d\n", counter);
        int last_event_id = lastEvent.id();
        double sending_rate = lastEvent.nextSendingRate();
        double next_real_update_sending_rate = sending_rate;      

        if(yield_enabled && counter >= 3000)
        {
            // next_real_update_sending_rate = 1500000.0;
            // next_real_update_sending_rate = sending_rate / 10;
            next_real_update_sending_rate = sending_rate / pow(2,exp);
            if (((counter % 500) == 0) && exp <= 8)
            {
                exp = exp + 1;
            }
            // next_real_update_sending_rate = 150000000.0;
        }

        // Second - request the actual sending rate from the actuator
        Event monitorIntervalEvent(0, id);
        Event queryNextSendingRateEvent(1, id);
        Event updateSendingRateReal(3, last_event_id, NULL, next_real_update_sending_rate); // 3 signals updateSendingRateReal
        
        requested.clear();
        watched.clear();
        blocked.clear();
        
        requested.append(updateSendingRateReal);
        blocked.append(monitorIntervalEvent);
        blocked.append(queryNextSendingRateEvent);
        this->statisticsFileHandler->LogSendingRate(next_real_update_sending_rate, 3); // 3 signals updateSendingRateReal
        // printf("YieldThroughputBThread: bSync(updateSendingRateReal, none, {monitorIntervalEvent, queryNextSendingRateEvent})...\n");
        bSync(requested, watched, blocked, "YieldThroughputBThread");
        Event lastEvent_2 = this->lastEvent();
        // printf("YieldThroughputBThread: lastEvent.updateSendingRateReal, id: %d, type: %d, sending_rate: %f\n", lastEvent_2.id(), lastEvent_2.type(), lastEvent_2.nextSendingRate());
    }

    done();
    printf("Leave YieldThroughputBThread...\n" );
}


void YieldThroughputBThread::setStatisticsFileHandler(StatisticsFileHandler* statisticsFileHandler)
{
    this->statisticsFileHandler = statisticsFileHandler;
}
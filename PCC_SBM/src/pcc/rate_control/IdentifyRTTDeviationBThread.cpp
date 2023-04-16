#include "IdentifyRTTDeviationBThread.h"


IdentifyRTTDeviationBThread::IdentifyRTTDeviationBThread() : BThread("IdentifyRTTDeviationBThread")
{
    this->yield_policy = 1;
    this->initial_sending_rate_for_yield = -1.0;
    this->time_counter = -1;
    this->division_exp = 0;
    this->lambda = 0.001;
}

IdentifyRTTDeviationBThread::~IdentifyRTTDeviationBThread()
{
}

void IdentifyRTTDeviationBThread::entryPoint()
{
    printf("Enter IdentifyRTTDeviationBThread...\n" );
    
    int id = 1;
    int number_of_changes_in_deviation = 0;
    int number_of_sending_rate_reductions = 0;
    bool enterYieldMode = false;
    bool yield_on_switch = true;
    float rtt_deviation_1 = -1.0;
    float rtt_deviation_2 = -1.0;

    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;

    while(true)
    {
        Event queryNextSendingRateEvent(1, id);
        Event enterYieldEvent(5, id); // 5 signals yield bandwidth event
        requested.clear();
        watched.clear();
        blocked.clear();
        watched.append(queryNextSendingRateEvent);
        watched.append(enterYieldEvent);
        // printf("IdentifyRTTDeviationBThread: bSync(none, { queryNextSendingRateEvent , enterYieldEvent}, none)...\n"); // Stopped here
        bSync(requested, watched, blocked, "IdentifyRTTDeviationBThread");
        Event lastEvent = this->lastEvent();

        if(lastEvent.type() == 1) // last event is queryNextSendingRateEvent
        {
            // printf("IdentifyRTTDeviationBThread: last event is queryNextSendingRateEvent, id %d\n", lastEvent.id());
            Event updateSendingRate(2, id); // // First - listen to the update from the model event 2 - Extract to method(pattern)
            requested.clear();
            watched.clear();
            blocked.clear();
            watched.append(updateSendingRate);
            // printf("IdentifyRTTDeviationBThread: bSync(none, updateSendingRate, none)...\n");
            bSync(requested, watched, blocked, "IdentifyRTTDeviationBThread");
            Event lastEvent_2 = this->lastEvent();
            // printf("IdentifyRTTDeviationBThread: lastEvent.updateSendingRate, iteration: (%d), id: (%d), type: (%d), sending_rate: (%f)\n", id, lastEvent_2.id(), lastEvent_2.type(), lastEvent_2.nextSendingRate());

            int last_event_id = lastEvent_2.id();
            double sending_rate = lastEvent_2.nextSendingRate();
            double next_real_update_sending_rate = sending_rate; 

            if(yield_on_switch) // On/Off switch for yield behavior
            {
                if(enterYieldMode)
                {
                    if(this->initial_sending_rate_for_yield == -1.0)
                    {
                        setInitialSendingRateForYield(sending_rate);
                    }
                    next_real_update_sending_rate = getYieldSendingRate();
                }
            }

            // Second - request the actual sending rate from the actuator
            Event monitorIntervalEvent_2(0, id);
            Event queryNextSendingRateEvent_2(1, id); // TODO: perhaps block 2 as well?
            Event updateSendingRateIdentifyThread(3, last_event_id, NULL, next_real_update_sending_rate); // 3 signals updateSendingRateReal
            Event updateSendingRateRestoreThread(4, id, NULL, sending_rate); // 4 signals updateSendingRateRestoreThread

            requested.clear();
            watched.clear();
            blocked.clear();
            
            requested.append(updateSendingRateIdentifyThread);
            watched.append(updateSendingRateRestoreThread);
            blocked.append(monitorIntervalEvent_2);
            blocked.append(queryNextSendingRateEvent_2);

            // printf("IdentifyRTTDeviationBThread: bSync(updateSendingRateIdentifyThread, updateSendingRateRestoreThread, {monitorIntervalEvent, queryNextSendingRateEvent}), nextSendingRate: (%f)\n", updateSendingRateIdentifyThread.nextSendingRate());
            bSync(requested, watched, blocked, "IdentifyRTTDeviationBThread");
            Event lastEvent_3 = this->lastEvent();
            // printf("IdentifyRTTDeviationBThread: lastEvent.updateSendingRate(Identify/Restore), iteration: (%d), id: %d, type: %d, sending_rate: %f\n", id, lastEvent_3.id(), lastEvent_3.type(), lastEvent_3.nextSendingRate());
        }
        else if(lastEvent.type() == 5) // last event is enterYieldEvent
        {
            // printf("****IdentifyRTTDeviationBThread: last event is enterYieldEvent! - iteration: (%d)****\n", id);
            enterYieldMode = true;            
        }

        ++id;
        // usleep(1000);
    }

    done();
    printf("Leave IdentifyRTTDeviationBThread...\n" );
}

bool IdentifyRTTDeviationBThread::checkIncreaseInRttDev(float rtt_deviation_1, float rtt_deviation_2)
{
    bool retValue = false;
    if(rtt_deviation_1 > 0 &&  rtt_deviation_2 > 0)
    {
        if(rtt_deviation_1 < rtt_deviation_2)
        {
            retValue = true;
        }
    }
    return retValue;
    
}

void IdentifyRTTDeviationBThread::setInitialSendingRateForYield(double sending_rate)
{
    this->initial_sending_rate_for_yield = sending_rate;
    this->time_counter = 0;
}

double IdentifyRTTDeviationBThread::getYieldSendingRate()
{
    double next_real_update_sending_rate = -1.0;
    if(this->yield_policy == 1) //1 - Hard coded values policy
    {
        next_real_update_sending_rate = 15000000.0;
    }
    else if(this->yield_policy == 2) // 2 - Stairway policy - divide rate in fraction^K
    {
        double reduce_rate = pow(0.9, division_exp);
        next_real_update_sending_rate = (this->initial_sending_rate_for_yield * reduce_rate);

        if (((this->time_counter % 2000) == 0) && reduce_rate >= 0.00001)
        {
            ++this->division_exp;
        }
    }
    else if(this->yield_policy == 3) // 3 - Exponential decay policy
    {
        double mul = ((-1) * this->lambda) * this->time_counter;
        double exp_value = exp(mul);
        next_real_update_sending_rate = this->initial_sending_rate_for_yield * exp_value;
    }
    this->time_counter = this->time_counter + 1;
    return next_real_update_sending_rate;
}
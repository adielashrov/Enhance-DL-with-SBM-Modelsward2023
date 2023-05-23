#include "ReduceThroughputBThread.h"


ReduceThroughputBThread::ReduceThroughputBThread() : BThread("ReduceThroughputBThread")
{
    this->yield_policy = 1;
    this->initial_sending_rate_for_yield = -1.0;
    this->time_counter = -1;
    this->division_exp = 0;
    this->lambda = 0.001;
}

ReduceThroughputBThread::~ReduceThroughputBThread()
{
}

void ReduceThroughputBThread::entryPoint()
{
    printf("Enter ReduceThroughputBThread...\n" );
    
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
        bSync(requested, watched, blocked, "ReduceThroughputBThread");
        Event lastEvent = this->lastEvent();

        if(lastEvent.type() == 1) // last event is queryNextSendingRateEvent
        {
            Event updateSendingRate(2, id); // // First - listen to the update from the model
            requested.clear();
            watched.clear();
            blocked.clear();
            watched.append(updateSendingRate);
            bSync(requested, watched, blocked, "ReduceThroughputBThread");
            Event lastEvent_2 = this->lastEvent();

            int last_event_id = lastEvent_2.id();
            double sending_rate = lastEvent_2.nextSendingRate();
            double updated_sending_rate = sending_rate; 

            if(yield_on_switch) // On/Off switch for yield behavior
            {
                if(enterYieldMode)
                {
                    if(this->initial_sending_rate_for_yield == -1.0)
                    {
                        setInitialSendingRateForReduce(sending_rate);
                    }
                    updated_sending_rate = getReduceSendingRate();
                }
            }

            // Second - request the actual sending rate from the actuator
            Event monitorIntervalEvent_2(0, id);
            Event queryNextSendingRateEvent_2(1, id);
            Event updateSendingRateReduceEvent(3, last_event_id, NULL, updated_sending_rate); // 3 signals updateSendingRateReduceEvent
            Event updateSendingRateRestoreEvent(4, id, NULL, sending_rate); // 4 signals updateSendingRateRestoreEvent

            requested.clear();
            watched.clear();
            blocked.clear();
            
            requested.append(updateSendingRateReduceEvent);
            watched.append(updateSendingRateRestoreEvent);
            blocked.append(monitorIntervalEvent_2);
            blocked.append(queryNextSendingRateEvent_2);

            bSync(requested, watched, blocked, "ReduceThroughputBThread");
            Event lastEvent_3 = this->lastEvent();
        }
        else if(lastEvent.type() == 5) // last event is enterYieldEvent
        {
            enterYieldMode = true;            
        }

        ++id;
    }

    done();
    printf("Leave ReduceThroughputBThread...\n" );
}

bool ReduceThroughputBThread::checkIncreaseInRttDev(float rtt_deviation_1, float rtt_deviation_2)
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

void ReduceThroughputBThread::setInitialSendingRateForReduce(double sending_rate)
{
    this->initial_sending_rate_for_yield = sending_rate;
    this->time_counter = 0;
}

double ReduceThroughputBThread::getReduceSendingRate()
{
    double updated_sending_rate = -1.0;
    if(this->yield_policy == 1) //1 - Hard coded values policy
    {
        updated_sending_rate = 10000000.0;
    }
    else if(this->yield_policy == 2) // 2 - Stairway policy - divide rate in fraction^K
    {
        double reduce_rate = pow(0.9, division_exp);
        updated_sending_rate = (this->initial_sending_rate_for_yield * reduce_rate);

        if (((this->time_counter % 2000) == 0) && reduce_rate >= 0.00001)
        {
            ++this->division_exp;
        }
    }
    else if(this->yield_policy == 3) // 3 - Exponential decay policy
    {
        double mul = ((-1) * this->lambda) * this->time_counter;
        double exp_value = exp(mul);
        updated_sending_rate = this->initial_sending_rate_for_yield * exp_value;
    }
    this->time_counter = this->time_counter + 1;
    return updated_sending_rate;
}
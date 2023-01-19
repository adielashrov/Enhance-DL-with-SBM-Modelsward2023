 #include "MonitorNetworkStateBThread.h"


MonitorNetworkStateBThread::MonitorNetworkStateBThread() : BThread("MonitorNetworkStateBThread")
{
}

MonitorNetworkStateBThread::~MonitorNetworkStateBThread()
{
}

void MonitorNetworkStateBThread::entryPoint()
{
     printf("Enter MonitorNetworkStateBThread...\n" );
    
    int id = 1;
    int number_of_changes_in_deviation = 0;
    bool enterYieldMode = false;
    bool monitorHotSwitch = true;
    int numOfIterInYieldMode = 0;
    int numOfIterInRestoreMode = 0;
    int numOfDecraseInUtilityValue = 0;
    float rtt_deviation_1 = -1.0;
    float rtt_deviation_2 = -1.0;
    float current_utility = -1.0;

    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;

    while(true)
    {
        Event monitorIntervalEvent(0, id);
        requested.clear();
        watched.clear();
        blocked.clear();
        watched.append(monitorIntervalEvent); 
        // printf("MonitorNetworkStateBThread: bSync(none, monitorIntervalEvent, none)...\n");
        bSync(requested, watched, blocked, "MonitorNetworkStateBThread"); // Stopped here
        Event lastEvent = this->lastEvent();
        // printf("MonitorNetworkStateBThread: lastEvent.monitorIntervalEvent, id: %d, type: %d\n", lastEvent.id(), lastEvent.type());
        
        float rtt_dev = UtilityCalculator::ComputeRttDeviation(lastEvent.monitorInterval());
        rtt_deviation_1 = rtt_deviation_2;
        rtt_deviation_2 = rtt_dev;
        current_utility = lastEvent.monitorInterval()->GetObsUtility();

        // printf("MonitorNetworkStateBThread: last event is monitorIntervalEvent, id: %d, type: %d, rtt_dev_1: %f, rtt_dev_2: %f\n", lastEvent.id(), lastEvent.type(), rtt_deviation_1, rtt_deviation_2);

        if(monitorHotSwitch)
        {
            if(!enterYieldMode)
            {
                if(numOfIterInRestoreMode >= 2000)
                {
                    // if(numOfIterInRestoreMode == 5000)
                    // {
                    // printf("Start checking RTT deviation only after 5000 iterations\n");
                    // }
                    /*
                    if(checkIncreaseInRttDev(rtt_deviation_1, rtt_deviation_2))
                    {
                        ++number_of_changes_in_deviation;
                    }
                    else
                    {
                        number_of_changes_in_deviation = 0;
                    }
                    */

                    if(numOfIterInRestoreMode % 5000 == 0)
                    {
                        numOfDecraseInUtilityValue = 0;
                        printf("numOfDecraseInUtilityValue = 0\n");
                    }
                    else
                    {
                        if(current_utility < -100000.0)
                        {   
                            ++numOfDecraseInUtilityValue;
                            printf("current_utility: (%f), numOfDecraseInUtilityValue: (%d)\n", current_utility, numOfDecraseInUtilityValue);
                        }
                    }
                    
                    if(numOfDecraseInUtilityValue >= 3)
                    {
                        printf("***MonitorNetworkStateBThread: enterYieldMode...***\n");
                        enterYieldMode = true;                        
                        numOfIterInYieldMode = 0;
                        numOfIterInRestoreMode = 0;
                        numOfDecraseInUtilityValue = 0;
                        Event monitorIntervalEvent_2(0, id);
                        Event getNextSendingRateEvent(1, id); // 2 signals updateSendingRate
                        Event yieldBandwidthEvent(5, id); // 5 signals yield bandwidth event
                        requested.clear();
                        watched.clear();
                        blocked.clear();
                        requested.append(yieldBandwidthEvent);
                        blocked.append(monitorIntervalEvent_2);
                        blocked.append(getNextSendingRateEvent);
                        // printf("MonitorNetworkStateBThread: bSync(yieldBandwidthEvent, none, {monitorIntervalEvent, getNextSendingRateEvent})...\n");
                        bSync(requested, watched, blocked, "MonitorNetworkStateBThread");
                        // printf("MonitorNetworkStateBThread: last event is yieldBandwidthEvent\n");
                    }
                }
                // else
                // {
                    ++numOfIterInRestoreMode;
                // }

            }
            else
            {
                if(numOfIterInYieldMode >= 3000)
                {
                    enterYieldMode = false;                        
                    numOfIterInYieldMode = 0;
                    numOfIterInRestoreMode = 0;
                    printf("***MonitorNetworkStateBThread: Restoring throughput...***\n");
                    Event restoreThroughputEvent(6, id); // 6 signals yield bandwidth event 
                    requested.clear();
                    watched.clear();
                    blocked.clear(); // Should we block anything?
                    requested.append(restoreThroughputEvent);
                    // printf("MonitorNetworkStateBThread: bSync(restoreThroughputEvent, none, none)...\n"); 
                    bSync(requested, watched, blocked, "MonitorNetworkStateBThread");
                    // printf("MonitorNetworkStateBThread: last event is restoreThroughputEvent...\n");
                }
                // else
                // {
                    ++numOfIterInYieldMode;
                // }
            }
        }

        ++id;
        // usleep(100000);
    }
    
    done();
    printf("Leave MonitorNetworkStateBThread...\n" );
}

bool MonitorNetworkStateBThread::checkIncreaseInRttDev(float rtt_deviation_1, float rtt_deviation_2)
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
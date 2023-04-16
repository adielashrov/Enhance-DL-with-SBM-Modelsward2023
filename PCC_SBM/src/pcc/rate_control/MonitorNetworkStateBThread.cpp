 #include "MonitorNetworkStateBThread.h"


MonitorNetworkStateBThread::MonitorNetworkStateBThread() : BThread("MonitorNetworkStateBThread")
{
}

MonitorNetworkStateBThread::~MonitorNetworkStateBThread()
{
}

void MonitorNetworkStateBThread::entryPoint()
{
    printf("Enter MonitorNetworkStateScenario.\n" );
    
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
        bSync(requested, watched, blocked, "MonitorNetworkStateBThread");
        Event lastEvent = this->lastEvent();
        
        float rtt_dev = UtilityCalculator::ComputeRttDeviation(lastEvent.monitorInterval());
        rtt_deviation_1 = rtt_deviation_2;
        rtt_deviation_2 = rtt_dev;
        current_utility = lastEvent.monitorInterval()->GetObsUtility();
        // printf("current_utility: %4.2f\n", current_utility);

        if(monitorHotSwitch)
        {
            if(!enterYieldMode)
            {
                if(numOfIterInRestoreMode >= 1000)
                {
                    if(numOfIterInRestoreMode % 5000 == 0)
                    {
                        numOfDecraseInUtilityValue = 0;
                    }
                    else
                    {
                        if(current_utility < 0.0)
                        {   
                            ++numOfDecraseInUtilityValue;
                        }
                    }
                    
                    if(numOfDecraseInUtilityValue >= 3)
                    {
                        printf("*****MonitorNetworkStateScenario: Yielding throughput...*****\n");
                        enterYieldMode = true;                        
                        numOfIterInYieldMode = 0;
                        numOfIterInRestoreMode = 0;
                        numOfDecraseInUtilityValue = 0;
                        Event monitorIntervalEvent_2(0, id);
                        Event queryNextSendingRateEvent(1, id); // 2 signals updateSendingRate
                        Event enterYieldEvent(5, id); // 5 signals enterYield event
                        requested.clear();
                        watched.clear();
                        blocked.clear();
                        requested.append(enterYieldEvent);
                        blocked.append(monitorIntervalEvent_2);
                        blocked.append(queryNextSendingRateEvent);
                        bSync(requested, watched, blocked, "MonitorNetworkStateBThread");
                    }
                }
                ++numOfIterInRestoreMode;
            }
            else
            {
                if(numOfIterInYieldMode >= 1000)
                {
                    enterYieldMode = false;                        
                    numOfIterInYieldMode = 0;
                    numOfIterInRestoreMode = 0;
                    printf("*****MonitorNetworkStateScenario: Restoring throughput...*****\n");
                    Event restoreThroughputEvent(6, id); // 6 signals restore bandwidth event 
                    requested.clear();
                    watched.clear();
                    blocked.clear();
                    requested.append(restoreThroughputEvent);
                    bSync(requested, watched, blocked, "MonitorNetworkStateBThread");
                }
                ++numOfIterInYieldMode;
            }
        }

        ++id;
    }
    
    done();
    printf("Leave MonitorNetworkStateScenario...\n" );
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
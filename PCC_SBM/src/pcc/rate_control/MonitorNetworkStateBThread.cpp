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
    bool enterYieldMode = false;
    bool monitorHotSwitch = true;
    int numOfIterInYieldMode = 0;
    int numOfIterInRestoreMode = 0;
    int numOfDecraseInUtilityValue = 0;
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
        current_utility = lastEvent.monitorInterval()->GetObsUtility();

        if(monitorHotSwitch)
        {
            if(!enterYieldMode)
            {
                if(numOfIterInRestoreMode >= 2000)
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
                        Event queryNextSendingRateEvent(1, id); // 1 signals queryNextSendingRateEvent
                        Event enterYieldEvent(5, id); // 5 signals enterYieldEvent
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
                if(numOfIterInYieldMode >= 2000)
                {
                    enterYieldMode = false;                        
                    numOfIterInYieldMode = 0;
                    numOfIterInRestoreMode = 0;
                    printf("*****MonitorNetworkStateScenario: Restoring throughput...*****\n");
                    Event enterRestoreEvent(6, id); // 6 signals enterRestoreEvent
                    requested.clear();
                    watched.clear();
                    blocked.clear();
                    requested.append(enterRestoreEvent);
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
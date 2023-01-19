 #include "InterleaveBthread.h"


InterleaveBThread::InterleaveBThread() : BThread("InterleaveBThread")
{
}

InterleaveBThread::~InterleaveBThread()
{
}


void InterleaveBThread::entryPoint()
{
    printf("Enter to InterleaveBthread...\n" );
    
    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;

    int id = 1;
    int look_back_limit = 3;
    int look_back_counter  = 0;
    printf("InterleaveBthread start\n");
    while(true)
    {
        Event monitorEvent(0, id);
        Event getNextSendingRateEvent(1, id);
        while(look_back_counter < look_back_limit)
        {
            watched.clear();
            watched.append(monitorEvent);
            blocked.clear();
            blocked.append(getNextSendingRateEvent);
            // printf("InterleaveBthread - requesting monitorEvent; blocking getNextSendingRateEvent , id:%d \n", id);
            bSync(requested, watched, blocked, "InterleaveBThread");
            ++look_back_counter;
            ++id;
        }

        look_back_counter = 0;
        watched.clear();
        watched.append(getNextSendingRateEvent);
        blocked.clear();
        blocked.append(monitorEvent);
        // printf("InterleaveBthread - requesting getNextSendingRateEvent; blocking monitorEvent , id:%d \n", id);
        bSync(requested, watched, blocked, "InterleaveBThread");

        ++id;
        // sleep(5);
    }

    done();
    printf("Leave InterleaveBThread...\n" );
}
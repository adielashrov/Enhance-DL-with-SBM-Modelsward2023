#include "SendRateActuatorBThread.h"


SendRateActuatorBThread::SendRateActuatorBThread() : BThread("SendRateActuatorBThread")
{
    // Initial sending rate according to the Aurora simulation
    this->next_sending_rate = 6000000.0000000;
    this->statisticsFileHandler = NULL;
}

SendRateActuatorBThread::~SendRateActuatorBThread()
{
}

void SendRateActuatorBThread::entryPoint()
{
    printf("Enter SendRateActuatorBThread...\n" );
    
    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;

    int id = 1;
    
    while(true)
    {
        // Event updateSendingRate(2, id); // 2 signals updateSendingRate from model
        Event updateSendingRateReduceEvent(3, id); // 3 signals updateSendingRate from ReduceThroughputBThread
        Event updateSendingRateRestoreEvent(4, id); // 4 signals updateSendingRate from RestoreThroughputBThread
        requested.clear();
        requested.clear();
        watched.clear();
        blocked.clear();
        watched.append(updateSendingRateReduceEvent);
        watched.append(updateSendingRateRestoreEvent);
        bSync(requested, watched, blocked, "SendRateActuatorBThread");
        Event lastEvent = this->lastEvent();

        if(lastEvent.nextSendingRate() != -1.0)
        {
            int event_id = lastEvent.id();
            int type = lastEvent.type();
            double sending_rate = lastEvent.nextSendingRate();
            if(this->statisticsFileHandler != NULL)
            {
                this->statisticsFileHandler->LogSendingRate(sending_rate, type); // 3 signals updateSendingRateReal
            }
            std::unique_lock<std::mutex> lck(mtx);
            _eventsToSendRate.insert({ event_id, sending_rate });
            cv.notify_one();
        }

        id = id + 1;
    }

    done();
    printf("Leave SendRateActuatorBThread...\n" );
}

double SendRateActuatorBThread::readNextSendingRate(int id)
{
    std::unique_lock<std::mutex> lck(mtx);
    while(_eventsToSendRate.find(id) == _eventsToSendRate.end()) 
    {
        cv.wait(lck);
    }
    double sending_rate = _eventsToSendRate.find(id)->second;
    return sending_rate;
}

void SendRateActuatorBThread::setStatisticsFileHandler(StatisticsFileHandler* statisticsFileHandler)
{
    this->statisticsFileHandler = statisticsFileHandler;
}
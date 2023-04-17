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
        Event updateSendingRateIdentifyThread(3, id); // 3 signals updateSendingRate from ReduceThroughputBThread
        Event updateSendingRateRestoreThread(4, id); // 4 signals updateSendingRate from RestoreThroughputBThread
        requested.clear();
        requested.clear();
        watched.clear();
        blocked.clear();
        watched.append(updateSendingRateIdentifyThread);
        watched.append(updateSendingRateRestoreThread);
        // printf("SendRateAct: bSync(none, {updateSendingRateIdentifyThread, updateSendingRateRestoreThread} , none), id: %d \n", id); // Stopeed here
        bSync(requested, watched, blocked, "SendRateActuatorBThread");
        Event lastEvent = this->lastEvent();

        if(lastEvent.nextSendingRate() != -1.0)
        {
            // printf("SendRateAct: lastEvent.updateSendingRate(Identify/Restore) id: (%d), type: (%d) actual nextSendingRate: (%f)\n\n", lastEvent.id(), lastEvent.type(), lastEvent.nextSendingRate());
            int event_id = lastEvent.id();
            int type = lastEvent.type();
            double sending_rate = lastEvent.nextSendingRate();
            if(this->statisticsFileHandler != NULL)
            {
                this->statisticsFileHandler->LogSendingRate(sending_rate, type); // 3 signals updateSendingRateReal
            }
            // printf("SendRateActuatorBThread - Going to notify controller on new sending rate %f\n", sending_rate);
            std::unique_lock<std::mutex> lck(mtx);
            _eventsToSendRate.insert({ event_id, sending_rate });
            cv.notify_one();
             // printf("SendRateActuatorBThread - after cv.notify_one()...\n");
        }

        id = id + 1;
        // usleep(1000);
    }

    done();
    printf("Leave SendRateActuatorBThread...\n" );
}

double SendRateActuatorBThread::readNextSendingRate(int id)
{
    // return 300000000.00;

    // printf("SendRateActuatorBThread::readNextSendingRate enter: %d\n", id);
    std::unique_lock<std::mutex> lck(mtx);
    while(_eventsToSendRate.find(id) == _eventsToSendRate.end()) 
    {
        // printf("SendRateActuatorBThread::readNextSendingRate - waiting for id: %d\n", id);
        cv.wait(lck);
    }
    double sending_rate = _eventsToSendRate.find(id)->second;
    // printf("SendRateActuatorBThread::readNextSendingRate - after notification sendingRate: %f \n", sending_rate);
    return sending_rate;
}

void SendRateActuatorBThread::setStatisticsFileHandler(StatisticsFileHandler* statisticsFileHandler)
{
    this->statisticsFileHandler = statisticsFileHandler;
}
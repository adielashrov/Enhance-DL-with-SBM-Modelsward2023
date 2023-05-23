#include "MonitorIntervalSensorBThread.h"


MonitorIntervalSensorBThread::MonitorIntervalSensorBThread() : BThread("MonitorIntervalSensorBThread")
{
    this->sensor_bthread_lock_ = new std::mutex();
    this->ready_ = false;
    this->mi_p_vec.clear();
    utility_factor = 1.0;
}

MonitorIntervalSensorBThread::~MonitorIntervalSensorBThread()
{
    //TODO: Iterate over intervals pointers vector and delete each pointer
    delete this->sensor_bthread_lock_;
}


void MonitorIntervalSensorBThread::entryPoint()
{
    printf("Enter to MonitorIntervalSensorBThread...\n" );
    
    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;

    int id = 1;
    bool ready = false;

    while(true)
    {
        MonitorInterval* t_mi = nullptr;
        if(!this->mi_p_vec.empty())
        {
            t_mi = this->mi_p_vec.back();
            this->mi_p_vec.pop_back();
        }
        
        if(t_mi != nullptr)
        {
            Event currentEvent(0, id, t_mi);
            requested.clear();
            requested.append(currentEvent);
            bSync(requested, watched, blocked, "SensorBthread");
            Event lastEvent = this->lastEvent();
            ++id;
        }
    }

    done();
    printf("Leave MonitorIntervalSensorBThread...\n" );
}

void MonitorIntervalSensorBThread::notifyOnMonitorEvent(const MonitorInterval& mi)
{
    MonitorInterval* miCopy = new MonitorInterval(mi);
	auto it = this->mi_p_vec.begin();
    it = this->mi_p_vec.insert(it, miCopy);
}
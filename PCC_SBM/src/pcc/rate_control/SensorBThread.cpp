 #include "SensorBThread.h"


SensorBThread::SensorBThread() : BThread("SensorBThread")
{
    this->sensor_bthread_lock_ = new std::mutex();
    this->ready_ = false;
    this->mi_p_vec.clear();
    utility_factor = 1.0;
}

SensorBThread::~SensorBThread()
{
    //TODO: Iterate over intervals pointers vector and delete each pointer
    delete this->sensor_bthread_lock_;
}


void SensorBThread::entryPoint()
{
    printf("Enter to SensorBthread...\n" );
    
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
            // printf("Sensor: bSync(monitorIntervalEvent, none, none) id: %d \n", id);
            bSync(requested, watched, blocked, "SensorBthread");
            Event lastEvent = this->lastEvent();
            
            if(lastEvent.monitorInterval() != NULL)
            {
                // printf("Sensor: lastEvent.monitorInterval, id: %d , utility %f\n", lastEvent.id(), lastEvent.monitorInterval()->GetUtility());
            }
            else
            {
                // printf("\nSensorBthread - after bSync, id: %d, monitorInterval is NULL \n", lastEvent.id());   
            }
            
            ++id;
            // usleep(1000);
        }
    }

    done();
    printf("Leave SensorBthread...\n" );
}

void SensorBThread::notifyOnMonitorEvent(const MonitorInterval& mi)
{
    MonitorInterval* miCopy = new MonitorInterval(mi);
	auto it = this->mi_p_vec.begin();
    it = this->mi_p_vec.insert(it, miCopy);
}
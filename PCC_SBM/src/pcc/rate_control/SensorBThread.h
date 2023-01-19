#ifndef __sensorBThread_h__
#define __sensorBThread_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include <cstdlib>
#include <unistd.h>
#include <mutex>
#include <vector>
#include <math.h>

class SensorBThread : public BThread
{
    public:
        SensorBThread();
        ~SensorBThread();
        void entryPoint();
        void notifyOnMonitorEvent(const MonitorInterval& mi);

    private:
        std::mutex* sensor_bthread_lock_;
        bool ready_;
        std::vector<MonitorInterval*> mi_p_vec;
        double utility_factor;
};

#endif // __sensorBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
#ifndef __monitorIntervalSensorBThread_h__
#define __monitorIntervalSensorBThread_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include <cstdlib>
#include <unistd.h>
#include <mutex>
#include <vector>
#include <math.h>

class MonitorIntervalSensorBThread : public BThread
{
    public:
        MonitorIntervalSensorBThread();
        ~MonitorIntervalSensorBThread();
        void entryPoint();
        void notifyOnMonitorEvent(const MonitorInterval& mi);

    private:
        std::mutex* sensor_bthread_lock_;
        bool ready_;
        std::vector<MonitorInterval*> mi_p_vec;
        double utility_factor;
};

#endif // __monitorIntervalSensorBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
#ifndef __monitorNetworkStateBThread_h__
#define __monitorNetworkStateBThread_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include "UtilityCalculator.h"
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <cmath>


class MonitorNetworkStateBThread : public BThread
{
    public:
        MonitorNetworkStateBThread();
        ~MonitorNetworkStateBThread();
        void entryPoint();
};

#endif // __monitorNetworkStateBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//

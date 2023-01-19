#ifndef __utilityCalculator_h__
#define __utilityCalculator_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include "StatisticsFileHandler.h"
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <cmath>

class UtilityCalculator
{
    public:
        static float ComputeAvgRTT(MonitorInterval* interval);
        static float ComputeRttDeviation(MonitorInterval* interval);
};

#endif // __utilityCalculator_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
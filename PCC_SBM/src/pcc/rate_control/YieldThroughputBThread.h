#ifndef __yieldThroughputBThread_h__
#define __yieldThroughputBThread_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include "StatisticsFileHandler.h"
#include <cstdlib>
#include <unistd.h>
#include <atomic>
#include <mutex>
#include <vector>
#include <cmath>

class YieldThroughputBThread : public BThread
{
    public:
        YieldThroughputBThread();
        ~YieldThroughputBThread();
        void entryPoint();
        void setStatisticsFileHandler(StatisticsFileHandler* statisticsFileHandler);

    private:
        StatisticsFileHandler* statisticsFileHandler;
};

#endif // __sendRateBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
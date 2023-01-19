#ifndef __sendRateActuatorBThread_h__
#define __sendRateActuatorBThread_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include "StatisticsFileHandler.h"
#include <cstdlib>
#include <unistd.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>

class SendRateActuatorBThread : public BThread
{
    public:
        SendRateActuatorBThread();
        ~SendRateActuatorBThread();
        void entryPoint();
        double readNextSendingRate(int id);
        void setStatisticsFileHandler(StatisticsFileHandler* statisticsFileHandler);

    private:
        double next_sending_rate;
        std::map<int, double> _eventsToSendRate;
        std::mutex mtx;
        std::condition_variable cv;
        StatisticsFileHandler* statisticsFileHandler;
};

#endif // __sendRateActuatorBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
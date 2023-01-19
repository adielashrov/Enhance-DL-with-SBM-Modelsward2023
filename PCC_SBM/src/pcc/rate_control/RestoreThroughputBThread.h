#ifndef __restoreThroughputBThread_h__
#define __restoreThroughputBThread_h

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include <unistd.h>

class RestoreThroughputBThread : public BThread
{
    public:
        RestoreThroughputBThread();
        ~RestoreThroughputBThread();
        void entryPoint();

    private:
        int restore_policy;
        double next_sending_rate_for_restore;
        double sending_rate_upper_limit;
        int time_counter;
        double getRestoreSendingRate(double model_sending_rate);
        void setInitialSendingRateForRestore(double sending_rate);
};

#endif // __sendRateBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
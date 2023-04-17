#ifndef __identifyRTTDeviationBThread_h__
#define __identifyRTTDeviationBThread_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include "UtilityCalculator.h"
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <cmath>


class ReduceThroughputBThread : public BThread
{
    public:
        ReduceThroughputBThread();
        ~ReduceThroughputBThread();
        void entryPoint();
        bool checkIncreaseInRttDev(float rtt_deviation_1, float rtt_deviation_2);

    private:
        int yield_policy;
        double initial_sending_rate_for_yield;
        double lambda;
        int time_counter;
        double division_exp;
        double getYieldSendingRate();
        void setInitialSendingRateForYield(double sending_rate);

};

#endif // __identifyRTTDeviationBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
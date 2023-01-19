#ifndef __StatisticsFileHandler_h__
#define __StatisticsFileHandler_h__

#include "Event.h"
#include "BThread.h"
#include <unistd.h>
#include <mutex>
#include <queue>
#include <vector>
#include <python3.7/Python.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../../core/options.h"

#define BP_COUNTER_LIMIT 1000001

class StatisticsFileHandler
{
    public:
        StatisticsFileHandler(); // TODO: open statistics file name
        ~StatisticsFileHandler();
        void LogGiveSample(int bytes_sent,
                  int bytes_acked,
                  int bytes_lost,
                  double send_start_time_sec,
                  double send_end_time_sec,
                  double recv_start_time_sec,
                  double recv_end_time_sec,
                  double first_ack_latency_sec,
                  double last_ack_latency_sec,
                  int packet_size,
                  double utility,
                  float rtt_dev);
        void LogSendingRate(QuicBandwidth sending_rate, int event_type = 2);

    private:
        std::mutex m;
        int counter;
        std::ofstream log_file_samples;
};


#endif // __StatisticsFileHandler_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
#ifndef __ODRL_BThread_h__
#define __ODRL_BThread__

#include "Event.h"
#include "BThread.h"
#include "StatisticsFileHandler.h"
#include "UtilityCalculator.h"
#include <unistd.h>
#include <mutex>
#include <queue>
#include <vector>
#include <python3.7/Python.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../../core/options.h"

#ifndef USEC_PER_SEC
    #define USEC_PER_SEC 1000000
#endif

#define BP_COUNTER_LIMIT 1000001

class ODRL_BThread : public BThread
{
    public:
        ODRL_BThread();
        ~ODRL_BThread();
        void entryPoint();
        void setStatisticsFileHandler(StatisticsFileHandler* statisticsFileHandler);

    private:
        bool python_initialized_ = false;
        int id;
        bool has_time_offset;
        uint64_t time_offset_usec;

        PyObject* module;
        PyObject* give_sample_func;
        PyObject* get_rate_func;
        PyObject* reset_func;
        
        std::ofstream log_file_samples;
        int counter;
        QuicBandwidth current_rate;
        void InitializePython();
        void GiveSample(int bytes_sent,
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
                  MonitorInterval* mi = NULL);
        void MonitorIntervalFinished(MonitorInterval* mi);
        QuicBandwidth GetNextSendingRate();

        void testModel();
        bool getNextLine(std::string& line);
        bool isMonitorEvent(std::vector<double> vec);
        void testMonitorInterval(std::vector<double> vec);
        std::vector<double> parseLine(std::string& line);
        std::ifstream simulation_events_file;
        StatisticsFileHandler* statisticsFileHandler;
};


#endif // __ODRL_BThread__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
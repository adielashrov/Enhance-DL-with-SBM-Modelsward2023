#ifndef __testModelBThread_h__
#define __testModelBThread_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include <cstdlib>
#include <unistd.h>
#include <mutex>
#include <vector>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

class TestModelBThread : public BThread
{
    public:
        TestModelBThread();
        ~TestModelBThread();
        void entryPoint();

    private:
        bool getNextLine(std::string& line);
        bool isMonitorEvent(std::vector<double> vec);
        MonitorInterval* createMonitorInterval(std::vector<double> vec);
        std::vector<double> parseLine(std::string& line);
        std::ifstream simulation_events_file;
};

#endif // __testModelBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
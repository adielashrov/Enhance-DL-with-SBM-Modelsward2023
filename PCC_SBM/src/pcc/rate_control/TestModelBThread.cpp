#include "TestModelBThread.h"


TestModelBThread::TestModelBThread() : simulation_events_file("monitor_intervals_aurora_dl.txt", std::ios::in)
{
   printf("Opened file:... monitor_intervals_aurora_dl");
}

TestModelBThread::~TestModelBThread()
{
    simulation_events_file.close();
}

void TestModelBThread::entryPoint()
{
    printf("Enter to TestModelBThread...\n" );
    
    Vector<Event> requested;
    Vector<Event> watched;
    Vector<Event> blocked;
    int id = 1;

    std::string currLine = "";

    while(getNextLine(currLine))
    {   
        
        if (currLine.find("sending_rate") != std::string::npos) {
                continue;
        }
        std::vector<double> vec = parseLine(currLine);
        
        if(isMonitorEvent(vec))
        {
            printf("Next event is monitor interval event...");
            MonitorInterval* miCopy = createMonitorInterval(vec);
        }
        else
        {
            printf("Next event is sending rate event...");
        }

        Event currentEvent(0, id);
        watched.clear();
        watched.append(currentEvent);
        bSync(requested, watched, blocked, "TestModelBThread");
        Event lastEvent = this->lastEvent();
        if(lastEvent.monitorInterval() != NULL)
        {
            printf("TestModelBThread after bSync...\n");
        }
        else
        {
            printf("TestModelBThread after bSync monitorInterval is NULL \n");   
        }
        
        
        id = id + 1;
        sleep(1);  
    }

    done();
    printf("Leave TestModelBThread...\n" );
}

bool TestModelBThread::getNextLine(std::string& line) {

    if (simulation_events_file.is_open()) 
    {
        if(getline(simulation_events_file, line))
        {
            return true;
        }
    }

    return false;
}

std::vector<double> TestModelBThread::parseLine(std::string& line) {
    std::stringstream input_stringstream(line);
    std::vector<double> values;
    std::string word;
    
    while (getline(input_stringstream, word, '\t')) {
        if(!word.empty()) {
            double word_double = std::stod(word);
            values.push_back(word_double);
            
        }
    }

    return values;
}

bool TestModelBThread::isMonitorEvent(std::vector<double> vec) {

    if(vec.size() > 2) 
    {
        if(vec[1] == 1.0) // 1.0 stands for monitor interval event
        {
            return true;
        }
    }
    return false; // 2.0 stands for next sending rate event
}

MonitorInterval* TestModelBThread::createMonitorInterval(std::vector<double> vec) {

    return NULL;

}

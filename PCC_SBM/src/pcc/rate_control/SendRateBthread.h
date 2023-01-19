#ifndef __sendRateBThread_h__
#define __sendRateBThread_h__

#include "../monitor_interval/pcc_mi.h"
#include "Event.h"
#include "BThread.h"
#include <cstdlib>
#include <unistd.h>
#include <atomic>
#include <mutex>
#include <vector>

class SendRateBthread : public BThread
{
    public:
        SendRateBthread();
        ~SendRateBthread();
        void entryPoint();
        void notifyOnGetNextSendingRate(int id);

    private:
        double next_sending_rate;
        std::atomic<int> num_of_get_next_sending_rate;
        std::mutex vector_events_lock;
        std::vector<int> events_ids;   
};

#endif // __sendRateBThread_h__

//
// Local Variables:
// compile-command: "make -C .. "
// tags-file-name: "../TAGS"
// c-basic-offset: 4
// End:
//
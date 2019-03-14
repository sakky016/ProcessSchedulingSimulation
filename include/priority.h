#ifndef _PRIORITY_SCHEDULING_H_
#define _PRIORITY_SCHEDULING_H_

#include "process_scheduler.h"
#include<set>
#include<string>


using namespace std;

class PriorityScheduling : public ProcessScheduler
{
private:

public:
    PriorityScheduling(string name);
    ~PriorityScheduling();
    void setSchedulerName(string name);
    void ProcessJobs();
};

#endif

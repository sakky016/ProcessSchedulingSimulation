#ifndef _PRIORITY_AGEING_H_
#define _PRIORITY_AGEING_H_

#include "process_scheduler.h"
#include<set>
#include<string>


using namespace std;

class PriorityAgeingScheduling : public ProcessScheduler
{
private:

public:
    PriorityAgeingScheduling(string name);
    ~PriorityAgeingScheduling();
    void setSchedulerName(string name);
    void ProcessJobs();
};

#endif

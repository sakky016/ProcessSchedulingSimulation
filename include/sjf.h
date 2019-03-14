#ifndef _SJF_H_
#define _SJF_H_

#include "process_scheduler.h"
#include<set>
#include<string>


using namespace std;

class ShortestJobFirst : public ProcessScheduler
{
private:

public:
    ShortestJobFirst(string name);
    ~ShortestJobFirst();
    void setSchedulerName(string name);
    void ProcessJobs();
};

#endif

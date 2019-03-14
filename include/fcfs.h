#ifndef _FCFS_H_
#define _FCFS_H_

#include<string>
#include "process_scheduler.h"

using namespace std;

class FirstComeFirstServed: public ProcessScheduler
{
private:

public:
    FirstComeFirstServed(string name);
    ~FirstComeFirstServed();
    void setSchedulerName(string name);
    void ProcessJobs();
};

#endif

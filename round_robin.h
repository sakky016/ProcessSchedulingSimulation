#ifndef _ROUND_ROBIN_H_
#define _ROUND_ROBIN_H_

#include "process_scheduler.h"
#include<set>
#include<string>

using namespace std;

class RoundRobin : public ProcessScheduler
{
private:
    long long               m_timeQuantum;

public:
    RoundRobin(string name, long long timeQuantum);
    ~RoundRobin();
    void setSchedulerName(string name);
    void ProcessJobs();
    virtual bool DoJob(Job *job);
    long long getTimeQuantum() { return m_timeQuantum; }
};

#endif

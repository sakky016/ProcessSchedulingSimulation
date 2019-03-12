#ifndef _PROCESS_SCHEDULER_H_
#define _PROCESS_SCHEDULER_H_

#include<list>
#include<mutex>
#include<string>
#include<unordered_map>
#include "job.h"

using namespace std;

//-------------------------------------------------------------------------------------------------
// Scheduling is the method by which work specified by some means is assigned to resources that 
// complete the work. A scheduler is what carries out the scheduling activity. Schedulers are 
// often implemented so they keep all computer resources busy (as in load balancing), allow multiple
// users to share system resources effectively, or to achieve a target quality of service. 
// Scheduling is fundamental to computation itself, and an intrinsic part of the execution model 
// of a computer system; the concept of scheduling makes it possible to have computer multitasking 
// with a single central processing unit(CPU).

//A scheduler may aim at one or more of many goals, for example:
// • Maximizing throughput(the total amount of work completed per time unit);
// • Minimizing wait time(time from work becoming enabled until the first point it begins 
//   execution on resources);
// • Minimizing latency or response time(time from work becoming enabled until it is finished 
//   in case of batch activity, or until the system responds and hands the first output to 
//   the user in case of interactive activity); or
// • Maximizing fairness(equal CPU time to each process, or more generally appropriate times 
//   according to the priority and workload of each process).
//
// In practice, these goals often conflict(e.g.throughput versus latency), thus a scheduler will 
// implement a suitable compromise.Preference is measured by any one of the concerns mentioned above, 
// depending upon the user's needs and objectives.
//
// ProcessScheduler class implements the base class functionality of a short term scheduler (also known
// as CPU Scheduler). It is an abstract class. You need to inherit and create your own scheduling algorithm.
// It implements 2 pools:
// 1) Pending list - Whenver a new task is created and added to the ready queue, it gets stored
//                   in this list. Task is picked up for execution from this list.
// 2) Complete map - This stores all the jobs that were completed.
//
// It also has APIs for monitoring important parameters of a process scheduling. 
// 
//-------------------------------------------------------------------------------------------------
class ProcessScheduler
{
private:
    mutex                                   m_schedulerMutex;
    long long                               m_tsCreated;                  // timestamp value when this scheduler was created
    int                                     m_displayInterval;
protected:
    unsigned long long                      m_totalJobsInflow;
    string                                  m_schedulerName;              // name of the scheduling alogorithm used
    list<Job*>                              m_pendingJobPool;             // List of all the pending jobs
    unordered_map<unsigned long, Job*>      m_completedJobPool;           // Using map as this can be huge.

public:
    ProcessScheduler(string name);
    ~ProcessScheduler();
    virtual void setSchedulerName(string name) = 0;                       // This needs to be implemented by inheriting classes
    string getSchedulerName();
    virtual bool DoJob(Job *job);
    virtual list<Job*>::iterator JobComplete(Job *job);
    virtual void ProcessJobs() = 0;                                       // This needs to be implemented by inheriting classes.
                                                                          // It contains the main algorithm being implemented by the
                                                                          // scheduling algorithm.

    virtual bool addToReadyQueue(Job* job);                               // This can be overridden depending on how the algorithm
                                                                          // adds the job to its queue.
    void setDisplayInterval(int interval);
    int getDisplayInterval();
    void displayStatsAtInterval(time_t & t1, time_t & t2);
    void displayStats();

    double getJobInflowRate();
    double getAverageWaitingTime();
    double getAverageResponseTime();
    double getThroughput();
};

#endif

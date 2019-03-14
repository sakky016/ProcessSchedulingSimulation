#include "process_scheduler.h"
#include<Windows.h>
#include<assert.h>

//******************************************************************************************
// @name                    : ProcessScheduler
//
// @description             : Constructor
//
// @returns                 : Nothing
//******************************************************************************************
ProcessScheduler::ProcessScheduler(string name)
{
    m_schedulerName = name;
    m_simulationComplete = false;
    m_totalJobsInflow = 0;
    m_displayInterval = 10;          // Default is 10 seconds
    m_tsCreated = getCurrentTimestampInMilliseconds();
}

//******************************************************************************************
// @name                    : ~ProcessScheduler
//
// @description             : Clears the memory occupied by all the jobs in the pending
//                            and completed job pools.
//
// @returns                 : Nothing
//******************************************************************************************
ProcessScheduler::~ProcessScheduler()
{
    printf("Terminating scheduler [ %s ]\n", getSchedulerName().c_str());

    // Clear memory allocated to job object in ready queue
    for (auto it = m_readyJobPool.begin(); it != m_readyJobPool.end(); it++)
    {
        Job *job = *it;
        delete job;
    }

    // Clear memory allocated to job object in Pending pool
    for (auto it = m_pendingJobPool.begin(); it != m_pendingJobPool.end(); it++)
    {
        Job *job = *it;
        delete job;
    }

    // Clear memory allocated to job object in Completed pool
    for (auto it = m_completedJobPool.begin(); it != m_completedJobPool.end(); it++)
    {
        Job *job = it->second;
        delete job;
    }

    m_readyJobPool.clear();
    m_pendingJobPool.clear();
    m_completedJobPool.clear();
}

//******************************************************************************************
// @name                    : getSchedulerName
//
// @description             : Fetches name of the scheduling algorithm being used.
//
// @returns                 : Scheduler name
//******************************************************************************************
string ProcessScheduler::getSchedulerName()
{
    return m_schedulerName;
}

//******************************************************************************************
// @name                    : addToReadyQueue
//
// @param job                : Job that needs to be added.
//
// @description             : Adds the specified job to the ready queue of the scheduler.
//                            This simulates the Short Term Scheduler(also known as the CPU scheduler)
//                            Jobs get added to the scheduler via this API continuously from
//                            the job creater into the pending jobs list. 
//
// @returns                 : Nothing
//********************************************************************************************
bool ProcessScheduler::addToReadyQueue(Job* job)
{
    m_readyJobPool.push_back(job);
    m_totalJobsInflow++;

    return true;
}

void ProcessScheduler::setDisplayInterval(int interval)
{
    m_displayInterval = interval;
}

int ProcessScheduler::getDisplayInterval()
{
    return m_displayInterval;
}

//******************************************************************************************
// @name                    : displayStats
//
// @param t1                : Start timestamp (seconds).
// @param t2                : End timestamp (seconds).
//
// @description             : Displays statistics for this Scheduler only at an interval
//                            as specified by the display interval. No stats will be displayed
//                            if the interval is < 0.
//
// @returns                 : Nothing
//********************************************************************************************
void ProcessScheduler::displayStatsAtInterval(time_t & t1, time_t & t2)
{
    if (getDisplayInterval() < 0)
    {
        // Do not display stats
        return;
    }

    // Display job statistics at pre-defined interval
    t2 = time(&t2);
    if (t2 - t1 >= getDisplayInterval())
    {
        displayStats();
        t1 = time(&t1);
    }
}

//******************************************************************************************
// @name                    : displayStats
//
// @description             : Displays statistics for this Scheduler.
//
// @returns                 : Nothing 
//********************************************************************************************
void ProcessScheduler::displayStats()
{
    printf("\n\n");
    printf("+------------------------------------------------------------------------+\n");
    printf("| %-70s |\n", getSchedulerName().c_str());
    printf("+------------------------------------------------------------------------+\n");
    printf("Time elapsed                            : %lld seconds\n", ((getCurrentTimestampInMilliseconds() - m_tsCreated) / 1000));
    printf("Total jobs added to ready queue         : %llu\n", m_totalJobsInflow);
    printf("Pending jobs                            : %u\n", m_pendingJobPool.size());
    printf("Completed jobs                          : %u\n", m_completedJobPool.size());
    printf("Job inflow rate                         : %lf per second.\n", getJobInflowRate());
    printf("Avg time required by job to complete    : %lf ms.\n", getAverageTimeRequired());
    printf("Average waiting time                    : %lf ms.\n", getAverageWaitingTime());// / (double)1000);
    printf("Average response time                   : %lf ms.\n", getAverageResponseTime());// / (double)1000);
    printf("Response threshold exceeded for         : %.2lf %% jobs\n", getResponseThresholdExceededJobs());
    printf("Throughput                              : %lf per second.\n", getThroughput());
    printf("+------------------------------------------------------------------------+\n\n");
}


//******************************************************************************************
// @name                    : getAverageWaitingTime
//
// @description             : Get average waiting time (in ms.) of all the jobs in ready queue.
//                            Wait time - time from work becoming enabled until the first 
//                                        point it begins execution on resources
//
// @returns                 : AverageWaitingTime
//********************************************************************************************
double ProcessScheduler::getAverageWaitingTime()
{
    if (m_completedJobPool.size() == 0)
    {
        return 0;
    }

    long long totalWaitTime = 0;
    for (auto it = m_completedJobPool.begin(); it != m_completedJobPool.end(); it++)
    {
        Job *job = it->second;
        if (totalWaitTime + job->getWaitingTime() < totalWaitTime)
        {
            printf("ASSERTION failed: totalWaitTime exceeded limit. %lld, %lld\n", totalWaitTime, job->getWaitingTime());
            assert(0);
        }

        totalWaitTime += job->getWaitingTime();
    }

    return (double)totalWaitTime / m_completedJobPool.size();
}

//******************************************************************************************
// @name                    : getAverageTimeRequired
//
// @description             : Get average time required (in ms.) of all the jobs that have completed
//
// @returns                 : AverageTimeRequired
//********************************************************************************************
double ProcessScheduler::getAverageTimeRequired()
{
    if (m_completedJobPool.size() == 0)
    {
        return -1;
    }

    long long totalTimeRequired = 0;
    for (auto it = m_completedJobPool.begin(); it != m_completedJobPool.end(); it++)
    {
        Job *job = it->second;

        if (totalTimeRequired + job->getJobTimeRequired() < totalTimeRequired)
        {
            printf("ASSERTION failed: totalTimeRequired exceeded limit. %lld, %lld\n", totalTimeRequired, job->getJobTimeRequired());
            assert(0);
        }

        totalTimeRequired += job->getJobTimeRequired();
    }

    return (double)totalTimeRequired / m_completedJobPool.size();
}


//******************************************************************************************
// @name                    : getAverageResponseTime
//
// @description             : Get average response time (in ms.) of all the jobs. 
//                            Response time- time from work becoming enabled until it is finished 
//
// @returns                 : AverageResponseTime
//********************************************************************************************
double ProcessScheduler::getAverageResponseTime()
{
    if (m_completedJobPool.size() == 0)
    {
        return -1;
    }

    long long totalResponseTime = 0;
    for (auto it = m_completedJobPool.begin(); it != m_completedJobPool.end(); it++)
    {
        Job *job = it->second;

        if (totalResponseTime + job->getResponseTime() < totalResponseTime)
        {
            printf("ASSERTION failed: totalResponseTime exceeded limit. %lld, %lld\n", totalResponseTime, job->getResponseTime());
            assert(0);
        }

        totalResponseTime += job->getResponseTime();
    }

    return (double)totalResponseTime / m_completedJobPool.size();
}

//******************************************************************************************
// @name                    : getThroughput
//
// @description             : Get throughput(the total amount of work completed per second).
//
// @returns                 : Throughput
//********************************************************************************************
double ProcessScheduler::getThroughput()
{
    long long currentTimestamp = getCurrentTimestampInMilliseconds();
    if (m_completedJobPool.size() == 0 || (currentTimestamp - m_tsCreated == 0))
    {
        return 0;
    }

    long long timeDiff = (currentTimestamp - m_tsCreated);
    double throughput = ((double)(m_completedJobPool.size() * 1000)) / timeDiff;

    return throughput;
}

//******************************************************************************************
// @name                    : getJobInflowRate
//
// @description             : Get jobs added to ready queue per second.
//
// @returns                 : jobInflowRate
//********************************************************************************************
double ProcessScheduler::getJobInflowRate()
{
    long long currentTimestamp = getCurrentTimestampInMilliseconds();
    if ((m_totalJobsInflow == 0) || (currentTimestamp - m_tsCreated == 0))
    {
        return 0;
    }

    long long timeDiff = (currentTimestamp - m_tsCreated);
    double jobInflowRate = ((double)(m_totalJobsInflow * 1000)) / timeDiff;

    return jobInflowRate;
}

//******************************************************************************************
// @name                    : JobComplete
//
// @description             : This will move the job from pending job pool to completed 
//                            job pool.
//
// @param job               : Job to execute
//
// @returns                 : Returns an iterator to the next job in the pending list.
//********************************************************************************************
list<Job*>::iterator ProcessScheduler::JobComplete(Job *job)
{
    unsigned long jobId = job->getJobId();

    // Place this job in completed pool
    m_completedJobPool[jobId] = job;

    // Remove from pending jobs pool
    auto it = m_pendingJobPool.begin();
    while (it != m_pendingJobPool.end())
    {
        if ((*it)->getJobId() == jobId)
        {
            it = m_pendingJobPool.erase(it);
            //printf(" Removing [%lu] from Pending pool\n", jobId);
            break;
        }
        else
        {
            it++;
        }
    }

    return it;
}

//******************************************************************************************
// @name                    : DoJob
//
// @description             : Simulate execution of a job. This sleep is just a simulation 
//                            for doing work. Once this sleep duration gets completed, it 
//                            will be assumed that the job has finished executing.
//
// @param job               : Job to execute
//
// @returns                 : true on job completion, 
//                            false otherwise
//********************************************************************************************
bool ProcessScheduler::DoJob(Job *job)
{
    Sleep(job->getJobTimeRemaining());
    return true;
}

//******************************************************************************************
// @name                    : setSimulationComplete
//
// @description             : Marks end of simulation. This is used in ProcessJobs() function
//                            to check if simulation has been terminated.
//
// @param val               : value
//
// @returns                 : Nothing
//********************************************************************************************
void ProcessScheduler::setSimulationComplete(bool val)
{
    m_schedulerMutex.lock();
    m_simulationComplete = val; 
    m_schedulerMutex.unlock();
}

//******************************************************************************************
// @name                    : getResponseThresholdExceededJobs
//
// @description             : Gets the percentage of completed jobs whose response time
//                            exceeded the threshold value.
//
// @returns                 : %age of jobs whose response time exceeded threshold.
//********************************************************************************************
double ProcessScheduler::getResponseThresholdExceededJobs()
{
    long long countExceededResponseTimes = 0;
    for (auto it = m_completedJobPool.begin(); it != m_completedJobPool.end(); it++)
    {
        Job *job = it->second;

        if (job->isResponseThresholdReached())
        {
            countExceededResponseTimes++;
        }
    }

    return ((double)countExceededResponseTimes / m_completedJobPool.size()) * 100;
}
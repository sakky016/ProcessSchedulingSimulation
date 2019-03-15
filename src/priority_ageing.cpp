#include "priority_ageing.h"
#include "job.h"
#include<time.h>
#include<Windows.h>

//******************************************************************************************
// @name                    : PriorityAgeingScheduling
//
// @description             : Constructor
//
// @returns                 : Nothing
//******************************************************************************************
PriorityAgeingScheduling::PriorityAgeingScheduling(string name) :ProcessScheduler(name)
{
    printf("\nCreating [ %s ] scheduler\n", name.c_str());
}

//******************************************************************************************
// @name                    : ~PriorityAgeingScheduling
//
// @description             : Destructor
//
// @returns                 : Nothing
//******************************************************************************************
PriorityAgeingScheduling::~PriorityAgeingScheduling()
{

}

//******************************************************************************************
// @name                    : setSchedulerName
//
// @description             : Stores name of the scheduling algorithm being used.
//
// @returns                 : Nothing
//******************************************************************************************
void PriorityAgeingScheduling::setSchedulerName(string name)
{
    m_schedulerName = name;
}


//******************************************************************************************
// @name                    : ProcessJobs
//
// @description             : Selects for execution the waiting process with the highest
//                            priority. If a job keeps on waiting beyond its time required
//                            for completion, its priority is increased.
//
// @returns                 : Nothing
//********************************************************************************************
void PriorityAgeingScheduling::ProcessJobs()
{
    time_t t1 = time(&t1);
    time_t t2 = time(&t2);

    // Process jobs in pending queue continuously
    while (!isSimulationComplete())
    {
        // Move all the jobs in ready queue to pending job pool. Thread synchronization
        // is required as m_readyJobPool might be continuously being updated by the
        // Job creation thread.
        m_schedulerMutex.lock();
        m_pendingJobPool.splice(m_pendingJobPool.end(), m_readyJobPool);
        m_schedulerMutex.unlock();

        if (m_pendingJobPool.size())
        {
            displayStatsAtInterval(t1, t2);

            // From the pending job pool, check if any job's priority has to be updated. 
            // If a job has spent as much time in the queue as its time to completion, its
            // priority shall be increased. Lower value means higher priority
            // This must be done before sorting the list.
            for (auto it = m_pendingJobPool.begin(); it != m_pendingJobPool.end(); it++)
            {
                Job *job = *it;
                long long currentWaitingTime = getCurrentTimestampInMilliseconds() - job->getJobTimeCreated();
                if (currentWaitingTime >= job->getJobTimeRequired() && job->getJobPriority() > JOB_PRIORITY_HIGHEST)
                {
                    job->setJobPriority(job->getJobPriority() - 1);
                }
            }

            // Sort the pending job pool with least time required by job to complete in the beginning.
            m_pendingJobPool.sort([](Job* lhs, Job* rhs) {return lhs->getJobPriority() < rhs->getJobPriority(); });

            // Iterate through all the jobs in the list. This list must be sorted
            // at this point of time. So process the job with highest priority
            // fist, and proceed in decreasing order thereafter.
            auto it = m_pendingJobPool.begin();
            while (it != m_pendingJobPool.end())
            {
                /*printf("Sorted pending job list: \n");
                for (auto it = m_pendingJobPool.begin(); it != m_pendingJobPool.end(); it++)
                {
                    printf("%lld  ", (*it)->getJobTimeRemaining());
                }
                printf("\n");*/

                Job *job = *it;
                bool jobComplete = false;

                job->markJobAsStartedExecution();
                //printf("Job [%-5lu] running...\n", job->getJobId());

                jobComplete = DoJob(job);
                if (jobComplete)
                {
                    job->markJobAsComplete();

                    // This will remove job from pending list. use
                    // the modified "it" value
                    it = this->JobComplete(job);
                }
                else
                {
                    // Increment it.
                    it++;
                }
            }// Reached end of Pending Job pool
        }
    }
}
#include "priority.h"
#include "job.h"
#include<time.h>
#include<Windows.h>

//******************************************************************************************
// @name                    : PriorityScheduling
//
// @description             : Constructor
//
// @returns                 : Nothing
//******************************************************************************************
PriorityScheduling::PriorityScheduling(string name) :ProcessScheduler(name)
{
    printf("Creating [ %s ] scheduler\n", name.c_str());
}

//******************************************************************************************
// @name                    : ~PriorityScheduling
//
// @description             : Destructor
//
// @returns                 : Nothing
//******************************************************************************************
PriorityScheduling::~PriorityScheduling()
{

}

//******************************************************************************************
// @name                    : setSchedulerName
//
// @description             : Stores name of the scheduling algorithm being used.
//
// @returns                 : Nothing
//******************************************************************************************
void PriorityScheduling::setSchedulerName(string name)
{
    m_schedulerName = name;
}


//******************************************************************************************
// @name                    : ProcessJobs
//
// @description             : Selects for execution the waiting process with the highest
//                            priority
//
// @returns                 : Nothing
//********************************************************************************************
void PriorityScheduling::ProcessJobs()
{
    time_t t1 = time(&t1);
    time_t t2 = time(&t2);

    // Process jobs in pending queue continuously
    while (1)
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
                    printf("%lld  ", (*it)->getJobTimeRequired());
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
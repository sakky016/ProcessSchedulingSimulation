#include "sjf.h"
#include "job.h"
#include<time.h>
#include<Windows.h>

//******************************************************************************************
// @name                    : ShortestJobFirst
//
// @description             : Constructor
//
// @returns                 : Nothing
//******************************************************************************************
ShortestJobFirst::ShortestJobFirst(string name) :ProcessScheduler(name)
{
    printf("\nCreating [ %s ] scheduler\n", name.c_str());
}

//******************************************************************************************
// @name                    : ~ShortestJobFirst
//
// @description             : Destructor
//
// @returns                 : Nothing
//******************************************************************************************
ShortestJobFirst::~ShortestJobFirst()
{

}

//******************************************************************************************
// @name                    : setSchedulerName
//
// @description             : Stores name of the scheduling algorithm being used.
//
// @returns                 : Nothing
//******************************************************************************************
void ShortestJobFirst::setSchedulerName(string name)
{
    m_schedulerName = name;
}


//******************************************************************************************
// @name                    : ProcessJobs
//
// @description             : Selects for execution the waiting process with the smallest 
//                            execution time.[1] SJN is a non - preemptive algorithm.It is 
//                            advantageous because of its simplicity and because it minimizes 
//                            the average amount of time each process has to wait until its 
//                            execution is complete.

//                            However, it has the potential for process starvation for 
//                            processes which will require a long time to complete if short 
//                            processes are continually added.Highest response ratio next is 
//                            similar but provides a solution to this problem using a technique 
//                            called ageing.
//
//                            Another disadvantage of using SJF is that the total execution 
//                            time of a job must be known before execution.While it is 
//                            impossible to predict execution time perfectly, several 
//                            methods can be used to estimate it, such as a weighted average 
//                            of previous execution times.

//
// @returns                 : Nothing
//********************************************************************************************
void ShortestJobFirst::ProcessJobs()
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
            // Display stats
            displayStatsAtInterval(t1, t2);

            // Sort the pending job pool with least time required by job to complete in the beginning.
            m_pendingJobPool.sort([](Job* lhs, Job* rhs) {return lhs->getJobTimeRemaining() < rhs->getJobTimeRemaining(); });

            // Iterate through all the jobs in the list. This list must be sorted
            // at this point of time. So process the job with shortest time to complete
            // fist, and proceed in increasing order thereafter.
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
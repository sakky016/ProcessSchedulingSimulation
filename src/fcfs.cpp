#include "fcfs.h"
#include "job.h"
#include<time.h>
#include<Windows.h>

//******************************************************************************************
// @name                    : FirstComeFirstServed
//
// @description             : Constructor
//
// @returns                 : Nothing
//******************************************************************************************
FirstComeFirstServed::FirstComeFirstServed(string name):ProcessScheduler(name)
{
    printf("\nCreating [ %s ] scheduler\n", name.c_str());
}

//******************************************************************************************
// @name                    : ~FirstComeFirstServed
//
// @description             : Destructor
//
// @returns                 : Nothing
//******************************************************************************************
FirstComeFirstServed::~FirstComeFirstServed()
{

}

//******************************************************************************************
// @name                    : setSchedulerName
//
// @description             : Stores name of the scheduling algorithm being used.
//
// @returns                 : Nothing
//******************************************************************************************
void FirstComeFirstServed::setSchedulerName(string name)
{
    m_schedulerName = name;
}

//******************************************************************************************
// @name                    : ProcessJobs
//
// @description             : Processes the job on FCFS basis.
//                            First in, first out (FIFO), also known as first come, 
//                            first served (FCFS), is the simplest scheduling algorithm. 
//                            FIFO simply queues processes in the order that they arrive in the ready queue. 
//
// @returns                 : Nothing
//********************************************************************************************
void FirstComeFirstServed::ProcessJobs()
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

            // Iterate through all the jobs in the list. Process it on
            // a first-come first-serve basis
            auto it = m_pendingJobPool.begin();
            while (it != m_pendingJobPool.end())
            {
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
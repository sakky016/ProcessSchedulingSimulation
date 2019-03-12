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
    while (1)
    {
        if (m_pendingJobPool.size())
        {
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

                displayStatsAtInterval(t1, t2);
            }
        }
    }
}
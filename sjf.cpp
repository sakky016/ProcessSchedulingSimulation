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
    printf("Creating [ %s ] scheduler\n", name.c_str());
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
    while (1)
    {
        if (m_pendingJobPool.size())
        {
            // Sort the list on the basis of Time Required by a job
            m_pendingJobPool.sort([](Job *first, Job *second) 
            { 
                return first->getJobTimeRequired() < second->getJobTimeRequired(); 
            });

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
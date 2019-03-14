#include "round_robin.h"
#include "job.h"
#include<time.h>
#include<Windows.h>

//******************************************************************************************
// @name                    : RoundRobin
//
// @description             : Constructor
//
// @returns                 : Nothing
//******************************************************************************************
RoundRobin::RoundRobin(string name, long long timeQuantum) :ProcessScheduler(name)
{
    m_timeQuantum = timeQuantum;
    printf("\nCreating [ %s ] scheduler with time quantum: %lld ms.\n", name.c_str(), m_timeQuantum);
}

//******************************************************************************************
// @name                    : ~RoundRobin
//
// @description             : Destructor
//
// @returns                 : Nothing
//******************************************************************************************
RoundRobin::~RoundRobin()
{

}

//******************************************************************************************
// @name                    : setSchedulerName
//
// @description             : Stores name of the scheduling algorithm being used.
//
// @returns                 : Nothing
//******************************************************************************************
void RoundRobin::setSchedulerName(string name)
{
    m_schedulerName = name;
}


//******************************************************************************************
// @name                    : ProcessJobs
//
// @description             :  The scheduler assigns a fixed time unit per process, and cycles 
//                             through them. If process completes within that time-slice it gets 
//                             terminated otherwise it is rescheduled after giving a chance 
//                             to all other processes.
//                             • RR scheduling involves extensive overhead, especially with 
//                               a small time unit.
//                             • Balanced throughput between FCFS / FIFO and SJF / SRTF, 
//                               shorter jobs are completed faster than in FIFO and longer 
//                               processes are completed faster than in SJF.
//                             • Good average response time, waiting time is dependent on 
//                               number of processes, and not average process length.
//                             • Because of high waiting times, deadlines are rarely met in a 
//                               pure RR system.
//                             • Starvation can never occur, since no priority is given.
//                               Order of time unit allocation is based upon process arrival time, 
//                               similar to FIFO.
//                             • If Time - Slice is large it becomes FCFS / FIFO or if it is 
//                               short then it becomes SJF / SRTF.
//
// @returns                 : Nothing
//********************************************************************************************
void RoundRobin::ProcessJobs()
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

            // Iterate through all the jobs in the list. Execute each job for
            // TIME_QUANTUM period of time and then proceed to next job.
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
                    // If the job could not get completed in this time slice, it means
                    // that it executed for TIME_QUANTUM amount of time. Store this
                    // information as a progress indicator of this job.
                    job->markJobAsPaused(getTimeQuantum());

                    // Move to process next job in the pending pool.
                    it++;
                }
            }// Reached end of Pending Job pool
        }
    }
}

//******************************************************************************************
// @name                    : DoJob
//
// @description             : Simulate execution of a job. This sleep is just a simulation 
//                            for doing work. Once this sleep duration gets completed, it 
//                            will be assumed that the job has finished executing. 
//
//                            If the time quantum expires before completion of job, we return
//                            with False (meaning that job has not been completed yet).
//
// @param job               : Job to execute
//
// @returns                 : true on job completion, 
//                            false otherwise
//********************************************************************************************
bool RoundRobin::DoJob(Job *job)
{
    bool isJobCompleted = false;

    if (job->getJobTimeRemaining() <= getTimeQuantum())
    {
        // We can finish the job in this time slice
        Sleep(job->getJobTimeRemaining());
        isJobCompleted = true;
    }
    else
    {
        Sleep(getTimeQuantum());
    }

    return isJobCompleted;
}
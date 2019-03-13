#include<iostream>
#include<stdio.h>
#include<thread>
#include <windows.h>

#include "fcfs.h"
#include "sjf.h"
#include "job.h"
#include "process_scheduler.h"

using namespace std;

//---------------------------------------------------------------------------------------------------
// Enum
//---------------------------------------------------------------------------------------------------
typedef enum
{
    SCHEDULING_FCFS,
    SCHEDULING_PRIORITY,
    SCHEDULING_SJF,
    SCHEDULING_ROUND_ROBIN,
    SCHEDULING_MULTI_LEVEL_FEEDBACK,

    // This should be last
    SCHEDULING_MAX
}schedulingAlgorithm_en;


//---------------------------------------------------------------------------------------------------
// CONFIGURATIONS - Alter to vary simulation results
//---------------------------------------------------------------------------------------------------
// Scheduling algorithm to use. Refer to enum 'schedulingAlgorithm_en' above.
// Implemented algos- SCHEDULING_FCFS, SCHEDULING_SJF
const int SCHEDULING_ALGORITHM = SCHEDULING_FCFS;

// Number of jobs for which simulation has to be done.
// use '-1' to create jobs continuously.
const long int JOBS_TO_CREATE = 1000;

// Use random sleep duration for creation thread
const bool USE_RANDOM_JOB_CREATION_SLEEP = false;

// Seconds to wait before displaying statistics
const int STATS_DISPLAY_INTERVAL = 10;

// Max time duration (ms) before proceeding to 
// create another job. This means that before creating
// a new job, the system will wait for any value in b/w
// 0 to JOB_CREATION_SLEEP_MAX-1 seconds.
const int JOB_CREATION_SLEEP_MAX = 1000;

// Milliseconds before proceeding to create another job
const int JOB_CREATION_SLEEP_CONST = 1000;


//---------------------------------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------------------------------
unsigned long long g_totalJobs = 0;

//---------------------------------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------------------------------

//******************************************************************************************
// @name                    : createJob
//
// @description             : Create a a job with a jobID and random priority.
//
// @returns                 : Pointer to created job
//********************************************************************************************
Job* createJob()
{
    static RandomGenerator rng;
    Job *job = new Job(++g_totalJobs, rng.generateRandomNumber(JOB_PRIORITY_LOWEST));
    return job;
}


//******************************************************************************************
// @name                    : jobCreationThread
//
// @description             : This thread is spawned from main(). Creates jobs and adds it to
//                            the ready queue of the scheduler.
//
// @returns                 : Nothing
//********************************************************************************************
void jobCreationThread(ProcessScheduler *scheduler)
{
    static RandomGenerator rng;
    long int i = 0;

    while (i < JOBS_TO_CREATE)
    {
        Job *j = createJob();
        scheduler->addToReadyQueue(j);
        //j->displayJobDetails();

        if (USE_RANDOM_JOB_CREATION_SLEEP)
        {
            // Wait for some random time duration before proceeding to
            // create another job
            Sleep(rng.generateRandomNumber(JOB_CREATION_SLEEP_MAX));
        }
        else
        {
            Sleep(rng.generateRandomNumber(JOB_CREATION_SLEEP_CONST));
        }

        // Do this only if a finite value is specified in configurations
        if (JOBS_TO_CREATE >= 0)
        {
            i++;
        }
    } // simulation complete

    // Wait till all the created jobs are complete
    while (scheduler->getCompletedJobs() < JOBS_TO_CREATE) 
    {
        // DO NOTHING
    }

    // Display final statistics
    scheduler->displayStats();
    printf("\n\n SIMULATION COMPLETE \n\n");

}

/*-----------------------------------------------------------------------------------------------------
M A I N
--------
This program simulates the Process Scheduling. First a scheduler has to be created. Following are 
supported:
1) FCFS (First-come First-served)
2) Priority Scheduling
3) SJF (Shortest Job First)
4) Round Robin
5) Multi-level feedback queue

This has been implemented by inheriting from the ProcessScheduler class. Depending on the name
of the algorithm, derived class for that particular scheduler is created.

Jobs with an ID and a random priority are continuously created by an RNG (Random Number Generator).
The job creator is a separate thread. This is done so as to simulate scenarios where pre-emption is
required.

The scheduler is then started with ProcessJobs() API. it will keep on processing the jobs present
in the pending job pool list.
-----------------------------------------------------------------------------------------------------*/
int main()
{
    ProcessScheduler *scheduler = nullptr;
    switch (SCHEDULING_ALGORITHM)
    {
    case SCHEDULING_FCFS:
        scheduler = new FirstComeFirstServed("First Come First Served");
        break;
    case SCHEDULING_PRIORITY:
        //scheduler = new PriorityScheduling("Priority Scheduling");
        break;
    case SCHEDULING_SJF:
        scheduler = new ShortestJobFirst("Shortest Job First");
        break;
    case SCHEDULING_ROUND_ROBIN:
        //scheduler = new RoundRobin("Round Robind");
        break;
    case SCHEDULING_MULTI_LEVEL_FEEDBACK:
        //scheduler = new MultiLevelFeedback("Multi Level Feedback");
        break;
    default:
        printf("ERROR: Invalid Scheduling algorithm specified!\n");
        exit(0);
    }
    
    // Display jobs being created for this simulation
    if (JOBS_TO_CREATE >= 0)
    {
        printf("Running simulation for %ld jobs\n", JOBS_TO_CREATE);
    }
    else
    {
        printf("Running simulation jobs being created continuously\n");
    }

    if (USE_RANDOM_JOB_CREATION_SLEEP)
    {
        printf("Using random job creation rate\n");
    }
    else
    {
        printf("Waiting for %f seconds before creating a new job\n", (float)JOB_CREATION_SLEEP_CONST / 1000);
    }

    // Set stats display interval
    scheduler->setDisplayInterval(STATS_DISPLAY_INTERVAL);
    printf("Using stats display interval: %d seconds\n", scheduler->getDisplayInterval());

    // Spawn a thread to create jobs randomly
    thread jobCreationThreadId = thread(jobCreationThread, scheduler);
    
    // Start executing the jobs
    scheduler->ProcessJobs();

    // Wait for Job creation thread to complete.
    jobCreationThreadId.join();
    
    getchar();
    return 0;
}
#include<iostream>
#include<stdio.h>
#include<thread>
#include <windows.h>

#include "fcfs.h"
#include "job.h"
#include "process_scheduler.h"

using namespace std;

//---------------------------------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------------------------------
//#define USE_RANDOM_JOB_CREATION_SLEEP

//---------------------------------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------------------------------
const int JOB_CREATION_SLEEP_MAX = 1000;           // Max time duration (ms) before proceeding to 
                                                   // create another job. This means that before creating
                                                   // a new job, the system will wait for any value in b/w
                                                   // 0 to JOB_CREATION_SLEEP_MAX-1 seconds.

const int JOB_CREATION_SLEEP_CONST = 1000;         // Milliseconds before proceeding to create another job

unsigned long long g_totalJobs = 0;

//---------------------------------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------------------------------

// Create a random job
Job* createJob()
{
    static RandomGenerator rng;
    Job *job = new Job(++g_totalJobs, rng.generateRandomNumber(JOB_PRIORITY_LOWEST));
    return job;
}

void jobCreationThread(ProcessScheduler *scheduler)
{
    static RandomGenerator rng;
    while (1)
    {
        Job *j = createJob();
        scheduler->addToReadyQueue(j);
        //j->displayJobDetails();

#ifdef USE_RANDOM_JOB_CREATION_SLEEP
        // Wait for some random time duration before proceeding to
        // create another job
        Sleep(rng.generateRandomNumber(JOB_CREATION_SLEEP_MAX));
#else
        Sleep(rng.generateRandomNumber(JOB_CREATION_SLEEP_CONST));
#endif
    }
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
    ProcessScheduler *scheduler = new FirstComeFirstServed("First Come First Served");
    
    // Spawn a thread to create jobs randomly
    thread jobCreationThreadId = thread(jobCreationThread, scheduler);
    
    // Start executing the jobs
    scheduler->ProcessJobs();

    // Wait for Job creation thread to complete.
    jobCreationThreadId.join();
    
    getchar();
    return 0;
}
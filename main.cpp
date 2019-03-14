#include<iostream>
#include<stdio.h>
#include<thread>
#include <windows.h>

#include "fcfs.h"
#include "sjf.h"
#include "priority.h"
#include "round_robin.h"
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

// Simulate all the algorithms.
const bool SIMULATE_ALL_SCHEDULING_ALGORITHMS = true;

// Scheduling algorithm to use. Refer to enum 'schedulingAlgorithm_en' above.
// Implemented algos- SCHEDULING_FCFS, SCHEDULING_SJF, SCHEDULING_PRIORITY, SCHEDULING_ROUND_ROBIN
const int SCHEDULING_ALGORITHM = SCHEDULING_PRIORITY;

// Time quantum (in milliseconds) to be used. This is used in case of Round-Robin scheduling
// algorithm. This is the maximum time window a job will be allocated for execution.
// If the job completes in this duration, then the next job is processed. Otherwise,
// the job is paused and its state is saved. The scheduler will process other jobs for this
// time quantum again and return back to process this job again.
const long long TIME_QUANTUM = 500;

// Number of jobs for which simulation has to be done.
// use '-1' to create jobs continuously.
const long int JOBS_TO_CREATE = 1000;

// Show details of job running status
const bool SHOW_JOB_STATUS = false;

// Use random sleep duration for creation thread
const bool USE_RANDOM_JOB_CREATION_SLEEP = false;

// Seconds to wait before displaying statistics
// Use -1 to display the stats only at the end of simulation.
// Be careful though, if the simulation is run without a finite
// number of jobs, no stats would be shown.
const int STATS_DISPLAY_INTERVAL = -1;

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
    Job *job = new Job(SHOW_JOB_STATUS, ++g_totalJobs, rng.generateRandomNumber(JOB_PRIORITY_LOWEST));
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
        // This sleep is very important. As it allows the thread
        // to stop processing while loop continuosly thereby offering
        // the event generation thread to provide it the required
        // signal (getCompletedJobs()). If this sleep is not provided
        // this thread will continuously be looping of the while loop
        // and event thread will not get a chance to execute and provide
        // the required event.
        //
        //This is an improvement (in comparison to while loop without sleep), 
        // because the thread doesn’t waste processing time while it’s sleeping, 
        // but it’s hard to get the sleep period right. Too short a sleep 
        // between checks causes the thread to waste processing time checking; 
        // too long a sleep and the thread keeps on sleeping even when the 
        // task it’s waiting for is complete, introducing a delay. 

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Display final statistics
    scheduler->displayStats();
    scheduler->setSimulationComplete(true);
    printf("** SIMULATION COMPLETE **\n");
}

//******************************************************************************************
// @name                    : getScheduler
//
// @description             : Retrieves scheduler object on the basis of algorithm
//
// @param algo_index        : algorithm index
//
// @returns                 : Scheduler object
//********************************************************************************************
ProcessScheduler* getScheduler(schedulingAlgorithm_en algo_index)
{
    ProcessScheduler *scheduler = nullptr;

    switch (algo_index)
    {
    case SCHEDULING_FCFS:
        scheduler = new FirstComeFirstServed("First Come First Served");
        break;
    case SCHEDULING_PRIORITY:
        scheduler = new PriorityScheduling("Priority Scheduling");
        break;
    case SCHEDULING_SJF:
        scheduler = new ShortestJobFirst("Shortest Job First");
        break;
    case SCHEDULING_ROUND_ROBIN:
        scheduler = new RoundRobin("Round Robin", TIME_QUANTUM);
        break;
    case SCHEDULING_MULTI_LEVEL_FEEDBACK:
        printf("SCHEDULING_MULTI_LEVEL_FEEDBACK: Not implemented yet\n");
        //scheduler = new MultiLevelFeedback("Multi Level Feedback");
        exit(0);
        break;
    default:
        printf("ERROR: Invalid Scheduling algorithm specified!\n");
        exit(0);
    }

    return scheduler;
}

//******************************************************************************************
// @name                    : doSimulation
//
// @description             : Runs the simulation
//
// @param scheduler         : scheduler object
//
// @returns                 : Nothing
//********************************************************************************************
void doSimulation(ProcessScheduler *scheduler, int simulationIndex)
{
    if (!scheduler)
    {
        printf("ERROR: No scheduler found!\n");
        exit(0);
    }

    printf("\n");
    printf("+------------------------------------------------------------------------+\n");
    printf("| SIMULATION #%d: %s\n", simulationIndex+1, scheduler->getSchedulerName().c_str());
    printf("+------------------------------------------------------------------------+\n");

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
        printf("Waiting for %f second(s) before creating a new job\n", (float)JOB_CREATION_SLEEP_CONST / 1000);
    }

    // Set stats display interval
    scheduler->setDisplayInterval(STATS_DISPLAY_INTERVAL);
    if (STATS_DISPLAY_INTERVAL >= 0)
    {
        printf("Using stats display interval: %d second(s)\n", scheduler->getDisplayInterval());
    }
    else
    {
        printf("Scheduler stats will be shown at the end of the simulation.\n");
        printf("Running simulation...\n");
    }

    // Spawn a thread to create jobs randomly
    thread jobCreationThreadId = thread(jobCreationThread, scheduler);

    // Start executing the jobs
    scheduler->ProcessJobs();

    // Wait for Job creation thread to complete.
    jobCreationThreadId.join();
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

The scheduler is then started with ProcessJobs() API. It will keep on processing the jobs present
in the pending job pool list.
-----------------------------------------------------------------------------------------------------*/
int main()
{
    ProcessScheduler *scheduler = nullptr;
    int totalSimulations = 0;

    // Default - the one specified by configuration
    schedulingAlgorithm_en schedulingAlgorithm = static_cast<schedulingAlgorithm_en>(SCHEDULING_ALGORITHM);

    if (SIMULATE_ALL_SCHEDULING_ALGORITHMS == true)
    {
        printf("Simulating all the available scheduling algorithms...\n");
        for (int algo_index = 0; algo_index < SCHEDULING_MAX - 1; algo_index++) //TODO: Update once all algos done
        {
            scheduler = getScheduler(static_cast<schedulingAlgorithm_en>(algo_index));
            doSimulation(scheduler, totalSimulations);
            scheduler->~ProcessScheduler();
            totalSimulations++;
        }
    }
    else
    {
        scheduler = getScheduler(schedulingAlgorithm);
        doSimulation(scheduler, totalSimulations);
        scheduler->~ProcessScheduler();
        totalSimulations++;
    }
    
    printf("\n**** Completed %d simulation(s)\n", totalSimulations);
    getchar();
    return 0;
}
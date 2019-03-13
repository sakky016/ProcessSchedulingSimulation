# Process Scheduling Simulation
# //---------------------------------------------------------------------------------------------------
# // CONFIGURATIONS - Alter to vary simulation results
# //---------------------------------------------------------------------------------------------------

// Scheduling algorithm to use. Refer to enum 'schedulingAlgorithm_en' above.
// Implemented algos- SCHEDULING_FCFS, SCHEDULING_SJF, SCHEDULING_PRIORITY, SCHEDULING_ROUND_ROBIN
const int SCHEDULING_ALGORITHM = SCHEDULING_ROUND_ROBIN;

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
const int STATS_DISPLAY_INTERVAL = 10;

// Max time duration (ms) before proceeding to 
// create another job. This means that before creating
// a new job, the system will wait for any value in b/w
// 0 to JOB_CREATION_SLEEP_MAX-1 seconds.
const int JOB_CREATION_SLEEP_MAX = 1000;

// Milliseconds before proceeding to create another job
const int JOB_CREATION_SLEEP_CONST = 1000;

This program simulates the Process Scheduling. First a scheduler has to be created. Following are 
supported:
1) FCFS (First-come First-served)
2) Priority Scheduling
3) SJF (Shortest Job First)
4) Round Robin
5) Multi-level feedback queue

# Program Description
This has been implemented by inheriting from the ProcessScheduler class. Depending on the name
of the algorithm, derived class for that particular scheduler is created.

Jobs with an ID and a random priority are continuously created by an RNG (Random Number Generator).
The job creator is a separate thread. This is done so as to simulate scenarios where pre-emption is
required.

The scheduler is then started with ProcessJobs() API. It will keep on processing the jobs present
in the pending job pool list.

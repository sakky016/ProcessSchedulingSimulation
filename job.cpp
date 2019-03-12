#include <chrono>
#include "job.h"

RandomGenerator rng;                            // Random Generator

// Get time stamp in milliseconds.
long long getCurrentTimestampInMilliseconds()
{
    long long ts_us = chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
                      now().time_since_epoch()).count();
    return ts_us;
}

//******************************************************************************************
// @name                    : Job
//
// @description             : Constructor
//
// @returns                 : Nothing
//******************************************************************************************
Job::Job(unsigned long jobId, unsigned int priority)
{
    m_jobId = jobId;
    m_priority = priority;
    m_state = STATE_READY;
    m_timeRequired = rng.generateRandomNumber(MAX_TIME_REQUIRED);    // Don't know how this would be estimated in an actual run
    m_tsCreated = getCurrentTimestampInMilliseconds();
    m_tsExecutionStart = -1;                                         // Not yet started
    m_tsExecutionEnd = -1;                                           // Not yet completed 
    m_isComplete = false;
}

//******************************************************************************************
// @name                    : ~Job
//
// @description             : Destructor
//
// @returns                 : Nothing
//******************************************************************************************
Job::~Job()
{

}

//******************************************************************************************
// @name                    : getWaitingTime
//
// @description             : Get the duration (ms) for which this task has been waiting in ready queue
//
// @returns                 : Nothing
//******************************************************************************************
long long Job::getWaitingTime()
{
    if (m_tsExecutionStart != -1)
    {
        return (m_tsExecutionStart - m_tsCreated);
    }

    // Job not yet started.
    return -1;
}


//******************************************************************************************
// @name                    : getResponseTime
//
// @description             : Get the duration (ms) from work becoming enabled until it is finished
//
// @returns                 : Nothing
//******************************************************************************************
long long Job::getResponseTime()
{
    if (m_isComplete)
    {
        return (m_tsExecutionEnd - m_tsCreated);
    }

    // Job not yet complete
    return -1;
}

//******************************************************************************************
// @name                    : displayJobDetails
//
// @description             : Display details of this job.
//
// @returns                 : Nothing
//******************************************************************************************
void Job::displayJobDetails()
{
    printf("\n");
    printf("Job ID          : %lu\n", m_jobId);
    printf("Priority        : %d\n", m_priority);
    printf("State           : %d\n", m_state);
    printf("Time required   : %lld ms.\n", m_timeRequired);
    printf("Created at      : %lld\n", m_tsCreated);
    printf("Waiting time    : %lld ms.\n", getWaitingTime());
    printf("Response time   : %lld ms.\n", getResponseTime());
    printf("Complete        : %s\n", m_isComplete ? "YES" : "NO");
}

//******************************************************************************************
// @name                    : markJobAsStartedExecution
//
// @description             : Updates the state and records the time when this job 
//                            started execution.
//
// @returns                 : Nothing
//******************************************************************************************
void Job::markJobAsStartedExecution()
{
    m_state = STATE_RUNNING;
    m_tsExecutionStart = getCurrentTimestampInMilliseconds();
}

//******************************************************************************************
// @name                    : markJobAsComplete
//
// @description             : Updates the state and records the time when this job 
//                            completed execution.
//
// @returns                 : Nothing
//******************************************************************************************
void Job::markJobAsComplete()
{
    m_state = STATE_COMPLETED;
    m_tsExecutionEnd = getCurrentTimestampInMilliseconds();
    m_isComplete = true;
}
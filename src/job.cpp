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
Job::Job(bool showJobStatus, unsigned long jobId, unsigned int priority, int responseTimeThreshold)
{
    m_showJobStatus = showJobStatus;
    m_jobId = jobId;
    m_priority = priority;
    m_state = STATE_READY;
    m_timeRequired = rng.generateRandomNumber(MAX_TIME_REQUIRED);    // Don't know how this would be estimated in an actual run
    m_timeServed = 0;                                                // Job has not yet started execution
    m_tsCreated = getCurrentTimestampInMilliseconds();
    m_tsExecutionStart = -1;                                         // Not yet started
    m_tsExecutionEnd = -1;                                           // Not yet completed 
    m_waitingTime = -1;
    m_responseTime = -1;
    m_responseTimeThreshold = responseTimeThreshold;
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
// @name                    : getJobTimeRemaining
//
// @description             : Calculates the time required to complete this job. The job
//                            might have been paused earlier several times (e.g- in case of
//                            Round Robin Scheduling algorithm), so this needs to take into 
//                            account the total time served in running this task.
//
// @returns                 : Time required to complete this job (in milli seconds)
//******************************************************************************************
long long Job::getJobTimeRemaining()
{ 
    return (m_timeRequired - m_timeServed); 
}

//******************************************************************************************
// @name                    : getJobTimeRequired
//
// @description             : Fetches the time it will take for this job to get completed
//                            in 1 go. This is different from getJobTimeRemaining() as it
//                            accounts for Paused jobs as well. This will give the time
//                            required as per the initial creation value. This is used
//                            for displaying statistics.
//
// @returns                 : Time required
//******************************************************************************************
long long Job::getJobTimeRequired()
{
    return m_timeRequired;
}

//******************************************************************************************
// @name                    : getWaitingTime
//
// @description             : Get the duration (ms) for which this task has been waiting in ready queue
//
// @returns                 : waiting time in ms.
//******************************************************************************************
long long Job::getWaitingTime()
{
    // Job not yet complete
    if (m_waitingTime < 0)
        printf("*** %s:: Job %lu not yet started\n", __FUNCTION__, m_jobId);

    return m_waitingTime;
}

//******************************************************************************************
// @name                    : setWaitingTime
//
// @description             : Set waiting time
//
// @returns                 : Nothing
//******************************************************************************************
void Job::setWaitingTime(long long waitingTime)
{
    m_waitingTime = waitingTime;
}

//******************************************************************************************
// @name                    : getResponseTime
//
// @description             : Get the duration (ms) from work becoming enabled until it is finished
//
// @returns                 : response time in ms.
//******************************************************************************************
long long Job::getResponseTime()
{
    // Job not yet complete
    if (m_responseTime < 0)
        printf("*** %s:: Job %lu not yet complete\n", __FUNCTION__, m_jobId);
    
    return m_responseTime;
}

//******************************************************************************************
// @name                    : setResponseTime
//
// @description             : Set response time
//
// @returns                 : Nothing
//******************************************************************************************
void Job::setResponseTime(long long responseTime)
{
    m_responseTime = responseTime;
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
    printf("Job ID             : %lu\n", m_jobId);
    printf("Priority           : %d\n", m_priority);
    printf("State              : %d\n", m_state);
    printf("Time required      : %lld ms.\n", m_timeRequired);
    printf("Time served        : %lld ms.\n", m_timeServed);
    printf("Created at         : %lld\n", m_tsCreated);
    printf("Execution started  : %lld\n", m_tsExecutionStart);
    printf("Execution ended    : %lld\n", m_tsExecutionEnd);
    printf("Waiting time       : %lld ms.\n", getWaitingTime());
    printf("Response time      : %lld ms.\n", getResponseTime());
    printf("Reached RT thrshold: %s\n", isResponseThresholdReached() ? "YES" : "NO");
    printf("Complete           : %s\n", isJobComplete() ? "YES" : "NO");
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
    if (m_state == STATE_READY)
    {
        m_tsExecutionStart = getCurrentTimestampInMilliseconds();
        if (isDebugEnabled())
            printf("Job %lu (Priority: %d, TimeRequired: %.2lf s.) started\n", m_jobId, m_priority, m_timeRequired / (double)1000);
    }
    else if (m_state == STATE_PAUSED)
    {
        if (isDebugEnabled())
            printf("Job %lu resumed from %.2lf/%.2lf (s.)\n", m_jobId, m_timeServed / (double)1000, m_timeRequired / (double)1000);
    }
    else
    {
        // DO NOTHING
    }

    m_state = STATE_RUNNING;
    setWaitingTime(m_tsExecutionStart - m_tsCreated);
}

//******************************************************************************************
// @name                    : markJobAsPaused
//
// @description             : Updates the state and records the time for which this
//                            job executed.
//
// @param timeServed        : Duration of time (ms) for which this job was executed. 
//
// @returns                 : Nothing
//******************************************************************************************
void Job::markJobAsPaused(long long timeServed)
{
    m_state = STATE_PAUSED;
    m_timeServed = timeServed;

    if (isDebugEnabled())
        printf("Job %lu paused %.2lf/%.2lf (s.)\n", m_jobId, m_timeServed / (double)1000, m_timeRequired / (double)1000);
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
    m_timeServed = m_timeRequired;
    setResponseTime(m_tsExecutionEnd - m_tsCreated);

    if (isDebugEnabled())
    {
        printf("Job %lu completed\n", m_jobId);
        //displayJobDetails();
    }
}

//******************************************************************************************
// @name                    : isThresholdReached
//
// @description             : Waiting Time Threshold- This parameter identifies the jobs 
//                            whose response time period exceeds the permissible value in comparison 
//                            to its time required for completion. So for instance if a job 
//                            requires 200ms. to complete and if the response time
//                            threshold is 2, then response time threshold for this job is 400ms.
//
// @returns                 : true if response time threshold is reached, 
//                            false otherwise
//******************************************************************************************
bool Job::isResponseThresholdReached()
{
    return (m_responseTime >= (m_responseTimeThreshold * m_timeRequired));
}
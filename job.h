#ifndef _JOB_H_
#define _JOB_H_

#include "random.h"
#include<mutex>

using namespace std;

//---------------------------------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------------------------------
const int JOB_PRIORITY_LOWEST      = 512;       // Lowest priority
const int JOB_PRIORITY_HIGHEST     = 0;         // Highest priority
const long long MAX_TIME_REQUIRED = 1000;       // Maximum time (ms) that a task can need to complete
long long getCurrentTimestampInMilliseconds();
//---------------------------------------------------------------------------------------------------
// Enums and structures
//---------------------------------------------------------------------------------------------------
typedef enum
{
    STATE_INVALID,
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED,
    STATE_PAUSED,
    STATE_COMPLETED,

    // This should be last
    STATE_MAX
}jobState_en;


//---------------------------------------------------------------------------------------------------
// Job/Task class
//---------------------------------------------------------------------------------------------------
class Job
{
private:
    bool                    m_showJobStatus;     // Show details of job running status
    mutex                   m_jobMutex;          // Mutex for Job class
    unsigned long           m_jobId;             // Unique Job ID
    unsigned int            m_priority;          // Priority of this job, lower is highest priority
    jobState_en             m_state;             // Current state of the job
    long long               m_timeRequired;      // Time required (in ms) by this job to complete.
    long long               m_timeServed;        // Time (in ms) for which this job has executed.
    long long               m_tsCreated;         // timestamp value when this job was created.
    long long               m_tsExecutionStart;  // timestamp value when this job got completed.
    long long               m_tsExecutionEnd;    // timestamp value when this job got completed.
    long long               m_waitingTime;       // Waiting time of this job
    long long               m_responseTime;      // Response time of this job

public:
    Job(bool showJobStatus, unsigned long jobId, unsigned int priority);
    ~Job();

    bool isDebugEnabled() { return m_showJobStatus; }

    unsigned long getJobId() {return m_jobId;}
    unsigned int getJobPriority() { return m_priority; }

    jobState_en getJobState() { return m_state;}
    void setJobState(jobState_en state) { m_state = state; }

    long long getJobTimeRemaining();
    long long getJobTimeRequired();

    void setJobTimeRequired(long long timeRequired) { m_timeRequired = timeRequired; }

    bool isJobComplete() { return (m_state == STATE_COMPLETED); }

    void displayJobDetails();
    long long getWaitingTime();
    void setWaitingTime(long long waitingTime);
    void setResponseTime(long long responseTime);
    long long getResponseTime();
    void markJobAsStartedExecution();
    void markJobAsPaused(long long timeServed);
    void markJobAsComplete();
};

#endif

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
const long long MAX_TIME_REQUIRED = 1000;      // Maximum time (ms) that a task can need to complete
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
    mutex                   m_jobMutex;
    unsigned long           m_jobId;             // Unique Job ID
    unsigned int            m_priority;          // Priority of this job, lower is highest priority
    jobState_en             m_state;             // Current state of the job
    long long               m_timeRequired;      // Time required (in ms) by this job to complete.
    long long               m_tsCreated;         // timestamp value when this job was created.
    long long               m_tsExecutionStart;  // timestamp value when this job got completed.
    long long               m_tsExecutionEnd;    // timestamp value when this job got completed.
    bool                    m_isComplete;        // Determines if job was completed. 

public:
    Job(unsigned long jobId, unsigned int priority);
    ~Job();

    unsigned long getJobId() {return m_jobId;}

    jobState_en getJobState() { return m_state;}
    void setJobState(jobState_en state) { m_state = state; }

    long long getJobTimeRequired() { return m_timeRequired; }
    void setJobTimeRequired(long long timeRequired) { m_timeRequired = timeRequired; }

    void displayJobDetails();
    long long getWaitingTime();
    long long getResponseTime();
    void markJobAsStartedExecution();
    void markJobAsComplete();
};

#endif

/**
 * Name: Quynh Anh Do (or Halie Do)
 * RedID: 827707836
 */
#include "process.h"
#include <iostream>

/**
 * @brief - Intialize a process to be passed into the scheduler
 *
 * @param id - ID of the process
 * @param bursts - Holds the burst times and I/O times of a process
 * @param burstCount - Total number of bursts to be executed in a process
 */
Process::Process(int id, std::vector<int> bursts, int burstCount)
    : processID(id), remainingBursts(bursts), numOfBursts(burstCount),
      previousCPUBurst(FIRST_CPU_BURST), previousCPUPrediction(FIRST_CPU_BURST),
      currIOTimeLeft(0), executedCPUBursts(0), executedIOBursts(0), waitTime(0), turnaroundTime(0) {}

/**
 * @brief - Update total CPU burst time executed so far
 */
void Process::updateCPUExecuted()
{
    ++executedCPUBursts;
}

/**
 * @brief - Update total IO burst time executed so far
 */
void Process::updateIOExecuted()
{
    ++executedIOBursts;
}

/**
 * @brief - Initiates burst for the process before entering IO execution
 *
 * @param burstTime - Duration of the next IO execution
 */
void Process::initiateIOBurst(int burstTime)
{
    currIOTimeLeft = burstTime;
    predictions.push_back(static_cast<double>(burstTime));
}

/**
 * @brief - Update time remaining in IO execution of a process
 */
void Process::updateIOTimeRemaining()
{
    currIOTimeLeft -= 1;
}

/**
 * @brief - Calculate current exponetial averaging value
 *          to determine order of a process in the ready queue
 * @param alpha - Used for calculating exponetial averaging value
 */
void Process::updateExponetialAveragingVal(double alpha)
{
    double expAvgValue = 0;

    if (previousCPUBurst != FIRST_CPU_BURST)
    {
        expAvgValue = alpha * (previousCPUBurst) + (1 - alpha) * (previousCPUPrediction);
    }
    else
    { // first cpu execution
        expAvgValue = getCPUBurstAvg();
    }

    // update previous bursts & prediction
    previousCPUBurst = remainingBursts.front();
    previousCPUPrediction = expAvgValue;
    predictions.push_back(expAvgValue);
}

/**
 * @brief - Return the average of all CPU bursts in a process
 *        - Used for first prediction of a process (if alpha is given)
 */
double Process::getCPUBurstAvg()
{
    double sum = 0;
    int numOfCPUBursts = 0;
    for (size_t i = 0; i < remainingBursts.size(); i++)
    {
        if (i % 2 == 0) // cpu bursts only occur on even index
        {
            sum += remainingBursts[i];
            ++numOfCPUBursts;
        }
    }
    return sum / numOfCPUBursts;
}

/**
 * @brief - Calculate turnaround time of a process
 */
void Process::calcTurnaroundTime()
{
    turnaroundTime = executedCPUBursts + executedIOBursts + waitTime;
}

/**
 * @brief - Update the wait time of a process in ready queue
 */
void Process::updateWaitTime()
{
    ++waitTime;
}

/**
 * @brief - Return the most recent CPU prediction value of a process
 */
double Process::getPrediction()
{
    return previousCPUPrediction;
}

/**
 * @brief - Return remaining time before an IO execution is completed
 */
int Process::getRemainingIOTime()
{
    return currIOTimeLeft;
}

/**
 * @brief - Remove completed burst from list of remaining bursts to complete
 */
void Process::goToNextBurst()
{
    remainingBursts.erase(remainingBursts.begin());
}

/**
 * @brief - Return current burst being executed
 */
int Process::getCurrBurst()
{
    return remainingBursts.front();
}

/**
 * @brief - Return a boolean value signifying whether there are bursts left to execute
 */
bool Process::hasRemainingBursts()
{
    return !remainingBursts.empty();
}
/**
 * @brief - Return ID of a process
 */
int Process::getProcessID()
{
    return processID;
}

/**
 * @brief - Return total CPU bursts executed
 */
int Process::getTotalExecutedCPUBursts()
{
    return executedCPUBursts;
}

/**
 * @brief - Return total IO bursts executed
 */
int Process::getTotalExecutedIOBursts()
{
    return executedIOBursts;
}

/**
 * @brief - Return total time taken to complete the process
 */
int Process::getTurnaroundTime()
{
    return turnaroundTime;
}

/**
 * @brief - Return total time the process spends waiting in the ready queue
 */
int Process::getTotalWaitTime()
{
    return waitTime;
}

/**
 * @brief - Return all CPU burst predictions used throughout the execution of the process
 */
std::vector<double> Process::getEstimatedBursts()
{
    return predictions;
}

/**
 * @brief - Return total number of bursts that have to be executed
 */
int Process::getNumOfBursts()
{
    return numOfBursts;
}

/**
 * @brief - Return a vector containing bursts to execute
 */
std::vector<int> Process::getBursts()
{
    return remainingBursts;
}

/**
 * @brief - Add actual CPU burst value to predictions array (when no alpha vlue is provided)
 *
 * @param currBurst - CPU burst time that was used as prediction value
 */
void Process::addCurrBurstToPredictions(double currBurst)
{
    predictions.push_back(currBurst);
}
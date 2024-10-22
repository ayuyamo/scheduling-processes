/**
 * Name: Quynh Anh Do (or Halie Do)
 * RedID: 827707836
 */
#include "scheduler.h"
#include "log.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>

/**
 * @brief - Initialize a scheduler to run processes given
 *
 * @param processes - Holds the proccesses to run in scheduler
 * @param alphaVal - Used for calculating exponential averaging value of each process
 */
Scheduler::Scheduler(std::vector<Process> processes, double alphaVal) : alpha(alphaVal), timeElapsed(0), processesToRun(processes) {}

/**
 * @brief - Push a process to the ready queue so it could be scheduled for execution
 *
 * @param process - Process to be added to the queue
 */
void Scheduler::pushToReady(Process &process)
{
    if (alpha != NO_ALPHA) // calculate exponential averaging value when
    {
        process.updateExponetialAveragingVal(alpha);
        readyQueue.push_back(process);
        // sort processes by exponential averaging value if there's more than one in the queue
        if (readyQueue.size() > 1)
        {
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process &p1, Process &p2)
                      { return p1.getPrediction() < p2.getPrediction(); });
        }
    }
    // when alpha wasn't provided, compare processes by their actual burst value
    else
    {
        process.addCurrBurstToPredictions(static_cast<double>(process.getCurrBurst()));
        readyQueue.push_back(process);
        // sort processes by their current CPU burst time if there's more than one in the queue
        if (readyQueue.size() > 1)
        {
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process &p1, Process &p2)
                      { return p1.getCurrBurst() < p2.getCurrBurst(); }); // might create problems later?
        }
    }
}

/**
 * @brief - Push a process to the blocked queue for IO burst execution
 *
 * @param process - Process to be added to the queue
 */
void Scheduler::pushToBlocked(Process &process)
{
    blockedQueue.push_back(process);
    // sort queue by current remaining IO burst of each process
    std::sort(blockedQueue.begin(), blockedQueue.end(), [](Process &p1, Process &p2)
              { return p1.getRemainingIOTime() < p2.getRemainingIOTime(); });
}

/**
 * @brief - Increment total IO executed & decrement current IO burst time by 1 for each process in blocked queue
 */
void Scheduler::updateIOTimeInBlockedQueue()
{
    for (Process &p : blockedQueue)
    {
        p.updateIOExecuted();
        p.updateIOTimeRemaining();
    }
}

/**
 * @brief - Push all processes that have completed its IO execution to ready queue
 *        - Removed completed IO burst from each process's array of burst to execute
 */
void Scheduler::pushCompletedIOToReady()
{
    while ((!blockedQueue.empty()) && (blockedQueue.front().getRemainingIOTime() == 0))
    {
        blockedQueue.front().goToNextBurst(); // IO finished, move to the next CPU burst
        pushToReady(blockedQueue.front());
        blockedQueue.pop_front();
    }
}

/**
 * @brief - After executing a CPU burst the process will be pushed to blocked or completed
 *          if there's no more process remaining
 * @param - The process that just finished executing it's CPU burst
 */
void Scheduler::updateProcessStateAfterCPUBurst(Process &p)
{
    if (!p.hasRemainingBursts()) // process completed because there is no burst left
    {
        p.calcTurnaroundTime();
        log_cpuburst_execution(p.getProcessID(), p.getTotalExecutedCPUBursts(), p.getTotalExecutedIOBursts(), timeElapsed, COMPLETED);
        processOrderOfCompletion.push_back(p); // push completed process to processOrderOfCompletion
    }
    else
    { // if process is not done, push back to blocked queue --> switched to IO
        log_cpuburst_execution(p.getProcessID(), p.getTotalExecutedCPUBursts(), p.getTotalExecutedIOBursts(), timeElapsed, ENTER_IO);
        p.initiateIOBurst(p.getCurrBurst());
        pushToBlocked(p);
    }
}

/**
 * @brief - Keeps counting down IO burst for each process in the blocked queue until burst time = 0
 *        - Push processes that completed its IO execution into ready queue
 *        - Called when no current process is running and the ready queue is empty
 */
void Scheduler::countDownIOAndMoveToReadyQueue()
{
    while ((!blockedQueue.empty()) && (blockedQueue.front().getRemainingIOTime() > 0))
    {
        updateIOTimeInBlockedQueue();
        ++timeElapsed;
    }
    pushCompletedIOToReady();
}

/**
 * @brief - Main function used for running the processes
 */
bool Scheduler::runProcess()
{
    bool finished = false; // assigned to true if all processes are completed

    for (Process &p : processesToRun) // push all initial CPU bursts from each process to ready queue
    {
        pushToReady(p);
    }

    Process running = readyQueue.front();          // current process being executed
    int CPUTimeRemaining = running.getCurrBurst(); // time remaining of  execution of current process
    readyQueue.pop_front();                        // pop process to be executed from ready queue

    while (!finished)
    {
        // while a process is being run
        while (CPUTimeRemaining > 0)
        {
            // update wait time of processes waiting in ready queue
            for (Process &p : readyQueue) // would not execute if queue is empty
            {
                p.updateWaitTime(); // increment wait time by 1 for each process
            }

            updateIOTimeInBlockedQueue();
            pushCompletedIOToReady();
            // update current running process's CPU executed
            running.updateCPUExecuted();
            ++timeElapsed;
            --CPUTimeRemaining;
        }

        // Move to next burst after CPU execution was completed
        running.goToNextBurst();
        updateProcessStateAfterCPUBurst(running);

        if (readyQueue.empty()) // execute when no process is running and the ready queue is empty
        {
            if (!blockedQueue.empty())
            {
                countDownIOAndMoveToReadyQueue();
            }
            else
            { // at this point all processes should have been completed
                finished = true;
                continue;
            }
        }

        // pop another process from ready to running (only in case if ready queue isn't empty)
        running = readyQueue.front();
        readyQueue.pop_front();
        CPUTimeRemaining = running.getCurrBurst();
    }

    // print final log after all processes are completed

    for (Process &p : processOrderOfCompletion)
    { // print turnaround and wait time of each process
        log_process_completion(p.getProcessID(), p.getTurnaroundTime(), p.getTotalWaitTime());
    }

    for (Process &p : processOrderOfCompletion)
    { // print estimated bursts of each process
        int numOfBursts = p.getNumOfBursts();
        std::vector<double> estimatedBursts = p.getEstimatedBursts();
        float bursts[numOfBursts];
        std::copy(estimatedBursts.begin(), estimatedBursts.end(), bursts);
        log_process_estimated_bursts(p.getProcessID(), bursts, numOfBursts);
    }

    return finished;
}

/**
 * Name: Quynh Anh Do (or Halie Do)
 * RedID: 827707836
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "process.h"
#include <deque>

#define NO_ALPHA -1 // alpha value is not usable
class Scheduler
{
private:
    double alpha;
    std::deque<Process> readyQueue;
    std::deque<Process> blockedQueue;
    int timeElapsed;
    std::vector<Process> processesToRun;
    std::vector<Process> processOrderOfCompletion;

public:
    Scheduler(std::vector<Process> processes, double alphaVal = NO_ALPHA);
    void pushToReady(Process &process);
    void pushToBlocked(Process &process);
    void updateIOTimeInBlockedQueue();
    void pushCompletedIOToReady();
    void countDownIOAndMoveToReadyQueue();
    void updateProcessStateAfterCPUBurst(Process &process);
    bool runProcess();
};
#endif
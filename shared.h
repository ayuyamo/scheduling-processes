/**
 * Name: Quynh Anh Do (or Halie Do)
 * RedID: 827707836
 */
#ifndef SHARED_H
#define SHARED_H
#include "process.h"

/**
 * Constructor containing data to be shared between the main & worker threads
 * and the scheduler that will be called
 */
struct SharedData
{
    double alpha;
    bool threadFinished;
    std::vector<Process> processesToRun;
    SharedData(double alphaVal, std::vector<Process> processes) : alpha(alphaVal), threadFinished(false), processesToRun(processes) {}
};

#endif
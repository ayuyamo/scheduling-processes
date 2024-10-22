/**
 * Name: Quynh Anh Do (or Halie Do)
 * RedID: 827707836
 */
#ifndef PROCESS_H
#define PROCESS_H

#include <vector>

#define FIRST_CPU_BURST -1

class Process
{
private:
    int processID;
    std::vector<int> remainingBursts;
    int numOfBursts;
    int previousCPUBurst;
    double previousCPUPrediction;
    std::vector<double> predictions; // keeping track of all predictions for use later
    int currIOTimeLeft;
    int executedCPUBursts;
    int executedIOBursts;
    int waitTime;
    int turnaroundTime;

public:
    Process(int id, std::vector<int> bursts, int burstCount);
    void updateCPUExecuted();
    void updateIOExecuted();
    void addCurrBurstToPredictions(double currBurst);
    int getTotalExecutedCPUBursts();
    int getTotalExecutedIOBursts();
    int getTurnaroundTime();
    int getTotalWaitTime();
    int getNumOfBursts();
    std::vector<int> getBursts();
    std::vector<double> getEstimatedBursts();
    void initiateIOBurst(int burstTime);
    void updateIOTimeRemaining();
    void calcTurnaroundTime();
    void updateExponetialAveragingVal(double alpha);
    double getPrediction();
    double getCPUBurstAvg();
    bool hasRemainingBursts();
    int getProcessID();
    int getRemainingIOTime();
    void runProcess();
    int getCurrBurst();
    void goToNextBurst();
    void updateWaitTime();
};

#endif
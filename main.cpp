/**
 * Name: Quynh Anh Do (or Halie Do)
 * RedID: 827707836
 */

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sstream>
#include "log.h"
#include "scheduler.h"
#include "process.h"
#include "shared.h"
#define NORMAL_EXIT 0
#define BAD_EXIT 1

// receive the vector of processes and pass on to run() in scheduler
void *runScheduler(void *ptr)
{
    SharedData *data = (SharedData *)ptr;
    std::vector<Process> processes = data->processesToRun;

    // print initial information of processes to run in scheduler
    for (Process &p : processes)
    {
        int numOfBursts = p.getNumOfBursts();
        std::vector<int> burstsVector = p.getBursts();
        unsigned int bursts[numOfBursts];
        for (int i = 0; i < numOfBursts; i++)
        {
            bursts[i] = static_cast<unsigned int>(burstsVector[i]);
        }
        log_process_bursts(p.getProcessID(), bursts, numOfBursts);
    }

    Scheduler scheduler(processes, data->alpha);
    data->threadFinished = scheduler.runProcess(); // once scheduler is done, change threadFinished to true
    pthread_exit(NORMAL_EXIT);                     // exit worker thread
}

int main(int argc, char *argv[])
{
    if (argc < 2) // if only name of executable is provided, exit with error
    {
        std::cerr << "Usage: " << argv[0] << " <process_file_path> [-a <alpha_value> (optional)]" << std::endl;
        return BAD_EXIT;
    }

    int option;                 // holds the string option that the user inputs, "a:" means option is "-a"
    double alphaVal = NO_ALPHA; // initialize alphaVal with -1 if no alpha value was provided
    while ((option = getopt(argc, argv, "a:")) != -1)
    {
        switch (option)
        {
        case 'a':
            alphaVal = std::stod(optarg);       // extract the argument provided after specifying the option flag
            if (alphaVal <= 0 || alphaVal >= 1) // invalid argument for alpha value
            {
                std::cerr << "Alpha for exponential averaging must be within (0.0, 1.0)" << std::endl;
                return BAD_EXIT;
            }
            break;
        default: // if user provides unknown option
            std::cerr << "Unknown option: " << static_cast<char>(optopt) << std::endl;
            return BAD_EXIT;
        }
    }

    std::string burstFile = argv[optind]; // optind is index pointing to the next argument after the one above
    std::ifstream inputFile(burstFile);

    if (!inputFile.is_open())
    {
        std::cerr << "Error: Unable to open <<" << burstFile << ">>\"" << std::endl;
        return BAD_EXIT;
    }
    // Read from file and intialize processes from the given info
    std::string line;
    int numOfBursts = 0;            // hold numbers read on each line
    int index = 0;                  // index of processes
    int currBurstNum;               // current number being read from file
    std::vector<int> bursts;        // array holding numbers read in each line
    std::vector<Process> processes; // holds the processes initialized from provided info

    while (std::getline(inputFile, line))
    {
        std::istringstream iss(line);
        while (iss >> currBurstNum) // read each number on the current line
        {
            if (currBurstNum <= 0)
            {
                std::cerr << "A burst number must be bigger than 0" << std::endl;
                return BAD_EXIT;
            }
            bursts.push_back(currBurstNum);
            numOfBursts++;
        }
        if (numOfBursts % 2 == 0) // if # of bursts is even
        {
            std::cerr << "There must be an odd number of bursts for each process" << std::endl;
            return BAD_EXIT;
        }

        Process p(index, bursts, numOfBursts);
        processes.push_back(p);

        index++;
        numOfBursts = 0; // reset number of bursts to zero to count # of digits on second line
        bursts.clear();  // clear array to hold burst values in the next line
    }

    inputFile.close(); // close file after all information has been read

    SharedData shared(alphaVal, processes);
    // sample pthread code
    pthread_attr_t pthread_attr_default;            // holds thread attributes
    pthread_t thread1;                              // reference to thread
    pthread_attr_init(&pthread_attr_default);       // intialize threads with default attributes
    pthread_create(&thread1, &pthread_attr_default, // create worker thread
                   &runScheduler, (void *)&shared);

    while (!shared.threadFinished) // blocks main thread with busy waiting until worker thread is finished
    {
        continue;
    }

    return NORMAL_EXIT;
}
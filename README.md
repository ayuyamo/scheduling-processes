# Process Scheduler Simulation

## Overview

- This program simulates a process scheduler that reads and extracts information about CPU bursts of processes. It accepts an optional argument, `-a`, which specifies the alpha value used for calculating exponential averages. This value also influences the order in which processes are executed. The necessary information is utilized to initialize a `SharedData` class object, which is then passed to a separate thread responsible for scheduling and executing processes. The main program waits for all processes to complete before terminating.

## Command Line Arguments

The executable requires one mandatory command-line argument and accepts one optional argument:

**Mandatory Argument**

- `bursts.txt`: This is the input file containing process burst information used for scheduling.

**Optional Argument**

- `-a <alpha>`: This argument specifies the alpha value for exponential averaging in CPU burst estimations. If provided, it determines how the next CPU burst is estimated based on previous bursts.

**Examples**

**Using Exponential Averaging**:

```
./schedule bursts.txt -a 0.2
```

- This command uses the process bursts from the bursts.txt file for `Shortest Job Next (SJN)` scheduling.
- Exponential averaging is applied with an alpha value of `0.2` for CPU burst estimations.
- The estimated CPU bursts will dictate the order in which the next CPU burst is executed.
- The program will simulate the execution time of each CPU burst based on the input file.

**Without Exponential Averaging**:

```
./schedule bursts.txt
```

- This command also utilizes the process bursts from the `bursts.txt` file for SJN scheduling.
- Since the alpha value is not specified, exponential averaging is not used for CPU burst estimations.
- The program will use the CPU bursts from the input file to determine the order of execution and simulate how long each CPU burst will run.

## Functionality

### Process Initialization

Upon starting, all processes are placed in the Ready queue. The first process admitted to the system is located at the front of this queue. The scheduler continuously checks the status of both the Ready and Blocked queues.
Scheduling Strategy

The program implements the `Shortest Job Next (SJN)` scheduling strategy, which operates as follows:

- **Queue Management**:

  - Each time a process is selected for execution from the `Ready` queue, the queue is stable-sorted based on the estimated or actual next CPU burst in ascending order.
  - If the `Ready` queue is not empty, the process at the front of the queue is chosen for execution.

- **CPU Burst Execution**:

  - Increment the executed CPU bursts for the selected process by the amount of the current CPU burst.
  - Update the total elapsed time by the amount of the executed CPU burst to track completion time for each process.
  - While executing the current CPU burst, all processes in the Blocked queue simultaneously execute their respective I/O bursts.

- **Blocked Process Management**:

  - For each `blocked` process:
    - Check if its current I/O burst completes during the executing process's CPU burst. If it does, move the blocked process to the end of the `Ready` queue. The Ready queue may be re-sorted by the next CPU burst at this point.
    - If the I/O burst is not completed, update the I/O burst by subtracting the executed CPU burst amount.
      Increment the count of executed I/O bursts for display purposes.

- **CPU Burst Completion**:
  - The CPU burst execution stops for one of the following reasons:
    - **Current CPU Burst Completed**: The process enters its next I/O burst. The process is moved to the end of the Blocked queue, and the Blocked queue is stable-sorted by the first remaining I/O burst.
    - **All Bursts Completed**: The process is appended to the list of completed processes.

### Queue Status Handling

If the `Ready` queue is empty but the `Blocked` queue contains processes:

- Iterate through the `Blocked` queue. For each `blocked` process:
  - Execute its current I/O burst.
  - Update the count of executed I/O bursts.
  - Move the process to the end of the Ready queue if its I/O burst finishes.
- Increment the total elapsed time by the front I/O burst of the first process in the `Blocked` queue, ensuring that at least one process becomes available for execution in the Ready queue.

If both the Ready and `Blocked` queues are empty, the simulation concludes.

## Conclusion

This simulation provides a basic understanding of process scheduling using the Shortest Job Next strategy. It manages the execution of processes and their transitions between `Ready` and `Blocked` states, demonstrating key scheduling principles.

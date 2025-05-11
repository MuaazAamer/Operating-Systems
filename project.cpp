#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <cmath>
#include <queue>

using namespace std;

// Process Class
class process {
    int id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int priority;
    int cpu_time;

    public:

    // Overloaded Constructor
    process(int _id, int _arr, int _burst) {
        if(_arr < 0 || _burst < 0 || _id < 0)
            throw invalid_argument("Invalid Arguments");
        id = _id;
        arrival_time = _arr;
        burst_time = _burst;
        waiting_time = 0;
        priority = 0;
        cpu_time = 0;
    }

    // Second Constructor
    process(int _id) {
        id = _id;
        arrival_time = 0;
        burst_time = 0;
        waiting_time = 0;
        priority = 0;
        cpu_time = 0;
    }

    // Third Constructor
    process(int _id, int _arr, int _burst, int _priority) {
        if(_arr < 0 || _burst < 0 || _id < 0 || _priority < 0)
            throw invalid_argument("Invalid Arguments");
        id = _id;
        arrival_time = _arr;
        burst_time = _burst;
        waiting_time = 0;
        priority = _priority;
        cpu_time = 0;
    }

    // Getters
    int get_id() { return id; }
    int get_arrival_time() { return arrival_time; }
    int get_burst_time() { return burst_time; }
    int get_waiting_time() { return waiting_time; }
    int get_priority() { return priority; }
    int get_cpu_time() { return cpu_time; }

    // Setters
    void set_priority(int _priority) {
        if(_priority < 0)
            throw invalid_argument("Invalid Priority");
        priority = _priority;
    }

    // Functions for process execution
    void increment_waiting_time() { waiting_time++; }
    void decrement_burst_time() { burst_time--; }
    void increment_cpu_time() { cpu_time++; }
    void reset_cpu_time() { cpu_time = 0; }
    void reset_waiting_time() { waiting_time = 0; }

    // Calculate feedback ratio
    double feedback_ratio() {
        return (waiting_time + (double)burst_time) / burst_time;
    }
};

void LongestJobFirst()
{
    int noOfProcesses;

    cout << "Enter the number of processes: ";
    cin >> noOfProcesses;

    process** processes = new process*[noOfProcesses];

    for(int i = 0; i < noOfProcesses; i++)
    {
        int arrival_time, burst_time;
        cout << "Enter the arrival time and burst time of process " << i + 1 << ": ";
        cin >> arrival_time >> burst_time;

        processes[i] = new process(i + 1, arrival_time, burst_time);
    }

    int executed = 0;
    int time = 0;
    vector<process*> ready_queue;
    process* CPU = NULL;

    while(executed < noOfProcesses)
    {
        
        for(int i = 0; i < noOfProcesses; i++)
        {
            if(processes[i]->get_arrival_time() == time)
                ready_queue.push_back(processes[i]);
        }

        if(CPU != NULL)
        {
            CPU->decrement_burst_time();

            if(CPU->get_burst_time() == 0)
            {
                executed++;
                delete CPU;
                CPU = NULL;
            }
        }

        if(CPU == NULL)
        {
            if(ready_queue.size() > 0)
            {
                int max = 0;
                int index = 0;

                for(int i = 0; i < ready_queue.size(); i++)
                {
                    if(ready_queue[i]->get_burst_time() > max)
                    {
                        max = ready_queue[i]->get_burst_time();
                        index = i;
                    }
                }

                CPU = ready_queue[index];
                ready_queue.erase(ready_queue.begin() + index);
            }
        }

        cout << "Time: " << time << endl;
        if(CPU == NULL)
            cout << "CPU is idle" << endl;
        else
            cout << "CPU: " << CPU->get_id() << endl;
        cout << "Ready Queue: ";
        for(int i = 0; i < ready_queue.size(); i++)
            cout << ready_queue[i]->get_id() << " ";
        cout << endl << "------------------------------------------------\n";

        time++;
        for(int i = 0; i < ready_queue.size(); i++)
            ready_queue[i]->increment_waiting_time();
    }

    delete[] processes;
}

void MultiLevelQueue()
{
    int noOfProcesses, timeSlice;

    cout << "Enter the number of processes: ";
    cin >> noOfProcesses;
    cout << "Emter the time slice for Round Robin Queue: ";
    cin >> timeSlice;

    process** processes = new process*[noOfProcesses];
    vector<int> allocatedQueue(noOfProcesses);

    for(int i = 0; i < noOfProcesses; i++)
    {
        int arrival_time, burst_time, priority, allocated_queue;
        cout << "Enter the arrival time, burst time, and priorty of process " << i + 1 << ": ";
        cin >> arrival_time >> burst_time >> priority;

        processes[i] = new process(i + 1, arrival_time, burst_time, priority);
    }

    for(int i = 0; i < noOfProcesses; i++)
    {
        int queue;
        cout << "Enter the queue number for process " << i + 1 << ": ";
        cin >> queue;

        if(queue < 1 || queue > 3)
        {
            cout << "Invalid Queue Number" << endl;
            return;
        }
        allocatedQueue[i] = queue - 1;
    }

    int executed = 0;
    int time = 0;
    vector<process*> ready_queue[3];
    // ready_queue[0] = Priority Queue
    // ready_queue[1] = Round Robin Queue
    // ready_queue[2] = Shortest Remaining Job First Queue
    process* CPU = NULL;
    int timeSliceCounter = 0;

    while(executed < noOfProcesses)
    {
        
        for(int i = 0; i < noOfProcesses; i++)
        {
            if(processes[i]->get_arrival_time() == time)
                ready_queue[allocatedQueue[i]].push_back(processes[i]);
        }

        if(CPU != NULL)
        {
            int queueOfCrrCPUprocess = allocatedQueue[CPU->get_id() - 1];
            CPU->decrement_burst_time();

            int queueTurn = 0;
            while(ready_queue[queueTurn].size() == 0)
                queueTurn++;

            if(CPU->get_burst_time() == 0)
            {
                executed++;
                delete CPU;
                CPU = NULL;
            }
            // else if(queueOfCrrCPUprocess == 0)
            // {
            //     for(int i = 0; i < ready_queue[0].size(); i++)
            //     {
            //         if(ready_queue[0][i]->get_priority() < CPU->get_priority())
            //         {
            //             ready_queue[0].push_back(CPU);
            //             delete CPU;
            //             CPU = NULL;
            //             break;
            //         }
            //     }
            // }
            else if(queueTurn < queueOfCrrCPUprocess)
            {
                ready_queue[queueOfCrrCPUprocess].push_back(CPU);
                CPU = NULL;
            }
            else if(queueOfCrrCPUprocess == 1)
            {
                timeSliceCounter++;
                if(timeSliceCounter == timeSlice)
                {
                    ready_queue[1].insert(ready_queue[1].begin(), CPU);
                    CPU = NULL;
                    timeSliceCounter = 0;
                }
            }
            else if(queueOfCrrCPUprocess == 2)
            {
                for(int i = 0; i < ready_queue[2].size(); i++)
                {
                    if(ready_queue[2][i]->get_burst_time() < CPU->get_burst_time())
                    {
                        ready_queue[2].push_back(CPU);
                        CPU = NULL;
                        break;
                    }
                }
            }
        }

        if(CPU == NULL)
        {
            int queueTurn = 0;
            while(ready_queue[queueTurn].size() == 0)
                queueTurn++;
            if(queueTurn == 0)
            {
                int min = ready_queue[0][0]->get_priority();
                int index = 0;

                for(int i = 1; i < ready_queue[0].size(); i++)
                {
                    if(ready_queue[0][i]->get_priority() < min)
                    {
                        min = ready_queue[0][i]->get_priority();
                        index = i;
                    }
                }

                CPU = ready_queue[0][index];
                ready_queue[0].erase(ready_queue[0].begin() + index);
            }
            else if(queueTurn == 1)
            {
                CPU = ready_queue[1][0];
                ready_queue[1].erase(ready_queue[1].begin());
                
            }
            else if(queueTurn == 2)
            {
                if(ready_queue[2].size() > 0)
                {
                    int min = ready_queue[2][0]->get_burst_time();
                    int index = 0;

                    for(int i = 1; i < ready_queue[2].size(); i++)
                    {
                        if(ready_queue[2][i]->get_burst_time() < min)
                        {
                            min = ready_queue[2][i]->get_burst_time();
                            index = i;
                        }
                    }

                    CPU = ready_queue[2][index];
                    ready_queue[2].erase(ready_queue[2].begin() + index);
                }
            }
        }

        int queueOfCrrCPUprocess = -1;
        if(CPU != NULL)
            queueOfCrrCPUprocess = allocatedQueue[CPU->get_id() - 1];
        cout << "Time: " << time << endl;
        if(CPU == NULL)
            cout << "CPU is idle" << endl;
        else
            cout << "CPU: " << CPU->get_id() << endl;
        cout << "Ready Queues:\n";

        for(int i = 0; i < 3; i++)
        {
            cout << "Queue " << i + 1 << ": ";
            for(int j = 0; j < ready_queue[i].size(); j++)
                cout << ready_queue[i][j]->get_id() << " ";
            if(i == queueOfCrrCPUprocess)
                cout << " <--";
            cout << endl;
        }
        cout << endl << "------------------------------------------------\n";

        time++;
        for(int j = 0; j < 3; j++)
            for(int i = 0; i < ready_queue[j].size(); i++)
                ready_queue[j][i]->increment_waiting_time();
    }

    delete[] processes;
}

void MultiLevelFeedbackQueue()
{
    int noOfProcesses, timeSlice, waitingTimeLimit, CPUtimeLimit;

    // Get the number of processes, time slice, waiting time limit and CPU time limit
    cout << "Enter the number of processes: ";
    cin >> noOfProcesses;
    cout << "Emter the time slice for Round Robin Queue: ";
    cin >> timeSlice;
    cout << "Enter the waiting time limit to promote the processes: ";
    cin >> waitingTimeLimit;
    cout << "Enter CPU time limit to demote the processes: ";
    cin >> CPUtimeLimit;

    process** processes = new process*[noOfProcesses];
    vector<int> allocatedQueue(noOfProcesses);
    
    // Get the arrival time and burst time for each process
    int arrival_time, burst_time;
    for(int i = 0; i < noOfProcesses; i++)
    {
        cout << "Enter the arrival time and burst time of process " << i + 1 << ": ";
        cin >> arrival_time >> burst_time;

        processes[i] = new process(i + 1, arrival_time, burst_time);
    }

    // 1 = First Come First Serve Queue
    // 2 = Shortest Job Queue
    // 3 = Round Robin Queue
    // Get the queue number for each process
    for(int i = 0; i < noOfProcesses; i++)
        allocatedQueue[i] = 0;

    int executed = 0;
    int time = 0;
    vector<process*> ready_queue[3];
    // ready_queue[0] = First Come First Serve Queue
    // ready_queue[1] = Shortest Job Queue
    // ready_queue[2] = Round Robin Queue
    process* CPU = NULL;
    int timeSliceCounter = 0;

    while(executed < noOfProcesses)
    {
        
        for(int i = 0; i < noOfProcesses; i++)
        {
            if(processes[i]->get_arrival_time() == time)
                ready_queue[allocatedQueue[i]].push_back(processes[i]);
        }

        // Promotion
        for(int i = 1; i <= 2; i++)
        {
            for(int j = 0; j < ready_queue[i].size(); j++)
            {
                if(ready_queue[i][j]->get_waiting_time() >= waitingTimeLimit)
                {
                    ready_queue[i][j]->reset_cpu_time();
                    ready_queue[i][j]->reset_waiting_time();
                    allocatedQueue[ready_queue[i][j]->get_id() - 1] = i - 1;
                    ready_queue[i - 1].push_back(ready_queue[i][j]);
                    ready_queue[i].erase(ready_queue[i].begin() + j);
                    j--;
                }
            }
        }

        // Demotion
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < ready_queue[i].size(); j++)
            {
                if(ready_queue[i][j]->get_cpu_time() >= CPUtimeLimit)
                {
                    ready_queue[i][j]->reset_cpu_time();
                    ready_queue[i][j]->reset_waiting_time();
                    allocatedQueue[ready_queue[i][j]->get_id() - 1] = i + 1;
                    ready_queue[i + 1].push_back(ready_queue[i][j]);
                    ready_queue[i].erase(ready_queue[i].begin() + j);
                    j--;
                }
            }
        }

        // Making CPU Free
        if(CPU != NULL)
        {
            // Getting the queue number of the current process in CPU
            int queueOfCrrCPUprocess = allocatedQueue[CPU->get_id() - 1];
            // Decrementing the burst time of the process
            CPU->decrement_burst_time();    
            CPU->increment_cpu_time();

            int queueTurn = 0;
            while(ready_queue[queueTurn].size() == 0)
                queueTurn++;

            if(CPU->get_burst_time() == 0) // Process is completed
            {
                executed++;
                delete CPU;
                CPU = NULL;
            }
            else if(queueTurn < queueOfCrrCPUprocess) // Higher priority process is available
            {
                ready_queue[queueOfCrrCPUprocess].push_back(CPU);
                CPU = NULL;
            }
            else if(queueOfCrrCPUprocess == 2) // Time slice is over for the Round Robin Queue (third queue)
            {
                timeSliceCounter++;
                if(timeSliceCounter == timeSlice)
                {
                    ready_queue[2].insert(ready_queue[2].begin() + 1, CPU);
                    CPU = NULL;
                    timeSliceCounter = 0;
                }
            }
            else if(CPU->get_cpu_time() >= CPUtimeLimit) // CPU time limit is over for the process
            {
                CPU->reset_cpu_time();
                CPU->reset_waiting_time();
                ready_queue[queueOfCrrCPUprocess + 1].push_back(CPU);
                allocatedQueue[CPU->get_id() - 1] = queueOfCrrCPUprocess + 1;
                CPU = NULL;
            }
        }

        // Selecting a process for CPU
        if(CPU == NULL)
        {
            // Checking from which queue to select the process
            int queueTurn = 0;
            while(ready_queue[queueTurn].size() == 0)
                queueTurn++;
            if(queueTurn == 0) // Selecting from First Come First Serve Queue (first queue)
            {
                CPU = ready_queue[0][0];
                ready_queue[0].erase(ready_queue[0].begin());
            }
            else if(queueTurn == 1) // Selecting from Shortest Job Queue (second queue)
            {
                int minBurstTime = ready_queue[1][0]->get_burst_time();
                int minBurstTimeIndex = 0;
                for(int i = 1; i < ready_queue[1].size(); i++)
                {
                    if(ready_queue[1][i]->get_burst_time() < minBurstTime)
                    {
                        minBurstTime = ready_queue[1][i]->get_burst_time();
                        minBurstTimeIndex = i;
                    }
                }
                CPU = ready_queue[1][minBurstTimeIndex];
                ready_queue[1].erase(ready_queue[1].begin() + minBurstTimeIndex);
            }
            else if(queueTurn == 2) // Selecting from Round Robin Queue (third queue)
            {
                CPU = ready_queue[2][0];
                ready_queue[2].erase(ready_queue[2].begin());
            }
        }
        
        // Printing the status
        int queueOfCrrCPUprocess = -1;
        if(CPU != NULL)
            queueOfCrrCPUprocess = allocatedQueue[CPU->get_id() - 1];
        cout << "Time: " << time << endl;
        if(CPU == NULL)
            cout << "CPU is idle" << endl;
        else
            cout << "CPU: " << CPU->get_id() << endl;
        cout << "Ready Queues:\n";

        for(int i = 0; i < 3; i++)
        {
            cout << "Queue " << i + 1 << ": ";
            for(int j = 0; j < ready_queue[i].size(); j++)
                cout << ready_queue[i][j]->get_id() << " ";
            if(i == queueOfCrrCPUprocess)
                cout << " <--";
            cout << endl;
        }
        cout << endl << "------------------------------------------------\n";

        time++;
        for(int j = 0; j < 3; j++)
            for(int i = 0; i < ready_queue[j].size(); i++)
                ready_queue[j][i]->increment_waiting_time();
    }

    delete[] processes;
}

void LowestFeedbackRatioNext()
{
    int noOfProcesses;

    cout << " -- Lowest Feedback Ratio Next Scheduling -- \n";

    cout << "Enter the number of processes: ";
    cin >> noOfProcesses;

    process** processes = new process*[noOfProcesses];

    for(int i = 0; i < noOfProcesses; i++)
    {
        int arrival_time, burst_time;
        cout << "Enter the arrival time and burst time of process " << i + 1 << ": ";
        cin >> arrival_time >> burst_time;

        processes[i] = new process(i + 1, arrival_time, burst_time);
    }

    int executed = 0;
    int time = 0;
    vector<process*> ready_queue;
    process* CPU = NULL;

    while(executed < noOfProcesses)
    {
        
        for(int i = 0; i < noOfProcesses; i++)
        {
            if(processes[i]->get_arrival_time() == time)
                ready_queue.push_back(processes[i]);
        }

        if(CPU != NULL)
        {
            CPU->decrement_burst_time();

            if(CPU->get_burst_time() == 0)
            {
                executed++;
                delete CPU;
                CPU = NULL;
            }
        }

        if(CPU == NULL)
        {
            if(ready_queue.size() > 0)
            {
                double feedback;
                double min = ready_queue[0]->feedback_ratio();
                int index = 0;

                for(int i = 1; i < ready_queue.size(); i++)
                {
                    feedback = ready_queue[i]->feedback_ratio();
                    if(feedback < min)
                    {
                        min = feedback;
                        index = i;
                    }
                }

                CPU = ready_queue[index];
                ready_queue.erase(ready_queue.begin() + index);
            }
        }

        cout << "Time: " << time << endl;
        if(CPU == NULL)
            cout << "CPU is idle" << endl;
        else
            cout << "CPU: " << CPU->get_id() << endl;
        cout << "Ready Queue: ";
        for(int i = 0; i < ready_queue.size(); i++)
            cout << ready_queue[i]->get_id() << " ";
        cout << endl << "------------------------------------------------\n";

        time++;
        for(int i = 0; i < ready_queue.size(); i++)
            ready_queue[i]->increment_waiting_time();
    }

    delete[] processes;
}

bool safetyAlgorithm()
{
    int noOfProcesses, noOfResources;

    cout << "Enter the number of processes: ";
    cin >> noOfProcesses;

    cout << "Enter the number of resources: ";
    cin >> noOfResources;

    vector<int> available(noOfResources);
    vector<vector<int>> max(noOfProcesses, vector<int>(noOfResources));
    vector<vector<int>> allocation(noOfProcesses, vector<int>(noOfResources));

    cout << "Enter the maximum resources of each process: \n";
    for(int i = 0; i < noOfProcesses; i++)
    {
        cout << "For P" << i << ": ";
        for(int j = 0; j < noOfResources; j++)
            cin >> max[i][j];
    }

    cout << "Enter the allocated resources of each process: \n";
    for(int i = 0; i < noOfProcesses; i++)
    {
        cout << "For P" << i << ": ";
        for(int j = 0; j < noOfResources; j++)
            cin >> allocation[i][j];
    }

    cout << "Enter the available resources: ";
    for(int i = 0; i < noOfResources; i++)
        cin >> available[i];

    vector<vector<int>> need(noOfProcesses, vector<int>(noOfResources));
    vector<int> work(available);

    cout << "Need Matrix: \n";
    for(int i = 0; i < noOfProcesses; i++)
    {
        cout << "For P" << i << ": ";
        for(int j = 0; j < noOfResources; j++)
        {
            need[i][j] = max[i][j] - allocation[i][j];
            cout << need[i][j] << " ";
        }
        cout << endl;
    }

    queue<int> safeSequence;
    int missCount = 0;
    int executed = 0;
    int currentProcess = 0;
    vector<bool> isExecuted(noOfProcesses, false);

    while(executed < noOfProcesses && missCount < noOfProcesses - executed)
    {
        if(isExecuted[currentProcess])
        {
            currentProcess = (currentProcess + 1) % noOfProcesses;
            continue;
        }

        bool isResourceAvailable = true;

        for(int i = 0; i < noOfResources; i++)
        {
            if(need[currentProcess][i] > work[i] && !isExecuted[currentProcess])
            {
                isResourceAvailable = false;
                break;
            }
        }

        if(isResourceAvailable)
        {
            safeSequence.push(currentProcess);
            isExecuted[currentProcess] = true;
            executed++;

            for(int i = 0; i < noOfResources; i++)
                work[i] += allocation[currentProcess][i];

            missCount = 0;
        }
        else
            missCount++;
        
        currentProcess = (currentProcess + 1) % noOfProcesses;
    }

    if(executed == noOfProcesses)
    {
        cout << "Safe Sequence: ";
        while(!safeSequence.empty())
        {
            cout << "P" << safeSequence.front() << " ";
            safeSequence.pop();
        }
        cout << endl;
        return true;
    }
    else
    {
        cout << "No safe sequence exists\n";
        return false;
    }
}

bool resourceRequestAlgorithm()
{
    int noOfProcesses, noOfResources, requestingProcess;

    cout << "Enter the number of processes: ";
    cin >> noOfProcesses;

    cout << "Enter the number of resources: ";
    cin >> noOfResources;

    vector<int> available(noOfResources);
    vector<vector<int>> max(noOfProcesses, vector<int>(noOfResources));
    vector<vector<int>> allocation(noOfProcesses, vector<int>(noOfResources));
    vector<int> request(noOfResources);

    cout << "Enter the maximum resources of each process: \n";
    for(int i = 0; i < noOfProcesses; i++)
    {
        cout << "For P" << i << ": ";
        for(int j = 0; j < noOfResources; j++)
            cin >> max[i][j];
    }

    cout << "Enter the allocated resources of each process: \n";
    for(int i = 0; i < noOfProcesses; i++)
    {
        cout << "For P" << i << ": ";
        for(int j = 0; j < noOfResources; j++)
        {
            cin >> allocation[i][j];
            if(allocation[i][j] > max[i][j])
            {
                cout << "Invalid allocation\n";
                return false;
            }
        }
    }

    cout << "Enter the available resources: ";
    for(int i = 0; i < noOfResources; i++)
        cin >> available[i];

    cout << "Enter the process number who is requesting: ";
    cin >> requestingProcess;

    if(requestingProcess >= noOfProcesses || requestingProcess < 0)
    {
        cout << "Invalid process number\n";
        return false;
    }

    cout << "Enter the number of resources requested: ";
    for(int i = 0; i < noOfResources; i++)
        cin >> request[i];

    vector<vector<int>> need(noOfProcesses, vector<int>(noOfResources));
    vector<int> work(available);

    cout << "Need Matrix: \n";
    for(int i = 0; i < noOfProcesses; i++)
    {
        cout << "For P" << i << ": ";
        for(int j = 0; j < noOfResources; j++)
        {
            need[i][j] = max[i][j] - allocation[i][j];
            cout << need[i][j] << " ";
        }
        cout << endl;
    }

    queue<int> safeSequence;
    int missCount = 0;
    int executed = 0;
    int currentProcess = 0;
    vector<bool> isExecuted(noOfProcesses, false);
    bool requestProcessed = false;


    for(int i = 0; i < noOfResources; i++)
    {
        if(request[i] > need[currentProcess][i])
        {
            cout << "Request cannot be processed because no of requested resources are more than the need of the process!\n";
            return false;
        }
    }

    while(executed < noOfProcesses && missCount < noOfProcesses - executed)
    {
        if(requestProcessed == false)
        {
            bool isRecourceAvailable = true;

            for(int i = 0 ; i < noOfResources; i++)
                if(request[i] > work[i])
                    isRecourceAvailable = false;

            if(isRecourceAvailable)
            {
                for(int i = 0; i < noOfResources; i++)
                {
                    work[i] -= request[i];
                    allocation[requestingProcess][i] += request[i];
                    need[requestingProcess][i] -= request[i];
                }

                requestProcessed = true;
                cout << "Request processed successfully!\n";
            }
        }

        if(isExecuted[currentProcess])
        {
            currentProcess = (currentProcess + 1) % noOfProcesses;
            continue;
        }

        bool isResourceAvailable = true;
        for(int i = 0; i < noOfResources; i++)
        {
            if(need[currentProcess][i] > work[i] && !isExecuted[currentProcess])
            {
                isResourceAvailable = false;
                break;
            }
        }

        if(isResourceAvailable)
        {
            safeSequence.push(currentProcess);
            isExecuted[currentProcess] = true;
            executed++;

            for(int i = 0; i < noOfResources; i++)
                work[i] += allocation[currentProcess][i];

            missCount = 0;
        }
        else
            missCount++;
        
        currentProcess = (currentProcess + 1) % noOfProcesses;
    }

    if(executed == noOfProcesses)
    {
        cout << "Safe Sequence: ";
        while(!safeSequence.empty())
        {
            cout << "P" << safeSequence.front() << " ";
            safeSequence.pop();
        }
        cout << endl;
        return true;
    }
    else
    {
        cout << "No safe sequence exists\n";
        cout << "Request Denied!\n";
        return false;
    }
}

sem_t chopstick[5];
sem_t diningTable;

void *philosopher(void *pos)
{
    int i = *(int *)pos;
    while (true)
    {
        printf("Philosopher %d is thinking\n", i + 1);
        sleep(rand() % 10);

        sem_wait(&diningTable);
            sem_wait(&chopstick[i]);
                sem_wait(&chopstick[(i + 1) % 5]);

                printf("Philosopher %d is eating\n", i + 1);
                sleep(rand() % 10);

                sem_post(&chopstick[(i + 1) % 5]);
            sem_post(&chopstick[i]);
        sem_post(&diningTable);
    }
}

void philosophersDiningProblem()
{
    sem_init(&diningTable, 0, 4);
    for (int i = 0; i < 5; i++)
        sem_init(&chopstick[i], 0, 1);

    pthread_t philosopheres[5];
    int val[] = {0, 1, 2, 3, 4};

    pthread_create(&philosopheres[0], NULL, &philosopher, (void *)&val[0]);
    pthread_create(&philosopheres[1], NULL, &philosopher, (void *)&val[1]);
    pthread_create(&philosopheres[2], NULL, &philosopher, (void *)&val[2]);
    pthread_create(&philosopheres[3], NULL, &philosopher, (void *)&val[3]);
    pthread_create(&philosopheres[4], NULL, &philosopher, (void *)&val[4]);

    for (int i = 0; i < 5; i++)
        pthread_join(philosopheres[i], NULL);
}

template <typename T>
class circularList{

    class node
    {
        public:
        T data;
        node *next;
        bool isDummy;

        node(T data, node* next = NULL)
        {
            this->data = data;
            this->next = next;
            this->isDummy = false;
        }
    };
public:
    class iterator
    {
        node *ptr;

        public:
        iterator(node *ptr)
        {
            this->ptr = ptr;
        }

        iterator operator++(int)
        {
            ptr = ptr->next;
            return *this;
        }

        T& operator*()
        {
            return ptr->data;
        }

        bool operator!=(const iterator &other)
        {
            return this->ptr != other.ptr;
        }

        bool operator==(const iterator &other)
        {
            return this->ptr == other.ptr;
        }
    };

private:
    node *head;
    int size;

    public:
    circularList()
    {
        head = new node(T());
        head->next = head;
        head->isDummy = true;
        size = 0;
    }

    circularList(int _size, T data)
    {
        head = new node(T());
        head->next = head;
        head->isDummy = true;
        size = 0;

        for(int i = 0; i < _size; i++)
            insert(data);
    }

    void insert(T data)
    {
        node *newNode = new node(data, head->next);
        head->next = newNode;
        size++;
    }

    void remove(T data)
    {
        node *temp = head;
        while(temp->next->data != data)
            temp = temp->next;

        if(temp->next == head)
            return;
        
        node *toDelete = temp->next;
        temp->next = temp->next->next;
        delete toDelete;
        size--;
    }

    iterator begin()
    {
        return iterator(head->next);
    }

    iterator end()
    {
        return iterator(head);
    }

    int getSize()
    {
        return size;
    }

    iterator search(int data)
    {
        node *temp = head->next;
        while(temp->data != data && !temp->isDummy)
            temp = temp->next;
        return iterator(temp);
    }
    
    ~circularList()
    {
        node *temp = head->next;
        while(temp != head)
        {
            node *toDelete = temp;
            temp = temp->next;
            delete toDelete;
        }
        delete head;
    }
};

circularList<pair<int, bool>>::iterator searchPageInRAM(circularList<pair<int, bool>> RAM, int page)
{
    for(auto it = RAM.begin(); it != RAM.end(); it++)
        if((*it).first == page)
            return it;
    return RAM.end();
}

void printRAM(circularList<pair<int, bool>> RAM, circularList<pair<int, bool>>::iterator crrPointer)
{
    for(int i = 0; i < RAM.getSize(); i++)
        cout << "  _ ";
    cout << endl;

    for(auto it = RAM.begin(); it != RAM.end(); it++)
        if((*it).first != -1)
            cout << "| " << (*it).first << " ";
        else
            cout << "|   ";
    cout << "|" << endl;

    for(int i = 0; i < RAM.getSize(); i++)
        cout << "  _ ";
    cout << endl;

    for(auto it = RAM.begin(); it != RAM.end(); it++)
        if((*it).second)
            cout << "| 1 ";
        else
            cout << "| 0 ";
    cout << "|" << endl;

    for(int i = 0; i < RAM.getSize(); i++)
        cout << "  _ ";
    cout << endl;
    
    for(auto it = RAM.begin(); it != RAM.end(); it++)
        if(it == crrPointer)
            cout << "  ^ ";
        else
            cout << "    ";
    cout << endl;

    for(auto it = RAM.begin(); it != RAM.end(); it++)
        if(it == crrPointer)
            cout << "  | ";
        else
            cout << "    ";
    cout << endl;

    for(int i = 0; i < RAM.getSize(); i++)
        cout << "-----";
    cout << endl;
}

void SecondChancePageReplacement()
{
    int noOfFrames, noOfPages;
    cout << "Enter the number of frames in RAM: ";
    cin >> noOfFrames;
    cout << "Enter the number of pages to reference: ";
    cin >> noOfPages;

    vector<int> pageInsertionSeq(noOfPages);
    cout << "Enter the page access sequence: ";
    for(int i = 0; i < noOfPages; i++)
        cin >> pageInsertionSeq[i];

    circularList<pair<int, bool>> RAM(noOfFrames, make_pair(-1, false));

    int pageFaults = 0;
    circularList<pair<int, bool>>::iterator crrPointer = RAM.begin();

    printRAM(RAM, crrPointer);

    for(int i = 0; i < noOfPages; i++)
    {
        circularList<pair<int, bool>>::iterator index = searchPageInRAM(RAM, pageInsertionSeq[i]);
        if(index == RAM.end())
        {
            pageFaults++;
            while((*crrPointer).second == true)
            {
                (*crrPointer).second = false;

                crrPointer++;
                if(crrPointer == RAM.end())
                    crrPointer = RAM.begin();
            }
            (*crrPointer).first = pageInsertionSeq[i];
            (*crrPointer).second = true;
                
            crrPointer++;
            if(crrPointer == RAM.end())
                crrPointer = RAM.begin();
        }
        else
            (*index).second = true;
        cout << "Page Accessed: " << pageInsertionSeq[i];
        if(index == RAM.end())
            cout << " (Page Fault)" << endl;
        else
            cout << " (Page Hit)" << endl;
        printRAM(RAM, crrPointer);
    }
}

void HashedPageTable()
{
    int sizeOfRAM, sizeOfFrame;
    cout << "Enter the size of RAM: 2^";
    cin >> sizeOfRAM;
    sizeOfRAM = pow(2, sizeOfRAM);

    cout << "Enter the size of each frame: 2^";
    cin >> sizeOfFrame;
    sizeOfFrame = pow(2, sizeOfFrame);

    int noOfFrames = sizeOfRAM / sizeOfFrame;
    cout << "Number of frames in RAM: " << noOfFrames << endl;

    vector<vector<int>> TLB(64, vector<int>(2, -1));
    // 0 is page number
    // 1 is frame number

    unordered_map<int, int> pageTable;
    // key is page number
    // value is frame number

    vector<int> RAM(noOfFrames, -1);
    // index is frame number
    // value is page number

    int choice;
    bool exit = false;

    while(!exit)
    {
        cout << "\n--------------------------------------------------------------\n";
        cout << "1. Access a page" << endl;
        cout << "2. Print Page Table" << endl;
        cout << "3. Print TLB" << endl;
        cout << "4. Print RAM" << endl;
        cout << "5. Exit" << endl;

        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                int page;
                cout << "Enter the logical address: ";
                cin >> page;

                int pageNumber = page / sizeOfFrame;
                int offset = page % sizeOfFrame;

                cout << endl;
                cout << "Page Number: " << pageNumber << endl;
                cout << "Offset: " << offset << endl;

                int frameNumber = -1;

                cout << endl;
                if(TLB[pageNumber % 64][0] == pageNumber)
                {
                    cout << "TLB Hit" << endl;
                    frameNumber = TLB[pageNumber % 64][1];

                    cout << endl;
                    cout << "Frame Number: " << frameNumber << endl;
                    cout << "Physical Address: " << frameNumber * sizeOfFrame + offset << endl;
                }
                else
                {
                    cout << "TLB Fault" << endl;
                    if(pageTable.find(pageNumber) != pageTable.end())
                    {
                        cout << "Page Hit" << endl;
                        frameNumber = pageTable[pageNumber];

                        cout << endl;
                        cout << "Frame Number: " << frameNumber << endl;
                        cout << "Physical Address: " << frameNumber * sizeOfFrame + offset << endl;
                    }
                    else
                    {
                        cout << "Page Fault" << endl;
                        
                        bool isEmptyFrameFound;
                        frameNumber = rand() % noOfFrames;
                        if(RAM[frameNumber] != -1)
                        {
                            isEmptyFrameFound = false;
                            for(int i = frameNumber + 1, collisions = 0; collisions < RAM.size(); i = (i + 1) % RAM.size(), collisions++)
                            {
                                if(RAM[i] == -1)
                                {
                                    frameNumber = i;
                                    RAM[frameNumber] = pageNumber;
                                    isEmptyFrameFound = true;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            RAM[frameNumber] = pageNumber;
                            isEmptyFrameFound = true;
                        }

                        if(!isEmptyFrameFound)
                        {
                            cout << "RAM is FULL" << endl;
                            break;
                        }
                        
                        pageTable[pageNumber] = frameNumber;

                        cout << endl;
                        cout << "Page Number " << pageNumber;
                        cout << " is stored on Frame Number " << frameNumber << endl;
                        cout << "Physical Address: " << frameNumber * sizeOfFrame + offset << endl;
                    }

                    TLB[pageNumber % 64][0] = pageNumber;
                    TLB[pageNumber % 64][1] = frameNumber;
                }

                break;
            }
            case 2:
            {
                cout << "| Page Number | Frame Number |" << endl;
                for(auto it = pageTable.begin(); it != pageTable.end(); it++)
                    cout << "| " << it->first << "\t|\t" << it->second << " |" << endl;
                break;
            }
            case 3:
            {
                cout << "| Page Number | Frame Number |" << endl;
                for(int i = 0; i < 64; i++)
                    if(TLB[i][0] != -1)
                        cout << "| " << TLB[i][0] << "\t|\t" << TLB[i][1] << " |" << endl;
                break;
            }
            case 4:
            {
                cout << "| Frame Number | Page Number |" << endl;
                for(int i = 0; i < noOfFrames; i++)
                    if(RAM[i] != -1)
                        cout << "| " << i << "\t|\t" << RAM[i] << " |" << endl;
                break;
            }
            case 5:
            {
                exit = true;
                break;
            }
            default:
            {
                cout << "Invalid Choice" << endl;
                break;
            }
        }
    }
}

void TwoLevelPageTable()
{
    int sizeOfRAM, sizeOfFrame, pageNumberLimit, maxLogicalAddress;
    cout << "Enter the size of RAM: 2^";
    cin >> sizeOfRAM;
    sizeOfRAM = pow(2, sizeOfRAM);

    cout << "Enter the size of each frame: 2^";
    cin >> sizeOfFrame;
    sizeOfFrame = pow(2, sizeOfFrame);

    cout << "Enter the maximum logical address: 2^";
    cin >> maxLogicalAddress;

    pageNumberLimit = maxLogicalAddress - log2(sizeOfFrame);

    int noOfFrames = sizeOfRAM / sizeOfFrame;
    int outerPageTableSize = floor(double(pageNumberLimit) / 2);
    int innerPageTableSize = ceil(double(pageNumberLimit) / 2);

    cout << "Number of frames in RAM: " << noOfFrames << endl;
    cout << "Size of Outer Page Table: " << pow(2, outerPageTableSize) << endl;
    cout << "Size of Inner Page Table: " << pow(2, innerPageTableSize) << endl;

    vector<vector<int>*> outerPageTable(pow(2, outerPageTableSize), NULL);
    // index is page number / 2^innerPageTable

    vector<int> RAM(noOfFrames, -1);
    // index is frame number
    // value is page number

    int choice;
    bool exit = false;

    while(!exit)
    {
        cout << "\n--------------------------------------------------------------\n";
        cout << "1. Access a page" << endl;
        cout << "2. Print Page Table" << endl;
        cout << "3. Print RAM" << endl;
        cout << "4. Exit" << endl;

        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                int address;
                cout << "Enter the logical address: ";
                cin >> address;

                while(address / sizeOfFrame >= pow(2, pageNumberLimit))
                {
                    cout << "Invalid Page Number" << endl;
                    cout << "Enter the logical address: ";
                    cin >> address;
                }

                int pageNumber = address / sizeOfFrame;
                int offset = address % sizeOfFrame;

                cout << endl;
                cout << "Page Number: " << pageNumber << endl;
                cout << "Offset: " << offset << endl;

                int frameNumber = -1;
                int outerTableIndex = pageNumber / pow(2, innerPageTableSize);
                int innerTableIndex = pageNumber % int(pow(2, innerPageTableSize));

                if(outerPageTable[outerTableIndex] == NULL)
                {
                    cout << endl << "Page Fault" << endl;

                    outerPageTable[outerTableIndex] = new vector<int>(pow(2, innerPageTableSize), -1);
                    
                    bool isEmptyFrameFound;
                    frameNumber = rand() % noOfFrames;
                    if(RAM[frameNumber] != -1)
                    {
                        isEmptyFrameFound = false;
                        for(int i = frameNumber + 1, collisions = 0; collisions < RAM.size(); i = (i + 1) % RAM.size(), collisions++)
                        {
                            if(RAM[i] == -1)
                            {
                                frameNumber = i;
                                RAM[frameNumber] = pageNumber;
                                isEmptyFrameFound = true;
                                break;
                            }
                        }
                    }
                    else
                    {
                        RAM[frameNumber] = pageNumber;
                        isEmptyFrameFound = true;
                    }

                    if(!isEmptyFrameFound)
                    {
                        cout << endl << "RAM is FULL" << endl;
                        break;
                    }

                    (*outerPageTable[outerTableIndex])[innerTableIndex] = frameNumber;
                    cout << endl << "Page Number " << pageNumber;
                    cout << " is stored on Frame Number " << frameNumber << endl;
                    cout << "Outer Page Table Index: " << outerTableIndex << endl;
                    cout << "Inner Page Table Index: " << innerTableIndex << endl;
                    cout << "Physical Address: " << frameNumber * sizeOfFrame + offset << endl;
                }
                else if((*outerPageTable[outerTableIndex])[innerTableIndex] == -1)
                {
                    cout << endl << "Page Fault" << endl;

                    bool isEmptyFrameFound;
                    frameNumber = rand() % noOfFrames;
                    if(RAM[frameNumber] != -1)
                    {
                        isEmptyFrameFound = false;
                        for(int i = frameNumber + 1, collisions = 0; collisions < RAM.size(); i = (i + 1) % RAM.size(), collisions++)
                        {
                            if(RAM[i] == -1)
                            {
                                frameNumber = i;
                                RAM[frameNumber] = pageNumber;
                                isEmptyFrameFound = true;
                                break;
                            }
                        }
                    }
                    else
                    {
                        RAM[frameNumber] = pageNumber;
                        isEmptyFrameFound = true;
                    }

                    if(!isEmptyFrameFound)
                    {
                        cout << endl << "RAM is FULL" << endl;
                        break;
                    }

                    (*outerPageTable[outerTableIndex])[innerTableIndex] = frameNumber;
                    cout << endl << "Page Number " << pageNumber;
                    cout << " is stored on Frame Number " << frameNumber << endl;
                    cout << "Outer Page Table Index: " << outerTableIndex << endl;
                    cout << "Inner Page Table Index: " << innerTableIndex << endl;
                    cout << "Physical Address: " << frameNumber * sizeOfFrame + offset << endl;
                }
                else
                {
                    frameNumber = (*outerPageTable[outerTableIndex])[innerTableIndex];
                    cout << endl << "Page Hit" << endl << endl;
                    cout << "Page Number: " << pageNumber << endl;
                    cout << "Frame Number: " << frameNumber << endl;
                    cout << "Outer Page Table Index: " << outerTableIndex << endl;
                    cout << "Inner Page Table Index: " << innerTableIndex << endl;
                    cout << "Physical Address: " << frameNumber * sizeOfFrame + offset << endl;
                }


                break;
            }
            case 2:
            {
                cout << endl << "\tOuter Page Table" << endl;
                cout << "-------------------------------" << endl;
                cout << "Index\t|\tValue" << endl;
                cout << "-------------------------------" << endl;
                for(int i = 0; i < outerPageTable.size(); i++)
                {
                    cout << i << "\t|\t";
                    if(outerPageTable[i] == NULL)
                        cout << "NULL" << endl;
                    else
                        cout << "Table No " << i << endl;
                }
                cout << "-------------------------------" << endl;

                cout << "\n\tInner Page Tables" << endl;
                cout << "-------------------------------" << endl;
                for(int i = 0; i < outerPageTable.size(); i++)
                {
                    if(outerPageTable[i] != NULL)
                    {
                        cout << endl << "Inner Page Table No " << i << endl;
                        cout << "-------------------------------" << endl;
                        cout << "Index\t|\tValue" << endl;
                        cout << "-------------------------------" << endl;
                        for(int j = 0; j < (*outerPageTable[i]).size(); j++)
                        {
                            cout << j << "\t|\t";
                            if((*outerPageTable[i])[j] == -1)
                                cout << " " << endl;
                            else
                                cout << (*outerPageTable[i])[j] << endl;
                        }
                        cout << "-------------------------------" << endl;
                    }
                }
                break;
            }
            case 3:
            {
                cout << "\t\tRAM" << endl;
                cout << "-------------------------------" << endl;
                cout << "Frame Number\t|\tPage Number" << endl;
                cout << "-------------------------------" << endl;
                for(int i = 0; i < RAM.size(); i++)
                {
                    cout << i << "\t\t|\t";
                    if(RAM[i] == -1)
                        cout << " " << endl;
                    else
                        cout << RAM[i] << endl;
                }
                cout << "-------------------------------" << endl;
                break;
            }
            case 4:
            {
                exit = true;
                break;
            }
            default:
            {
                cout << "Invalid Choice" << endl;
                break;
            }
        }
    }
}
int main()
{
    int choice = 0;
    int option = 0;
    while (option!=5)
    {
        cout << "\t*****WELCOME TO OPERATING SYSYTEM SIMULATOR*****" << endl;
        cout << "\t\t1:CPU Scheduling" << endl;
        cout << "\t\t2:Semaphore" << endl;
        cout << "\t\t3:DeadLock" << endl;
        cout << "\t\t4:Memory Management" << endl;
        cout << "\t\t5:Exit" << endl;
        cout << "\t\tEnter an Option:";
        cin >> option;
        while (option <1 || option>5)
        {
            //sleep(5);
            //system("CLS");
            cout << "\t*****WELCOME TO OPERATING SYSYTEM SIMULATOR*****" << endl;
            cout << "\t\t1:CPU Scheduling" << endl;
            cout << "\t\t2:Semaphore" << endl;
            cout << "\t\t3:DeadLock" << endl;
            cout << "\t\t4:Memory Management" << endl;
            cout << "\t\tEnter an Option:";
            cin >> option;
        }
        if (option == 1)
        {
            //system("CLS");
            cout << "\t***************************" << endl;
            cout << "\t\t1:Longest Job First" << endl;
            cout << "\t\t2:Multi Level Queue" << endl;
            cout << "\t\t3:Multi Level Feedback Queue" << endl;
            cout << "\t\t4:Lowest Feedback Ratio Next" << endl;
            cout << "\t\tEnter choice to run the Algorithm:";
            cin >> choice;
            while (choice < 1 || choice>4)
            {
                //system("CLS");
                //sleep(5);
               cout << "\t***************************" << endl;
                cout << "\t\t1:Longest Job First" << endl;
                cout << "\t\t2:Multi Level Queue" << endl;
                cout << "\t\t3:Multi Level Feedback Queue" << endl;
                cout << "\t\t4:Lowest Feedback Ratio Next" << endl;
                cout << "\t\tEnter choice to run the Algorithm:";
                cin >> choice;
            }
            cout << endl << endl;
            if (choice == 1)
            {
                LongestJobFirst();
            }
            else if (choice == 2)
            {
                MultiLevelQueue();
            }
            else if (choice == 3)
            {
                MultiLevelFeedbackQueue();
            }
            else if (choice == 4)
            {
                LowestFeedbackRatioNext();
            }
        }
        else if (option == 2)
        {
            //system("CLS");
            cout << "\t***************************" << endl;
            cout << "\t\t1:Philosopher Dinning Problem" << endl;
            cout << "\t\tEnter choice to run the Algorithm:";
            cin >> choice;
            while (choice != 1)
            {
                //system("CLS");
                //sleep(5);
                cout << "\t***************************" << endl;
                cout << "\t\t1:Philosopher Dinning Problem" << endl;
                cout << "\t\tEnter choice to run the Algorithm:";
                cin >> choice;
            }
            cout << endl << endl;
            philosophersDiningProblem();
        }
        else if (option == 3)
        {
           //system("CLS");
           cout << "\t***************************" << endl; 
           cout<<"\t\t1:Safety Algorithm"<<endl;
           cout<<"\t\t2:Resource Request Algorithm"<<endl;
           cout << "\t\tEnter choice to run the Algorithm:";
           cin >> choice;
           while (choice < 1 || choice>3)
            {
                //sleep(5);
                //system("CLS");
               cout << "\t***************************" << endl;
                cout<<"\t\t1:Safety Algorithm"<<endl;
                cout<<"\t\t2:Resource Request Algorithm"<<endl;
                cout << "\t\tEnter choice to run the Algorithm:";
                cin >> choice;
            }
            if(choice==1)
            {
              bool c=safetyAlgorithm();
            }
            else
            {
               bool c=resourceRequestAlgorithm();
            }
        }
        else if (option == 4)
        {
            //system("CLS");
            cout << "\t***************************" << endl;
            cout << "\t\t1:Second Chance Page Replacement" << endl;
            cout << "\t\t2:Hashed Paged Table" << endl;
            cout << "\t\t3:Two Level Page Table" << endl;
            cout << "\t\tEnter choice to run the Algorithm:";
            cin >> choice;
            while (choice < 1 || choice>3)
            {
                //sleep(5);
                //system("CLS");
                 cout << "\t***************************" << endl;
                cout << "\t\t1:Second Chance Page Replacement" << endl;
                cout << "\t\t2:Hashed Paged Table" << endl;
                cout << "\t\t3:Two Level Page Table" << endl;
                cout << "\t\tEnter choice to run the Algorithm:";
                cin >> choice;
            }
            cout << endl << endl;
            if (choice == 1)
            {
                SecondChancePageReplacement();
            }
            else if (choice == 2)
            {
                HashedPageTable();
            }
            else
            {
                TwoLevelPageTable();
            }
        }
        sleep(10);
        //system("CLS");
    }
}

/*
 * CSE4502 Operating Systems Lab: 1A
 * Task: Preemptive SJF Scheduling Algorithm (SRTF - Shortest Remaining Time First)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <climits>

using namespace std;

struct Process {
    int id;
    int at;        // Arrival Time
    int bt;        // Burst Time
    int ct;        // Completion Time
    int tat;       // Turnaround Time
    int wt;        // Waiting Time
    int remaining; // Remaining Time for preemption
};

void preemptiveSJF(vector<Process>& processes) {
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;
    int totalTime = 0;

    // Calculate total burst time to know when to stop
    for (auto& p : processes) totalTime += p.bt;

    while (completed < n) {
        // Find process with shortest remaining time among arrived processes
        int shortest = -1;
        int minRemaining = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (processes[i].at <= currentTime && processes[i].remaining > 0) {
                if (processes[i].remaining < minRemaining ||
                    (processes[i].remaining == minRemaining &&
                     processes[i].at < processes[shortest].at)) {
                    minRemaining = processes[i].remaining;
                    shortest = i;
                }
            }
        }

        if (shortest == -1) {
            // No process available, CPU idle
            currentTime++;
            continue;
        }

        // Execute for 1 time unit
        processes[shortest].remaining--;
        currentTime++;

        // Check if completed
        if (processes[shortest].remaining == 0) {
            processes[shortest].ct  = currentTime;
            processes[shortest].tat = processes[shortest].ct - processes[shortest].at;
            processes[shortest].wt  = processes[shortest].tat - processes[shortest].bt;
            completed++;
        }
    }
}

int main() {
    vector<Process> processes = {
        {1, 0, 6, 0, 0, 0, 6},
        {2, 2, 4, 0, 0, 0, 4},
        {3, 4, 2, 0, 0, 0, 2},
        {4, 6, 5, 0, 0, 0, 5}
    };

    preemptiveSJF(processes);

    // Print results table
    cout << left
         << setw(6)  << "ID"
         << setw(6)  << "AT"
         << setw(6)  << "BT"
         << setw(6)  << "CT"
         << setw(6)  << "TAT"
         << setw(6)  << "WT"
         << endl;
    cout << string(36, '-') << endl;

    double avgTAT = 0, avgWT = 0;
    for (auto& p : processes) {
        cout << left
             << setw(6) << p.id
             << setw(6) << p.at
             << setw(6) << p.bt
             << setw(6) << p.ct
             << setw(6) << p.tat
             << setw(6) << p.wt
             << endl;
        avgTAT += p.tat;
        avgWT  += p.wt;
    }

    int n = processes.size();
    cout << fixed << setprecision(2);
    cout << "\nAverage Turnaround Time: " << avgTAT / n << endl;
    cout << "Average Waiting Time:    " << avgWT  / n << endl;

    return 0;
}

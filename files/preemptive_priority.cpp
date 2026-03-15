/*
 * CSE4502 Operating Systems Lab: 2A
 * Task: Preemptive Priority Scheduling Algorithm
 * (Lower priority number = Higher priority)
 */

#include <iostream>
#include <vector>
#include <climits>
#include <iomanip>

using namespace std;

struct Process {
    int pid;
    int at;         // Arrival Time
    int bt;         // Burst Time
    int priority;   // Lower number = higher priority
    int rt;         // Remaining Time
    int ct;         // Completion Time
    int tat;        // Turnaround Time
    int wt;         // Waiting Time
    bool completed;
};

void preemptivePriorityScheduling(vector<Process>& p) {
    int n = p.size();
    int completed = 0;
    int currentTime = 0;

    while (completed < n) {
        // Find arrived, incomplete process with highest priority (lowest number)
        int selected = -1;
        int bestPriority = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!p[i].completed && p[i].at <= currentTime) {
                if (p[i].priority < bestPriority ||
                    (p[i].priority == bestPriority && p[i].at < p[selected].at)) {
                    bestPriority = p[i].priority;
                    selected = i;
                }
            }
        }

        if (selected == -1) {
            currentTime++; // CPU idle
            continue;
        }

        // Execute 1 time unit
        p[selected].rt--;
        currentTime++;

        if (p[selected].rt == 0) {
            p[selected].completed = true;
            p[selected].ct  = currentTime;
            p[selected].tat = p[selected].ct - p[selected].at;
            p[selected].wt  = p[selected].tat - p[selected].bt;
            completed++;
        }
    }
}

int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> processes(n);

    for (int i = 0; i < n; i++) {
        processes[i].pid       = i + 1;
        processes[i].completed = false;
        cout << "Enter Arrival Time, Burst Time, and Priority for Process "
             << processes[i].pid << ": ";
        cin >> processes[i].at >> processes[i].bt >> processes[i].priority;
        processes[i].rt = processes[i].bt;
    }

    preemptivePriorityScheduling(processes);

    cout << "\n" << left
         << setw(6)  << "PID"
         << setw(6)  << "AT"
         << setw(6)  << "BT"
         << setw(10) << "Priority"
         << setw(6)  << "CT"
         << setw(6)  << "TAT"
         << setw(6)  << "WT"
         << endl;
    cout << string(46, '-') << endl;

    double avgTAT = 0, avgWT = 0;
    for (auto& p : processes) {
        cout << left
             << setw(6)  << p.pid
             << setw(6)  << p.at
             << setw(6)  << p.bt
             << setw(10) << p.priority
             << setw(6)  << p.ct
             << setw(6)  << p.tat
             << setw(6)  << p.wt
             << endl;
        avgTAT += p.tat;
        avgWT  += p.wt;
    }

    cout << fixed << setprecision(2);
    cout << "\nAverage Turnaround Time: " << avgTAT / n << endl;
    cout << "Average Waiting Time:    " << avgWT  / n << endl;

    return 0;
}

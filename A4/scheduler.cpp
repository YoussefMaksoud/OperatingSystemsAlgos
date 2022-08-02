// you need to modify this file

#include "scheduler.h"
#include "common.h"
#include <iostream>

using namespace std;

// this is the function you should edit
//
// runs Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrival_times, and bursts
// output:
//   seq[] - will contain the execution sequence but trimmed to max_seq_len size
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//
void simulate_rr(
    int64_t quantum, int64_t max_seq_len, vector<Process> & processes, vector<int> & seq)
{

    // replace the wrong implementation below with your own!!!!
    seq.clear();

    vector<int> cpu; // integer represents time spent in cpu
    vector<int> rq;
    vector<Process> copy = processes;
    vector<int> done;
    vector<int> runtime;

    int idle = 1;

    for (uint64_t i = 0; i < processes.size(); i++) { runtime.push_back(0); }

    int jobs_remaining = processes.size();

    int64_t clk = 0;
    int start_cpu = 0;

    // while there are still processes waiting to use cpu
    while (1) {

        // break if there are no more jobs to do
        if (jobs_remaining == 0) { break; }

        if (idle == 1) { // cpu not occupied
            // if new process is arriving add new proces to rq

            if (copy[0].arrival_time == clk && ! copy.empty()) {
                rq.push_back(copy[0].id);
                copy.erase(copy.begin());
                cout << "\narriving";
            }
            if (! rq.empty()) {
                cout << "\nassigning";
                cpu.push_back(rq[0]);
                if (processes[cpu[0]].start_time == -1) { processes[cpu[0]].start_time = clk; }
                rq.erase(rq.begin());
                start_cpu = 0;
                idle = 0;
                int end = *(seq.end() - 1);
                if (seq.size() < max_seq_len && end != cpu[0]) { seq.push_back(cpu[0]); }
            } else if (rq.empty() && ! copy.empty() && ((copy[0].arrival_time - clk) > 100)) {
                cout << "\narriving quick";
                rq.push_back(copy[0].id);
                clk = copy[0].arrival_time - 1;
                copy.erase(copy.begin());
            } else {
                int end = *(seq.end() - 1);
                if (seq.size() < max_seq_len && end != -1) seq.push_back(-1);
            }
        }

        if (idle == 0) { // cpu occupied

            if (runtime[cpu[0]] == processes[cpu[0]].burst) {
                cout << "\ndone";
                done.push_back(cpu[0]);
                processes[cpu[0]].finish_time = clk;
                idle = 1;
                jobs_remaining--;
                cpu.clear();
                continue;
            }

            if (start_cpu == quantum && runtime[cpu[0]] < processes[cpu[0]].burst
                && jobs_remaining > 1) {
                cout << "\nslice up";
                rq.push_back(cpu[0]);
                idle = 1;
                cpu.clear();
                continue;

            } else if (jobs_remaining == 1) {
                cout << "\nlast job fast";
                clk = clk + (processes[cpu[0]].burst - runtime[cpu[0]]);
                processes[cpu[0]].finish_time = clk;
                return;
            }

            if (copy[0].arrival_time == clk && ! copy.empty()) {
                cout << "\narriving while running";
                rq.push_back(copy[0].id);
                copy.erase(copy.begin());
            }

            start_cpu++;
            runtime[cpu[0]]++;
        }

        clk++;

//attempt at handling difficult optimization
        /*if (runtime.empty()) {
            clk = copy[0].arrival_time - 1;
            return;
        }
        cout << "running  quick";
        int64_t pivot;
        int64_t set_change = 0;
        for (int i = 0; i < rq.size(); i++) {
            pivot = (processes[rq[i]].burst - runtime[i]) % quantum;
            if (! copy.empty() && (pivot < set_change)
                && (copy[0].arrival_time > (clk + (pivot * quantum)))) {
                set_change = pivot;
            }
        }

        for (int i = 0; i < runtime.size(); i++) { runtime[i] += (set_change * quantum); }
        clk = clk + (set_change * quantum);
    }
    cout << "hitting nothing";*/
    }

        
}
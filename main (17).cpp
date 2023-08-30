#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

// Enumeration to represent the state of a process
enum State {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED,
    OUTPUT
};

// Struct to represent a process request
struct Request {
    string type;
    int duration;
    int time_left;
    int lock_id;

    Request(string t, int d, int l) {
        type = t;
        duration = d;
        time_left = d;
        lock_id = l;
    }
};

// Struct to represent a process
struct Process {
    int sequence;
    int start_time;
    int end_time;
    vector<Request> requests;
    int current_request;
    State state;

    Process(int s, int t) {
        sequence = s;
        start_time = t;
        end_time = -1;
        current_request = 0;
        state = READY;
    }

    void add_request(string type, int duration, int lock_id = -1) {
        requests.push_back(Request(type, duration, lock_id));
    }
};

int main() {
    int ncores;
    int curr_time = 0;
    int process_sequence = 0;
    int terminated_processes = 0;
    int busy_cores = 0;
    int output_time = 0;
    vector<int> core_pid;
    vector<int> locks;
    vector<Process> processes;
    queue<int> ready_queue;

    // Read input from file
    ifstream input_file("input13.txt");
    string line;
    while (getline(input_file, line)) {
        istringstream ss(line);
        string keyword;
        ss >> keyword;
        if (keyword == "NCORES") {
            ss >> ncores;
            core_pid.resize(ncores, -1);
            locks.resize(64, 0);
        } else if (keyword == "START") {
            int start_time;
            ss >> start_time;
            processes.push_back(Process(process_sequence++, start_time));
        } else if (keyword == "CPU" || keyword == "SSD") {
            int duration;
            ss >> duration;
            processes.back().add_request(keyword, duration);
        } else if (keyword == "LOCK") {
            int lock_number;
            ss >> lock_number;
            processes.back().add_request(keyword, 0, lock_number);
        } else if (keyword == "UNLOCK") {
            int lock_number;
            ss >> lock_number;
            processes.back().add_request(keyword, 0, lock_number);
        } else if (keyword == "OUTPUT") {
            int duration;
            ss >> duration;
            processes.back().add_request(keyword, duration);
        } else if (keyword == "END") {
            processes.back().add_request(keyword, 0);
        }
    }
    input_file.close();

    // Print header of process table
    cout << "Time | Process | Busy Cores | Ready Queue | Process Table" << endl;

    // Loop until all processes are terminated
    while (terminated_processes < processes.size()) {
        // Add any new processes that start at the current time to the ready queue
        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].start_time == curr_time && processes[i].state == READY) {
                ready_queue.push(processes[i].sequence);
                processes[i].state = BLOCKED;
                cout << curr_time << " | Process " << processes[i].sequence << " starts | ";
                cout << busy_cores << " | ";
                queue<int> temp_queue = ready_queue;
                while (!temp_queue.empty()) {
                    cout << temp_queue.front() << " ";
                    temp_queue.pop();
                }
                cout << "| ";
                for (int j = 0; j < processes.size(); j++) {
                    Process p = processes[j];
                    if (p.start_time <= curr_time && p.end_time == -1) {
                        string state_str;
                        if (p.state == RUNNING) {
                            state_str = "RUNNING";
                        } else if (p.state == BLOCKED) {
                            state_str = "BLOCKED";
                        } else if (p.state == OUTPUT) {
                            state_str = "OUTPUT";
                        } else {
                            state_str = "READY";
                        }
                        cout << "P" << p.sequence << ": " << state_str << " ";
                        for (int k = 0; k < p.requests.size(); k++) {
                            Request r = p.requests[k];
                            if (r.type == "LOCK" || r.type == "UNLOCK") {
                                cout << r.type << " " << r.lock_id << " ";
                            } else if (r.type == "OUTPUT") {
                                cout << r.type << " " << r.duration << " ";
                            } else if (r.type != "END") {
                                cout << r.type << " " << r.duration << "ms ";
                            }
                        }
                        cout << "| ";
                    }
                }
                cout << endl;
            }
        }
// Update state of each process
for (int i = 0; i < processes.size(); i++) {
Process &p = processes[i];
if (p.end_time == -1) {
if (p.state == BLOCKED) {
if (p.requests[p.current_request].type == "LOCK") {
int lock_id = p.requests[p.current_request].lock_id;
if (locks[lock_id] == 0) {
locks[lock_id] = p.sequence;
p.state = RUNNING;
busy_cores++;
core_pid[busy_cores-1] = p.sequence;
p.requests[p.current_request].time_left--;
cout << curr_time << " | Process " << p.sequence << " running on core " << busy_cores-1 << " | ";
cout << busy_cores << " | ";
queue<int> temp_queue = ready_queue;
while (!temp_queue.empty()) {
cout << temp_queue.front() << " ";
temp_queue.pop();
}
cout << "| ";
for (int j = 0; j < processes.size(); j++) {
Process p = processes[j];
if (p.start_time <= curr_time && p.end_time == -1) {
string state_str;
if (p.state == RUNNING) {
state_str = "RUNNING";
} else if (p.state == BLOCKED) {
state_str = "BLOCKED";
} else if (p.state == OUTPUT) {
state_str = "OUTPUT";
} else {
state_str = "READY";
}
cout << "P" << p.sequence << ": " << state_str << " ";
for (int k = 0; k < p.requests.size(); k++) {
Request r = p.requests[k];
if (r.type == "LOCK" || r.type == "UNLOCK") {
cout << r.type << " " << r.lock_id << " ";
} else if (r.type == "OUTPUT") {
cout << r.type << " " << r.duration << " ";
} else if (r.type != "END") {
cout << r.type << " " << r.duration << "ms ";
}
}
cout << "| ";
}
}
cout << endl;
} else {
p.state = BLOCKED;
p.requests[p.current_request].time_left--;
}
} else {
p.state = RUNNING;
busy_cores++;
core_pid[busy_cores-1] = p.sequence;
p.requests[p.current_request].time_left--;
cout << curr_time << " | Process " << p.sequence << " running on core " << busy_cores-1 << " | ";
cout << busy_cores << " | ";
queue<int> temp_queue = ready_queue;
while (!temp_queue.empty()) {
cout << temp_queue.front() << " ";
temp_queue.pop();
}
cout << "| ";
for (int j = 0; j < processes.size(); j++) {
Process p = processes[j];
if (p.start_time <= curr_time && p.end_time == -1) {
string state_str;
if (p.state == RUNNING) {
state_str = "RUNNING";
} else if (p.state == BLOCKED) {
state_str = "BLOCKED";
} else if (p.state == OUTPUT) {
state_str = "OUTPUT";
} else {
state_str = "READY";
}
cout << "P" << p.sequence << ": " << state_str << " ";
for (int k = 0; k < p.requests.size(); k++) {
Request r = p.requests[k];
if (r.type == "LOCK" || r.type == "UNLOCK") {
cout << r.type << " " << r.lock_id << " ";
} else if (r.type == "OUTPUT") {
cout << r.type << " " << r.duration << " ";
} else if (r.type != "END") {
cout << r.type << " " << r.duration << "ms ";
}
}
cout << "| ";
}
}
cout << endl;
} }
else if (p.state == OUTPUT) {
p.requests[p.current_request].time_left--;
if (p.requests[p.current_request].time_left == 0) {
p.state = READY;
ready_queue.push(p.sequence);
}
} else if (p.state == READY) {
p.requests[p.current_request].time_left--;
if (p.requests[p.current_request].time_left == 0) {
p.current_request++;
if (p.current_request == p.requests.size()) {
p.state = BLOCKED;
p.end_time = curr_time;
busy_cores--;
} else {
if (p.requests[p.current_request].type == "OUTPUT") {
p.state = OUTPUT;
} else if (p.requests[p.current_request].type == "LOCK") {
int lock_id = p.requests[p.current_request].lock_id;
if (locks[lock_id] == 0) {
locks[lock_id] = p.sequence;
p.state = RUNNING;
busy_cores++;
core_pid[busy_cores-1] = p.sequence;
cout << curr_time << " | Process " << p.sequence << " running on core " << busy_cores-1 << " | ";
cout << busy_cores << " | ";
queue<int> temp_queue = ready_queue;
while (!temp_queue.empty()) {
cout << temp_queue.front() << " ";
temp_queue.pop();
}
cout << "| ";
for (int j = 0; j < processes.size(); j++) {
Process p = processes[j];
if (p.start_time <= curr_time && p.end_time == -1) {
string state_str;
if (p.state == RUNNING) {
state_str = "RUNNING";
} else if (p.state == BLOCKED) {
state_str = "BLOCKED";
} else if (p.state == OUTPUT) {
state_str = "OUTPUT";
} else {
state_str = "READY";
}
cout << "P" << p.sequence << ": " << state_str << " ";
for (int k = 0; k < p.requests.size(); k++) {
Request r = p.requests[k];
if (r.type == "LOCK" || r.type == "UNLOCK") {
cout << r.type << " " << r.lock_id << " ";
} else if (r.type == "OUTPUT") {
cout << r.type << " " << r.duration << " ";
} else if (r.type != "END") {
cout << r.type << " " << r.duration << "ms ";
}
}
cout << "| ";
}
}
cout << endl;
} else {
p.state = BLOCKED;
}
} else {
p.state = RUNNING;
busy_cores++;
core_pid[busy_cores-1] = p.sequence;
cout << curr_time << " | Process " << p.sequence << " running on core " << busy_cores-1 << " | ";
cout << busy_cores << " | ";
queue<int> temp_queue = ready_queue;
while (!temp_queue.empty()) {
cout << temp_queue.front() << " ";
temp_queue.pop();
}
cout << "| ";
for (int j = 0; j < processes.size(); j++) {
Process p = processes[j];
if (p.start_time <= curr_time && p.end_time == -1) {
string state_str;
if (p.state == RUNNING) {
state_str = "RUNNING";
} else if (p.state == BLOCKED) {
state_str = "BLOCKED";
} else if (p.state == OUTPUT) {
state_str = "OUTPUT";
} else {
state_str = "READY";
}
cout << "P" << p.sequence << ": " << state_str << " ";
for (int k = 0; k < p.requests.size(); k++) {
Request r = p.requests[k];
if (r.type == "LOCK" || r.type == "UNLOCK") {
cout << r.type << " " << r.lock_id << " ";
} else if (r.type == "OUTPUT") {
cout << r.type << " " << r.duration << " ";
} else if (r.type != "END") {
cout << r.type << " " << r.duration << "ms ";
}
}
cout << "| ";
}
}
cout << endl;
}
}
} else if (p.state == BLOCKED) {
// do nothing
}
}
curr_time++;
}

// print final statistics
int total_wait_time = 0;
int total_turnaround_time = 0;
for (int i = 0; i < processes.size(); i++) {
Process p = processes[i];
int wait_time = p.start_time - p.start_time;
int turnaround_time = p.end_time - p.start_time;
total_wait_time += wait_time;
total_turnaround_time += turnaround_time;
cout << "Process " << p.sequence << ": ";
cout << "arrival time = " << p.start_time << ", ";
cout << "start time = " << p.start_time << ", ";
cout << "end time = " << p.end_time << ", ";
cout << "wait time = " << wait_time << ", ";
cout << "turnaround time = " << turnaround_time << endl;
}
cout << "Total wait time = " << total_wait_time << endl;
cout << "Total turnaround time = " << total_turnaround_time << endl;
cout << "Average wait time = " << (double) total_wait_time / processes.size() << endl;
cout << "Average turnaround time = " << (double) total_turnaround_time / processes.size() << endl;

return 0;
}}}
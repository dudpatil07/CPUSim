#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <queue>
#include <sstream>

using namespace std;

class CacheLevel {
private:
    string name;
    size_t capacity;
    deque<int> recentMem;
    unordered_map<int, bool> memLookup;

public:
    int latency;

    CacheLevel(string cname, size_t size, int lat)
        : name(cname), capacity(size), latency(lat) {}

    string getName() const { return name; }

    bool check(int memIndex) {
        return memLookup.find(memIndex) != memLookup.end();
    }

    int addMemory(int memIndex) {
        if (check(memIndex)) return -1;
        int evicted = -1;
        if (recentMem.size() >= capacity) {
            evicted = recentMem.front();
            memLookup.erase(evicted);
            recentMem.pop_front();
        }
        recentMem.push_back(memIndex);
        memLookup[memIndex] = true;
        return evicted;
    }

    void printCache() const {
        cout << "  " << name << ": [";
        for (size_t i = 0; i < recentMem.size(); i++) {
            cout << "M" << recentMem[i];
            if (i != recentMem.size() - 1) cout << ", ";
        }
        cout << "]";
    }
};

class CacheHierarchy {
private:
    vector<CacheLevel> levels;
    int ramAccessCount = 0;

public:
    CacheHierarchy() {
        levels.push_back(CacheLevel("L1", 32, 4));
        levels.push_back(CacheLevel("L2", 128, 12));
        levels.push_back(CacheLevel("L3", 512, 40));
    }

    int getRAMAccesses() const {
        return ramAccessCount;
    }

    int requestMemory(int memIndex) {
        int totalLatency = 0;
        bool hitFound = false;

        for (size_t i = 0; i < levels.size(); i++) {
            levels[i].printCache();
            totalLatency += levels[i].latency;
            if (levels[i].check(memIndex)) {
                cout << " -> HIT (" << levels[i].latency << " cycles)\n";
                hitFound = true;
                break;
            } else {
                cout << " >> MISS\n";
            }
        }

        if (!hitFound) {
            cout << "  Fetching from RAM... (200 cycles)\n";
            totalLatency += 200;
            ramAccessCount++;
        }

        for (size_t i = 0; i < levels.size(); i++) {
            int evicted = levels[i].addMemory(memIndex);
            levels[i].printCache();
            if (evicted != -1) cout << " (M" << evicted << " evicted)";
            cout << "\n";
        }

        return totalLatency;
    }
};

struct Task {
    string name;
    int burst;
    vector<int> memLocation;
    size_t memFetchIndex = 0;

    Task(string taskName, int burstTime, vector<int> mem)
        : name(taskName), burst(burstTime), memLocation(mem) {}
};

struct TaskComparer {
    bool operator()(const Task& t1, const Task& t2) {
        if (t1.burst == t2.burst)
            return t1.name > t2.name;
        return t1.burst > t2.burst;
    }
};

vector<Task> parseInput(const string& filename) {
    ifstream file(filename);
    vector<Task> result;
    if (!file.is_open()) return result;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string dummy, name, burstStr, memToken;
        int burst;

        ss >> dummy >> name >> dummy >> burst >> memToken;

        vector<int> mems;
        while (ss >> memToken) {
            if (memToken[0] == 'M')
                mems.push_back(stoi(memToken.substr(1)));
        }

        result.push_back(Task(name, burst, mems));
    }

    return result;
}

int main() {
    priority_queue<Task, vector<Task>, TaskComparer> scheduler;
    CacheHierarchy memorySystem;

    vector<Task> taskPool = parseInput("input/input_task2.txt");
    if (taskPool.empty()) {
        cerr << "Error reading file or input is empty.\n";
        return 1;
    }

    int currentCycle = 0;
    int tasksComplete = 0;
    size_t poolIndex = 0;

    while (poolIndex < taskPool.size() || !scheduler.empty()) {
        if (poolIndex < taskPool.size()) {
            scheduler.push(taskPool[poolIndex]);
            poolIndex++;
        }

        if (!scheduler.empty()) {
            Task currentTask = scheduler.top();
            scheduler.pop();

            currentCycle++;
            cout << "Cycle " << currentCycle << " - Running: " << currentTask.name;

            if (currentTask.memFetchIndex < currentTask.memLocation.size()) {
                int targetMem = currentTask.memLocation[currentTask.memFetchIndex];
                cout << " Requesting: M" << targetMem << "\n";

                int penalty = memorySystem.requestMemory(targetMem);
                currentCycle += (penalty - 1);
                currentTask.memFetchIndex++;
            } else {
                cout << " Processing compute cycle\n";
            }

            currentTask.burst--;

            if (currentTask.burst <= 0) {
                cout << "Cycle " << currentCycle << " -> Task " << currentTask.name << " completed\n\n";
                tasksComplete++;
            } else {
                scheduler.push(currentTask);
            }
        } else {
            currentCycle++;
        }
    }

    cout << "=== Final Results ===\n";
    cout << "Total Cycles:    " << currentCycle << "\n";
    cout << "Tasks Completed: " << tasksComplete << "\n";
    cout << "Scheduler:       Shortest Job First (SJF)\n";
    cout << "RAM Accesses:    " << memorySystem.getRAMAccesses() << "\n";

    return 0;
}
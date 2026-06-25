# CPUSim

A CPU Scheduler and Memory Hierarchy Simulator written in C++.

## Features

- Shortest Job First (SJF) Scheduler
- Three-level Cache Hierarchy
    - L1 (32 slots)
    - L2 (128 slots)
    - L3 (512 slots)
- FIFO Cache Replacement
- RAM Simulation
- Cache Hit/Miss Visualization
- RAM Access Statistics

## Project Structure

```
src/
input/
output/
```

## Compile

```bash
g++ src/main.cpp -o CPUSim
```

## Run

```bash
./CPUSim
```

---

Input file:

```
input/input_task2.txt

output : ![Uploading outputtask2.png…]()

```

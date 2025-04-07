# False Sharing Experiment in Multithreading

## Overview

This program demonstrates the concept of false sharing in multithreaded programming, where two or more threads modify variables that reside on the same cache line. False sharing can degrade performance due to cache coherence protocols. The program compares the performance of two approaches:
1. **Adjacent Counters**: Counters stored adjacently in memory, which can lead to false sharing.
2. **Padded Counters**: Counters with padding between them, ensuring that they are stored in separate cache lines to avoid false sharing.

The program measures the time taken for multiple threads to update the counters in both cases and calculates the speedup achieved by using padding to reduce false sharing.

## Features

- **Multi-threading**: Updates counters in parallel across multiple threads.
- **Experimentation**: The program allows you to specify the number of threads, number of updates per thread, and the number of experimental runs.
- **CSV Logging**: Results are logged to a CSV file, which contains:
  - Run number
  - Time taken for adjacent counters
  - Time taken for padded counters
  - Speedup ratio (adjacent / padded time)
- **Average Speedup**: The program calculates and displays the average speedup after running the experiment multiple times.

## Requirements

- A C++17 or later compatible compiler (e.g., `g++` or `clang++`).
- **Kaizen** library for command-line argument parsing (for optional arguments).
- POSIX thread support (e.g., `pthread` on Linux/macOS).

## Compilation & Running

### 1. Clone the repository (if applicable) or create the program in your local environment.

### 2. Build the program

```bash
g++ -std=c++17 -o false_sharing_experiment main.cpp -pthread
```

### 3. Run the program

```bash
./false_sharing_experiment --num_threads 4 --num_updates 1000000 --num_runs 100
```

- `--num_threads`: Number of threads to use for updating the counters (default: 4).
- `--num_updates`: Number of updates each thread should perform on the counters (default: 1,000,000).
- `--num_runs`: Number of times to run the experiment (default: 100).

### 4. Results

After running the experiment, the program will display the following in the console:
- The number of threads, updates, and runs used.
- Average time for adjacent and padded counters.
- Average speedup (how much faster padded counters are compared to adjacent ones).

Additionally, the results will be saved to a CSV file called `experiment_results.csv`, containing detailed time measurements for each run, including the speedup ratio.

## Sample Console Output

```
-----------------------------------------------
Experiment Parameters:
-----------------------------------------------
Number of Threads: 4
Number of Updates per Thread: 1000000
Number of Runs: 100
-----------------------------------------------
Average Time for Adjacent Counters: 0.123456 seconds
Average Time for Padded Counters: 0.041234 seconds
Average Speedup: 3.0000x
-----------------------------------------------
Results have been saved to 'experiment_results.csv'
```

## CSV Output Format

The `experiment_results.csv` file will contain the following columns:
- `Run`: The run number (1 to N).
- `Time Adjacent (s)`: The time taken for the adjacent counters (in seconds).
- `Time Padded (s)`: The time taken for the padded counters (in seconds).
- `Speedup`: The ratio of the time taken by adjacent counters to the time taken by padded counters.

Example CSV output:
```
Run,Time Adjacent (s),Time Padded (s),Speedup
1,0.412345,0.134567,3.0621
2,0.398765,0.130789,3.0504
...
100,0.399876,0.131234,3.0498
```

## Explanation of the Code

- **CountersAdjacent**: A struct with four counters stored adjacently, potentially causing false sharing.
- **CountersPadded**: A struct with padding inserted between counters to prevent false sharing by ensuring each counter is stored in a separate cache line.
- **updateCounters()**: A template function that atomically updates the counters multiple times from different threads.
- **Main Function**:
  - Reads command-line arguments for the number of threads, updates, and runs.
  - Runs the experiment for both adjacent and padded counters.
  - Logs the results to a CSV file and outputs the average speedup to the console.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

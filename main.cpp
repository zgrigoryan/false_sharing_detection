#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <cassert>

#include "kaizen.h"

struct CountersAdjacent {
    std::atomic<int> counter1{0};
    std::atomic<int> counter2{0};
    std::atomic<int> counter3{0};
    std::atomic<int> counter4{0};
};

struct CountersPadded {
    std::atomic<int> counter1{0};
    char padding1[64];  // (64-byte cache line size on local machine)
    std::atomic<int> counter2{0};
    char padding2[64];
    std::atomic<int> counter3{0};
    char padding3[64];
    std::atomic<int> counter4{0};
};

// Function to update counters
template<typename T>
void updateCounters(T& counters, int numUpdates) {
    for (int i = 0; i < numUpdates; ++i) {
        counters.counter1.fetch_add(1, std::memory_order_relaxed);
        counters.counter2.fetch_add(1, std::memory_order_relaxed);
        counters.counter3.fetch_add(1, std::memory_order_relaxed);
        counters.counter4.fetch_add(1, std::memory_order_relaxed);
    }
}

int main(int argc, char *argv[])
{
    int numThreads = 4, numUpdates = 1000000, numRuns = 100; 

    zen::cmd_args args(argv, argc);

    if (args.is_present("--num_threads"))
    {
        numThreads = std::stoi(args.get_options("--num_threads")[0]);
    }

    if (args.is_present("--num_updates"))
    {
        numUpdates = std::stoi(args.get_options("--num_updates")[0]);
    }

    if (args.is_present("--num_runs"))
    {
        numRuns = std::stoi(args.get_options("--num_runs")[0]);
    }

    if (numThreads <= 0 || numUpdates <= 0 || numRuns <= 0)
    {
        std::cerr << "Number of threads, updates, and runs must all be positive integers." << std::endl;
        return 1;
    }

    std::ofstream outputFile("experiment_results.csv");
    outputFile << "Run,Time Adjacent (s),Time Padded (s),Speedup\n"; 

    double totalSpeedup = 0;

    for (int run = 1; run <= numRuns; ++run) {
        // Measure time for adjacent counters
        CountersAdjacent countersAdjacent;
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<std::thread> threadsAdjacent;
        for (int i = 0; i < numThreads; ++i) {
            threadsAdjacent.push_back(std::thread(updateCounters<CountersAdjacent>, std::ref(countersAdjacent), numUpdates));
        }
        for (auto& t : threadsAdjacent) {
            t.join();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto durationAdjacent = std::chrono::duration<double>(end - start).count();

        // Measure time for padded counters
        CountersPadded countersPadded;
        start = std::chrono::high_resolution_clock::now();
        std::vector<std::thread> threadsPadded;
        for (int i = 0; i < numThreads; ++i) {
            threadsPadded.push_back(std::thread(updateCounters<CountersPadded>, std::ref(countersPadded), numUpdates));
        }
        for (auto& t : threadsPadded) {
            t.join();
        }
        end = std::chrono::high_resolution_clock::now();
        auto durationPadded = std::chrono::duration<double>(end - start).count();

        // Calculate speedup and log the result to CSV
        double speedup = durationAdjacent / durationPadded;
        totalSpeedup += speedup;

        outputFile << run << "," 
                   << std::fixed << std::setprecision(6) << durationAdjacent << ","
                   << std::fixed << std::setprecision(6) << durationPadded << ","
                   << std::fixed << std::setprecision(6) << speedup << "\n";
    }

    outputFile.close();

    // Output the results to the console
    double averageSpeedup = totalSpeedup / numRuns;
    double averageTimeAdjacent = 0;
    double averageTimePadded = 0;
    std::ifstream inputFile("experiment_results.csv");
    std::string line;
    std::getline(inputFile, line); // Skip header   
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string run;
        double timeAdjacent, timePadded, speedup;
        std::getline(iss, run, ',');
        iss >> timeAdjacent;
        iss.ignore(1); // Ignore comma
        iss >> timePadded;
        iss.ignore(1); // Ignore comma
        iss >> speedup;

        averageTimeAdjacent += timeAdjacent;
        averageTimePadded += timePadded;
    }
    averageTimeAdjacent /= numRuns;
    averageTimePadded /= numRuns;
    inputFile.close();
    averageSpeedup = averageTimeAdjacent / averageTimePadded;
    std::cout << "\n-----------------------------------------------\n";
    std::cout << "Experiment Parameters:\n";
    std::cout << "-----------------------------------------------\n";
    std::cout << "Number of Threads: " << numThreads << "\n";
    std::cout << "Number of Updates per Thread: " << numUpdates << "\n";
    std::cout << "Number of Runs: " << numRuns << "\n";
    std::cout << "-----------------------------------------------\n";   
    std::cout << "Average Time for Adjacent Counters: " << averageTimeAdjacent << " seconds\n";
    std::cout << "Average Time for Padded Counters: " << averageTimePadded << " seconds\n";
    std::cout << "Average Speedup: " << averageSpeedup << "x\n";
    std::cout << "-----------------------------------------------\n";
    std::cout << "Results have been saved to 'experiment_results.csv'\n";

    return 0;
}

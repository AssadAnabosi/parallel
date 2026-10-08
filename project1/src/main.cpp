#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <chrono>
#include <tuple>
#include "sequential.cpp"
#include "parallel.cpp"


// Function to save execution times to a CSV file
void save_times_to_csv(const std::string &filename, const std::vector<std::tuple<int, int, double, double>> &results)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        // Write the header
        file << "Array Size,Threads,Execution Time (ms),Speedup" << std::endl;

        // Write the results
        for (const auto &result : results)
        {
            file << std::get<0>(result) << "," << std::get<1>(result) << "," << std::get<2>(result) << "," << std::get<3>(result) << std::endl;
        }

        file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
    }
}

// Utility Function to save vector data into a text file
void save_vector_to_file(const std::string &filename, const std::vector<int> &vec)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << vec.size() << std::endl; // Write the size of the vector
        for (int v : vec)
        {
            file << v << " "; // Write each element separated by a space
        }
        file << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
    }
}

// generate a random vector of integers
std::vector<int> generate_random_vector(int size)
{
    std::vector<int> vec(size);
    for (int i = 0; i < size; ++i)
    {
        vec[i] = rand() % 100; // Random values between 0 and 99
    }
    std::ostringstream filename;
    filename << "../results/data_" << size << ".txt";
    save_vector_to_file(filename.str(), vec);
    return vec;
}

int main()
{
    std::filesystem::create_directory("../results");
    int sizes[] = {
        128000,
        256000,
        512000,
        1024000,
        2048000,
        4096000,
    }; // Sizes of the arrays to test
    int threads[] = {
        2, 4, 5, 6, 8, 10, 12};

    std::vector<std::tuple<int, int, double, double>> results; // To store execution times and speedup
    double sequential_time = 0.0;
    int seqLastItem = 0, parLastItem = 0;
    for (int size : sizes)
    {
        sequential_time = 0.0,
        seqLastItem = 0,
        parLastItem = 0;
        std::cout << "Array size: " << size << std::endl;
        // Generate a random array of the current size
        std::vector<int> data = generate_random_vector(size);

        std::cout << "Sequential: " << std::endl;
        double totalDuration = 0.0;
        for (int i = 1; i <= 5; ++i)
        {
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<int> seq_result = prefix_sum_sequential(data);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            std::cout << "#" << i << " Execution time: " << duration.count() << " ms" << std::endl;
            totalDuration += duration.count();
            seqLastItem = seq_result[size - 1];
        }
        double averageDuration = totalDuration / 5.0;
        std::cout << "Seq. Avg. Execution time: " << averageDuration << " ms" << std::endl;
        sequential_time = averageDuration;
        results.emplace_back(size, 1, averageDuration, 1.0);

        std::cout << "Parallel: " << std::endl;
        for (int thread : threads)
        {
            totalDuration = 0.0;
            std::cout << "Threads: " << thread << std::endl;
            for (int i = 1; i <= 5; ++i)
            {
                auto start = std::chrono::high_resolution_clock::now();
                std::vector<int> par_result = prefix_sum_parallel(data, thread);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> duration = end - start;
                std::cout << "#" << i << " Execution time: " << duration.count() << " ms" << std::endl;
                totalDuration += duration.count();
                parLastItem = par_result[size - 1];
            }
            double averageDuration = totalDuration / 5.0;
            std::cout << "Par. Avg. Execution time: " << averageDuration << " ms" << std::endl;
            double speedup = sequential_time / averageDuration;
            std::cout << "Speedup: " << speedup << std::endl;
            bool valid = seqLastItem == parLastItem;
            std::cout << "Valid Result: " << (valid ? "Yes" : "No") << std::endl;
            results.emplace_back(size, thread, averageDuration, speedup);
        }

        // Free vector memory
        data.clear();
    }

    // Save results to a CSV file
    save_times_to_csv("../results/execution_times.csv", results);

    return 0;
}
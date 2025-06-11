
#include <vector>
#include <omp.h>
#include <iostream>

std::vector<int> prefix_sum_parallel(const std::vector<int> &input, int num_threads)
{
    int n = input.size();
    if (n == 0)
        return {};

    std::vector<int> output(n);
    omp_set_num_threads(num_threads);

    int block_size = (n + num_threads - 1) / num_threads;
    std::vector<int> block_sums(num_threads, 0);

// Step 1: Compute prefix sum within each block and collect block totals
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int start = tid * block_size;
        int end = std::min(start + block_size, n);

        if (start < n)
        {
            output[start] = input[start];
            for (int i = start + 1; i < end; i++)
            {
                output[i] = output[i - 1] + input[i];
            }
            block_sums[tid] = output[end - 1];
        }
    }

    // Step 2: Compute prefix sum of block sums (sequential for simplicity)
    for (int i = 1; i < num_threads; i++)
    {
        block_sums[i] += block_sums[i - 1];
    }

// Step 3: Add block prefix to each element
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int start = tid * block_size;
        int end = std::min(start + block_size, n);

        if (tid > 0 && start < n)
        {
            for (int i = start; i < end; i++)
            {
                output[i] += block_sums[tid - 1];
            }
        }
    }

    return output;
}
# Prefix Sum: Sequential vs. Parallel Implementation Using Pthread

## 1. Introduction

We selected the **Prefix Sum (Scan)** algorithm for this project due to its simplicity and suitability for parallel processing. Given an array of numbers, the prefix sum at position `i` is the sum of all elements up to index `i`. This algorithm is commonly used in parallel computing applications such as stream compaction, histogram generation, and cumulative sums.

### Why it's Parallelize-able:

Because it depends on earlier values, the prefix sum is by nature sequential. However, we can efficiently parallelize it by splitting the input array into chunks and performing local prefix sums in parallel, followed by an offset adjustment phase. This method illustrates important parallel programming ideas like thread communication, synchronization, and work division.

### Time Measurement:

Time was measured using the `std::chrono::high_resolution_clock` for precision, capturing start and end time around the function call.

## 2. Sequential Implementation

### Code Explanation:

```cpp
std::vector<int> prefix_sum_sequential(const std::vector<int>& input) {
    std::vector<int> output(input.size());
    if (input.empty()) return output;

    output[0] = input[0];
    for (size_t i = 1; i < input.size(); ++i) {
        output[i] = output[i - 1] + input[i];
    }
    return output;
}
```

- We initialize the output array.
- Compute prefix sums in a single loop using the previous value.

## 3. Parallelization Strategy

### Work Division:

- The array is divided into `n` chunks, one per thread.
- Each thread computes the prefix sum for its chunk.
- Threads synchronize using condition variables.
- A second pass adjusts each chunk with the sum of all previous chunks.

### Pthread Usage:

- `pthread_t`: To create threads.
- `pthread_create`, `pthread_join`: For lifecycle management.
- `pthread_mutex_t`, `pthread_cond_t`: For synchronization.

### Data Structure:

```cpp
struct ThreadData {
    const std::vector<int>* input;
    std::vector<int>* output;
    int start, end, thread_idx;
    int* thread_sums;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    int* threads_done;
    int total_threads;
};
```

### Correctness Validation:

Last item within the output vector must be the same as the last one in the sequential result, otherwise the result is not correct.

## 4. Experiments

### Hardware:

- **CPU**: Intel Core i5-9600KF
- **Cores**: 6 Cores
- **Speed**: 4.8GHz (Fixed Overclocked across all 6 cores)
- **OS**: Ubuntu 22.04 (WSL2)

CPU Speed is not turbo boost, rather its a fixed speed at all times.

### Input Sizes and Thread Counts:

- Input sizes: 128000, 256000, 512000, 1024000, 2048000, 4096000
- Threads: 1, 2, 4, 5, 6, 8, 10, 12

## 5. Results

| **Array Size** | **Threads** | **Execution Time (ms)** | **Speedup** |
| -------------- | ----------- | ----------------------- | ----------- |
| **128000**     | 1           | 1.28133                 | 1.00        |
| **128000**     | 2           | 1.34374                 | 0.953557    |
| **128000**     | 4           | 0.803049                | 1.59558     |
| **128000**     | 5           | 0.814678                | 1.57281     |
| **128000**     | 6           | 0.901286                | 1.42167     |
| **128000**     | 8           | 1.31887                 | 0.971537    |
| **128000**     | 10          | 1.56716                 | 0.817611    |
| **128000**     | 12          | 1.37749                 | 0.930195    |
|                |             |                         |             |
| **256000**     | 1           | 2.22821                 | 1.00        |
| **256000**     | 2           | 1.50326                 | 1.48225     |
| **256000**     | 4           | 1.53664                 | 1.45006     |
| **256000**     | 5           | 1.3413                  | 1.66123     |
| **256000**     | 6           | 1.36028                 | 1.63805     |
| **256000**     | 8           | 2.37848                 | 0.93682     |
| **256000**     | 10          | 2.18914                 | 1.01785     |
| **256000**     | 12          | 2.18841                 | 1.01819     |
|                |             |                         |             |
| **512000**     | 1           | 5.51413                 | 1.00        |
| **512000**     | 2           | 2.78919                 | 1.97696     |
| **512000**     | 4           | 2.24206                 | 2.4594      |
| **512000**     | 5           | 3.54777                 | 1.55425     |
| **512000**     | 6           | 3.0919                  | 1.78341     |
| **512000**     | 8           | 2.40235                 | 2.29531     |
| **512000**     | 10          | 3.0144                  | 1.82926     |
| **512000**     | 12          | 3.33786                 | 1.652       |
|                |             |                         |             |
| **1024000**    | 1           | 9.56233                 | 1.00        |
| **1024000**    | 2           | 5.93904                 | 1.61008     |
| **1024000**    | 4           | 4.03831                 | 2.36791     |
| **1024000**    | 5           | 4.44345                 | 2.152       |
| **1024000**    | 6           | 4.80533                 | 1.98994     |
| **1024000**    | 8           | 5.06352                 | 1.88847     |
| **1024000**    | 10          | 4.6064                  | 2.07588     |
| **1024000**    | 12          | 4.61247                 | 2.07315     |
|                |             |                         |             |
| **2048000**    | 1           | 18.8151                 | 1.00        |
| **2048000**    | 2           | 13.7053                 | 1.37284     |
| **2048000**    | 4           | 9.70155                 | 1.93939     |
| **2048000**    | 5           | 16.4394                 | 1.14451     |
| **2048000**    | 6           | 12.3209                 | 1.52709     |
| **2048000**    | 8           | 8.249                   | 2.2809      |
| **2048000**    | 10          | 8.23724                 | 2.28415     |
| **2048000**    | 12          | 9.40852                 | 1.9998      |
|                |             |                         |             |
| **4096000**    | 1           | 35.1043                 | 1.00        |
| **4096000**    | 2           | 21.3825                 | 1.64173     |
| **4096000**    | 4           | 15.5632                 | 2.25559     |
| **4096000**    | 5           | 16.5653                 | 2.11914     |
| **4096000**    | 6           | 16.254                  | 2.15973     |
| **4096000**    | 8           | 17.2186                 | 2.03874     |
| **4096000**    | 10          | 15.4177                 | 2.27688     |
| **4096000**    | 12          | 24.1357                 | 1.45445     |

### Graphs:

![Execution Time vs. Thread Count](docs\graphs\Graph1.jpg)

![Speedup vs. Thread Count](docs\graphs\Graph2.jpg)

## 6. Discussion

### Sublinear Speedup Causes:

- **Overhead**: Thread creation and synchronization delay actual computation.
- **Load Imbalance**: Last chunk may be smaller, reducing efficiency.
- **Memory Bandwidth**: Increased contention on shared resources like memory.

### Amdahl's Law:

Amdahl's Law states the maximum speedup is limited by the sequential portion of the algorithm. Our results show diminishing returns as thread count increases, in line with Amdahl's prediction.

## 7. Conclusion

### Lessons Learned:

- Simple algorithms can illustrate complex parallelization concepts.
- Synchronization primitives (mutex, cond_var) are essential but tricky.
- Real-world speedups depend on both algorithm structure and hardware constraints.

### Challenges:

- Avoiding race conditions.
- Managing thread synchronization properly.
- Accurate time measurements.

### Used Tools:

[CSV to markdown](https://tableconvert.com/csv-to-markdown)

---

This project effectively demonstrated the contrast between sequential and parallel computing using pthread, with practical insights into synchronization and performance analysis.
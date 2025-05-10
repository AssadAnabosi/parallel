#include <iostream>
#include <vector>
#include <pthread.h>

struct ThreadData
{
    const std::vector<int> *input;
    std::vector<int> *output;
    int start;
    int end;
    int thread_idx;
    int *thread_sums;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
    int *threads_done;
    int total_threads;
};

void *prefix_sum_worker(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);
    const std::vector<int> &input = *data->input;
    std::vector<int> &output = *data->output;
    int local_sum = 0;

    for (int i = data->start; i < data->end; ++i)
    {
        local_sum += input[i];
        output[i] = local_sum;
    }

    data->thread_sums[data->thread_idx] = local_sum;

    pthread_mutex_lock(data->mutex);
    (*data->threads_done)++;
    if (*data->threads_done < data->total_threads)
    {
        pthread_cond_wait(data->cond, data->mutex);
    }
    else
    {
        pthread_cond_broadcast(data->cond);
    }
    pthread_mutex_unlock(data->mutex);

    if (data->thread_idx > 0)
    {
        int offset = 0;
        for (int i = 0; i < data->thread_idx; ++i)
        {
            offset += data->thread_sums[i];
        }
        for (int i = data->start; i < data->end; ++i)
        {
            output[i] += offset;
        }
    }

    return nullptr;
}

std::vector<int> prefix_sum_parallel(const std::vector<int> &input, int num_threads)
{
    std::vector<int> output(input.size(), 0);
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    int thread_sums[num_threads] = {0};
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    int threads_done = 0;

    int start = 0;
    int chunk = input.size() / num_threads;
    int reminder = input.size() % num_threads;

    for (int idx = 0; idx < num_threads; ++idx)
    {
        thread_data[idx] = {
            &input,
            &output,
            start,
            start + chunk + (idx < reminder),
            idx,
            thread_sums,
            &mutex,
            &cond,
            &threads_done,
            num_threads};
        pthread_create(&threads[idx], nullptr, prefix_sum_worker, &thread_data[idx]);
        start = thread_data[idx].end;
    }

    for (int idx = 0; idx < num_threads; ++idx)
    {
        pthread_join(threads[idx], nullptr);
    }

    return output;
}
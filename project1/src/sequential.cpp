#include <iostream>
#include <vector>

// Sequential prefix sum (scan)
std::vector<int> prefix_sum_sequential(const std::vector<int>& input) {
    std::vector<int> output(input.size());
    if (input.empty()) return output;

    output[0] = input[0];
    for (size_t i = 1; i < input.size(); ++i) {
        output[i] = output[i - 1] + input[i];
    }
    return output;
}
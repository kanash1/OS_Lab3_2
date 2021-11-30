#include <iostream>
#include <chrono>
#include <utility>
#include <iomanip>
#include <omp.h>
#include "incontrol.h"

const size_t BLOCK_SIZE = 9'307'130;
const size_t ACCURACY = 100'000'000;

std::pair<long double, long long> pi_and_avg_time_calc_omp(size_t);

int main() {
    InputControler<size_t> input(std::cin);
    size_t threads_count = 0;

    std::cout << "Enter count of threads (max 64): ";
    input >> threads_count;

    if (input.GetSate() == InputState::NORMAL && threads_count <= 64) {
        auto pi_and_avg_time = pi_and_avg_time_calc_omp(threads_count);
        std::cout << "Average time: " << pi_and_avg_time.second << " milliseconds\n"
            << "Pi value: " << std::fixed << std::setprecision(15) << pi_and_avg_time.first << '\n';
    }
}

std::pair<long double, long long> pi_and_avg_time_calc_omp(const size_t threads_count) {
    long long time = 0;
    long double pi = 0;
    long double x = 0;

    for (size_t i = 0; i < 100; ++i) {
        pi = 0;
        x = 0;
        auto start = std::chrono::high_resolution_clock::now();

        omp_set_num_threads(static_cast<int>(threads_count));
#pragma omp parallel for private(x) schedule(dynamic, BLOCK_SIZE) reduction(+:pi)
        for (int i = 0; i < ACCURACY; ++i) {
            x = (i + 0.5) / ACCURACY;
            pi += 4 / (1 + x * x);
        }
        pi /= ACCURACY;

        auto stop = std::chrono::high_resolution_clock::now();
        time += std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    }

    return std::make_pair(pi, time / 100);
}
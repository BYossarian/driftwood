
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

#include "./sort/insertion-sort.h"
#include "./sort/merge-sort.h"
#include "./sort/heap-sort.h"
#include "./sort/quick-sort.h"
#include "./helpers/timer.h"

void print_array(const std::vector<int> &array) {

    for (int i = 0, l = array.size(); i < l; i++) {

        std::cout << array[i] << " ";

    }

    std::cout << "\n";

}

bool is_sorted(const std::vector<int> &array) {

    for (int i = 1, l = array.size(); i < l; i++) {

        if (array[i - 1] > array[i]) {
            return false;
        }

    }

    return true;

}

// checks that the array is the sequence 0, 2, 3, 4, 6, ...
bool is_ordered_evens(const std::vector<int> &array) {

    for (int i = 0, l = array.size(); i < l; i++) {

        if (array[i] != 2 * i) {
            return false;
        }

    }

    return true;

}

int main() {

    timer<std::chrono::microseconds> sort_timer;
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<int> array(10000);
    for (int i = 0, l = array.size(); i < l; i++) { array[i] = 2 * i; }

    
    // insertion sort

    std::shuffle(array.begin(), array.end(), gen);
    std::cout << "insertion sort: ";

    sort_timer.reset();
    insertion_sort(array);

    std::cout << sort_timer.get_ticks() / 1000.0 << " ms\n";
    if (!is_ordered_evens(array)) {
        std::cout << "FAILED!\n";
        throw;
    }

    
    // merge sort

    std::shuffle(array.begin(), array.end(), gen);
    std::cout << "merge sort: ";

    sort_timer.reset();
    merge_sort(array);

    std::cout << sort_timer.get_ticks() / 1000.0 << " ms\n";
    if (!is_ordered_evens(array)) {
        std::cout << "FAILED!\n";
        throw;
    }

    
    // heap sort

    std::shuffle(array.begin(), array.end(), gen);
    std::cout << "heap sort: ";

    sort_timer.reset();
    heap_sort(array);

    std::cout << sort_timer.get_ticks() / 1000.0 << " ms\n";
    if (!is_ordered_evens(array)) {
        std::cout << "FAILED!\n";
        throw;
    }

    
    // quick sort

    std::shuffle(array.begin(), array.end(), gen);
    std::cout << "quick sort: ";

    sort_timer.reset();
    quick_sort(array);

    std::cout << sort_timer.get_ticks() / 1000.0 << " ms\n";
    if (!is_ordered_evens(array)) {
        std::cout << "FAILED!\n";
        throw;
    }

    
    // randomised quick sort

    std::shuffle(array.begin(), array.end(), gen);
    std::cout << "randomised quick sort: ";

    sort_timer.reset();
    randomised_quick_sort(array);

    std::cout << sort_timer.get_ticks() / 1000.0 << " ms\n";
    if (!is_ordered_evens(array)) {
        std::cout << "FAILED!\n";
        throw;
    }

    
    // standard lib sort

    std::shuffle(array.begin(), array.end(), gen);
    std::cout << "standard lib sort: ";

    sort_timer.reset();
    std::sort(array.begin(), array.end());

    std::cout << sort_timer.get_ticks() / 1000.0 << " ms\n";
    if (!is_ordered_evens(array)) {
        std::cout << "FAILED!\n";
        throw;
    }

    std::cout << "all good... :)\n";

}


#include <vector>
#include <random>
#include <functional>

// partitions an array and returns the index of the point at which 
// the array has been split such that every element to the left is 
// less than the value at the partition, and every element to the right 
// is greater
int partition(std::vector<int> &array, int start, int end) {

    // choose some value to act as the partition:
    int partition_value = array[end];

    // now swap any elements less than that value to the start
    // (and thus, as a consequence, elements larger than partition_value
    // will be at the end):
    int partition_index = start;
    for (int i = start; i < end; i++) {
        if (array[i] < partition_value) {
            int temp = array[i];
            array[i] = array[partition_index];
            array[partition_index] = temp;
            partition_index++;
        }
    }

    // partition_index now points to the point in the array at which 
    // the partition will take place. Just need to swap the partition_value
    // into place:
    array[end] = array[partition_index];
    array[partition_index] = partition_value;

    return partition_index;

}

void quick_sort(std::vector<int> &array, int start, int end) {

    if (start < end) {

        int partition_index = partition(array, start, end);

        quick_sort(array, start, partition_index - 1);
        quick_sort(array, partition_index + 1, end);

    }

}

void quick_sort(std::vector<int> &array) {
    quick_sort(array, 0, array.size() - 1);
}

// quick_sort will actually perform very poorly (O(n^2)) if the 
// array is already sorted. Since sorting an already sorted (or
// nearly sorted) array is relatively common (e.g. inserting some 
// value into an already sorted array), we introduce some 
// randomness as part of the quick_sort algorithm in order to 
// side-step this. The randomness comes in the form of choosing 
// a partition_value at random within partition. Or, more precisely, 
// we move a randomly chosen element to the end before calling partition:

void randomised_quick_sort(std::vector<int> &array, int start, int end, const std::function<double()> &get_random) {

    if (start < end) {

        {
            // swap a random element of array[start ... end]
            // to the end:
            int swap_index = get_random() * (end - start) + start;
            int temp = array[swap_index];
            array[swap_index] = array[end];
            array[end] = temp;
        }

        int partition_index = partition(array, start, end);

        randomised_quick_sort(array, start, partition_index - 1, get_random);
        randomised_quick_sort(array, partition_index + 1, end, get_random);

    }

}

void randomised_quick_sort(std::vector<int> &array) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distribution(0.0, 1.0);
    randomised_quick_sort(array, 0, array.size() - 1, [&gen, &distribution]() {
        return distribution(gen);
    });
}

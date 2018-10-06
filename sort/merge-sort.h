
#include <vector>

// merges array[start ... middle] with array[middle + 1 ... end]:
void merge(std::vector<int> &array, int start, int middle, int end) {

    int left_size = middle - start + 1;
    int right_size = end - middle;

    std::vector<int> left(left_size);
    std::vector<int> right(right_size);

    for (int i = 0; i < left_size; i++) {
        left[i] = array[start + i];
    }

    for (int i = 0; i < right_size; i++) {
        right[i] = array[middle + 1 + i];
    }

    int left_pointer = 0;
    int right_pointer = 0;

    for (int i = start; i <= end; i++) {

        // if we've gone beyond the bounds of the left array, or we're still within the bounds of the right, and 
        // the right element is smaller than the left, then take from the right; otherwise, take from the left:
        if (left_pointer >= left_size || (right_pointer < right_size && left[left_pointer] > right[right_pointer])) {
            array[i] = right[right_pointer];
            right_pointer++;
        } else {
            array[i] = left[left_pointer];
            left_pointer++;
        }

    }

}

void merge_sort(std::vector<int> &array, int start, int end) {

    if (start < end) {

        int middle = (start + end) / 2;

        merge_sort(array, start, middle);
        merge_sort(array, middle + 1, end);
        merge(array, start, middle, end);

    }

}

void merge_sort(std::vector<int> &array) {
    merge_sort(array, 0, array.size() - 1);
}

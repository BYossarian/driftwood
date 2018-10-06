
#include <vector>

// takes a heap, and an index into that heap. Assumes that 
// the children of array[index] are both max-heaps in order 
// to make the sub-tree rooted at array[index] a max-heap:
void fix_max_heap_subtree(std::vector<int> &array, int index, int heap_length) {

    while (true) {

        int left_child = 2 * index + 1;
        int right_child = left_child + 1;
        int largest = index;

        if (left_child < heap_length && array[left_child] > array[largest]) {
            largest = left_child;
        }
        if (right_child < heap_length && array[right_child] > array[largest]) {
            largest = right_child;
        }

        if (largest == index) { break; }

        // swap array[index] and array[largest]
        int temp = array[index];
        array[index] = array[largest];
        array[largest] = temp;

        index = largest;

    }

}

// converts an array of ints into a max heap:
void array_to_max_heap(std::vector<int> &array) {

    int heap_size = array.size();

    for (int i = heap_size/2; i >= 0; i--) {

        fix_max_heap_subtree(array, i, heap_size);

    }

}

void heap_sort(std::vector<int> &array) {

    array_to_max_heap(array);

    int heap_size = array.size();

    while (heap_size > 1) {

        // move current largest element to end of array:
        int temp = array[0];
        array[0] = array[heap_size - 1];
        array[heap_size - 1] = temp;

        // remove this element from the heap:
        heap_size--;

        // fix the heap:
        fix_max_heap_subtree(array, 0, heap_size);

    }

}

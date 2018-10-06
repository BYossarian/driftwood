
#include <vector>

void insertion_sort(std::vector<int> &array) {

    for (int i = 1, l = array.size(); i < l; i++) {

        int value = array[i];
        int j = i - 1;

        while (j >= 0 && array[j] > value) {

            array[j + 1] = array[j];
            j--;

        }

        array[j + 1] = value;

    }

}

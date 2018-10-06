
#include <iostream>
#include <random>
#include <algorithm>

#include "./set-adt/binary-search-tree.h"
#include "./set-adt/avl-tree.h"

int main() {

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<int> array(15);
    for (int i = 0, l = array.size(); i < l; i++) { array[i] = i; }

    binary_search_tree<int> bst;

    std::shuffle(array.begin(), array.end(), gen);
    for (int i = 0, l = array.size(); i < l; i++) {
        std::cout << "inserting " << array[i] << "\n";
        bst.insert(array[i]);
        bst.sorted_print();
        if (!bst.verify() || bst.size() != i + 1) {
            throw;
        }
    }

    std::shuffle(array.begin(), array.end(), gen);
    for (int i = 0, l = array.size(); i < l; i++) {
        std::cout << "removing " << array[i] << "\n";
        bst.remove(array[i]);
        bst.sorted_print();
        if (!bst.verify() || bst.size() != 14 - i) {
            throw;
        }
    }

    avl_tree<int> avlt;

    std::shuffle(array.begin(), array.end(), gen);
    for (int i = 0, l = array.size(); i < l; i++) {
        std::cout << "inserting " << array[i] << "\n";
        avlt.insert(array[i]);
        avlt.sorted_print();
        if (!avlt.verify() || avlt.size() != i + 1) {
            throw;
        }
    }

    std::shuffle(array.begin(), array.end(), gen);
    for (int i = 0, l = array.size(); i < l; i++) {
        std::cout << "removing " << array[i] << "\n";
        avlt.remove(array[i]);
        avlt.sorted_print();
        if (!avlt.verify() || avlt.size() != 14 - i) {
            throw;
        }
    }

    std::cout << "all good... :)\n";

}

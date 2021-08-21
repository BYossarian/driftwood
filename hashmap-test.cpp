
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../helpers/time.cpp"
#include "./hashmap/hashmap.cpp"

constexpr size_t TEST_COUNT = 1000000;

char* rand_string() {

    char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t char_length = 5 + (rand() % 16);
    char* string = new char[char_length + 1];
    string[char_length] = '\0';
    while (char_length--) {
        string[char_length] = charset[rand() % 62];
    }

    return string;

}

void test(char** keys, size_t* values) {

    // *** create hash map
    HashMap map = {};
    if (!map.init(512)) {
        printf("INIT ERROR\n");
        return;
    }


    // *** test
    Timer timer;

    // insert
    for (size_t i = 0; i < TEST_COUNT; i++) {
        if (!map.insert(keys[i], (void*)values[i])) {
            printf("INSERT ERROR\n");
            return;
        }
    }

    if (map.num_entries != TEST_COUNT) {
        printf("ENTRY COUNT ERROR\n");
        return;
    }

    // find
    for (size_t i = 0; i < TEST_COUNT; i++) {
        void* found_value;
        bool found = map.find(keys[i], &found_value);
        if (!found || (size_t)found_value != values[i]) {
            printf("FIND ERROR %s - %zu (%i)\n", keys[i], values[i], found);
            //return;
        }
    }

    for (size_t i = 0; i < TEST_COUNT; i++) {
        values[i] *= 3;
    }

    // update
    for (size_t i = 0; i < TEST_COUNT; i++) {
        if (!map.insert(keys[i], (void*)(values[i]))) {
            printf("INSERT ERROR\n");
            return;
        }
    }

    if (map.num_entries != TEST_COUNT) {
        printf("INSERT ERROR\n");
        return;
    }

    // find
    for (size_t i = 0; i < TEST_COUNT; i++) {
        void* found_value;
        if (!map.find(keys[i], &found_value) || (size_t)found_value != values[i]) {
            printf("ERROR\n");
            return;
        }
    }

    // remove
    for (size_t i = 0; i < TEST_COUNT; i++) {
        map.remove(keys[i]);
        void* found_value;
        if (map.find(keys[i], &found_value)) {
            printf("REMOVE ERROR\n");
            return;
        }
    }

    if (map.num_entries != 0) {
        printf("INSERT ERROR\n");
        return;
    }

    for (size_t i = 0; i < TEST_COUNT; i++) {
        void* found_value;
        if (map.find(keys[i], &found_value)) {
            printf("ERROR\n");
            return;
        }
    }

    for (size_t i = 0; i < TEST_COUNT / 10; i++) {
        if (!map.insert(keys[i], (void*)values[i])) {
            printf("INSERT ERROR\n");
            return;
        }
    }

    for (size_t i = 0; i < TEST_COUNT / 20; i++) {
        map.remove(keys[i]);
        void* found_value;
        if (map.find(keys[i], &found_value)) {
            printf("REMOVE ERROR\n");
            return;
        }
    }

    for (size_t i = 0; i < TEST_COUNT; i++) {
        if (!map.insert(keys[i], (void*)values[i])) {
            printf("INSERT ERROR\n");
            return;
        }
    }

    if (map.num_entries != TEST_COUNT) {
        printf("INSERT ERROR\n");
        return;
    }

    // find
    for (size_t i = 0; i < TEST_COUNT; i++) {
        void* found_value;
        if (!map.find(keys[i], &found_value) || (size_t)found_value != values[i]) {
            printf("ERROR\n");
            return;
        }
    }

    int64_t time = timer.get_nanosecond_ticks();

    printf("Took %fms\n", time/1000000.0f);

}

int main() {

    // *** generate key-value pairs
    srand(1234);
    char** keys = new char*[TEST_COUNT];
    size_t* values = new size_t[TEST_COUNT];

    for (size_t i = 0; i < TEST_COUNT; i++) {
        keys[i] = rand_string();
        values[i] = i;
    }

    test(keys, values);
    test(keys, values);
    test(keys, values);

}

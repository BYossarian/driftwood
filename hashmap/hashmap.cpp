
// C-string to void* dictionary implemented via a hash table
// NB: this does not copy the strings internally

#pragma once

#include <stdint.h>
#include <new>
#include <stdlib.h>

#include "./string.cpp"
#include "./no_move_or_copy.cpp"

struct HashMap {

    struct HashMapEntry {
        const char* key = nullptr;
        void* value = nullptr;
        HashMapEntry* next = nullptr;
    };

    // ref: https://nullprogram.com/blog/2018/07/31/
    static uint32_t hash(const char* string) {
        uint32_t hash = UINT32_C(0xa63a177a);
        while (*string != '\0') {
            hash += *string;
            hash ^= hash >> 15;
            hash *= UINT32_C(0x2c1b3c6d);
            hash ^= hash >> 12;
            hash *= UINT32_C(0x297a2d39);
            hash ^= hash >> 15;
            string++;
        }
        hash ^= hash >> 16;
        hash *= UINT32_C(0x85ebca6b);
        hash ^= hash >> 13;
        hash *= UINT32_C(0xc2b2ae35);
        hash ^= hash >> 16;
        return hash + UINT32_C(0xa63a177a);
    }

    size_t num_buckets = 0;
    HashMapEntry** buckets = nullptr;
    size_t num_entries = 0;

    HashMapEntry* hash_entry_pool = nullptr;
    HashMapEntry* hash_entry_pool_head = nullptr;

    // NB: there are no checks to see if the pool is empty (should be fine since the hashmap will get 
    // resized when that becomes a danger)
    inline HashMapEntry* _get_hash_entry_from_pool() {

        HashMapEntry* return_value = hash_entry_pool_head;

        hash_entry_pool_head = hash_entry_pool_head->next ? hash_entry_pool_head->next : hash_entry_pool_head + 1;

        return return_value;

    }

    void _free_hash_entry(HashMapEntry* entry_ptr) {

        entry_ptr->key = nullptr;
        entry_ptr->value = nullptr;
        entry_ptr->next = hash_entry_pool_head;
        hash_entry_pool_head = entry_ptr;

    }

    bool _resize(size_t new_size) {

        HashMapEntry** new_buckets = new(std::nothrow) HashMapEntry*[new_size]();
        if (!new_buckets) {
            return false;
        }
        HashMapEntry* new_hash_entry_pool = new(std::nothrow) HashMapEntry[new_size * 2]();
        if (!new_hash_entry_pool) {
            delete[] new_buckets;
            return false;
        }
        HashMapEntry* new_hash_entry_pool_head = new_hash_entry_pool;

        size_t old_hash_entry_pool_size = num_buckets * 2;
        for (size_t i = 0; i < old_hash_entry_pool_size; i++) {

            HashMapEntry* cursor = hash_entry_pool + i;

            if (cursor->key) {
                new_hash_entry_pool_head->key = cursor->key;
                new_hash_entry_pool_head->value = cursor->value;
                // ref: https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
                uint32_t index = ((uint64_t)hash(new_hash_entry_pool_head->key) * (uint64_t)new_size) >> 32;
                new_hash_entry_pool_head->next = new_buckets[index];
                new_buckets[index] = new_hash_entry_pool_head;
                new_hash_entry_pool_head++;
            }

        }

        delete[] buckets;
        delete[] hash_entry_pool;
        buckets = new_buckets;
        num_buckets = new_size;

        hash_entry_pool = new_hash_entry_pool;
        hash_entry_pool_head = hash_entry_pool + num_entries;

        return true;

    }

    bool init(size_t init_size) {

        buckets = new(std::nothrow) HashMapEntry*[init_size]();
        if (!buckets) {
            return false;
        }
        hash_entry_pool = new(std::nothrow) HashMapEntry[init_size * 2]();
        if (!hash_entry_pool) {
            delete[] buckets;
            return false;
        }
        num_buckets = init_size;
        hash_entry_pool_head = hash_entry_pool;

        return true;

    }

    void destroy() {

        delete[] buckets;
        delete[] hash_entry_pool;

        num_buckets = 0;
        buckets = nullptr;
        num_entries = 0;
    
        hash_entry_pool = nullptr;
        hash_entry_pool_head = nullptr;

    }

    // NB: doesn't copy string internally
    // NB: can fail if resized is needed (check return value)
    bool insert(const char* key, void* value) {

        // ref: https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
        uint32_t index = ((uint64_t)hash(key) * (uint64_t)num_buckets) >> 32;

        HashMapEntry* cursor = buckets[index];

        if (!cursor) {
            if ((num_entries + 1) == num_buckets * 2) {
                // need to resize due to high load factor and running out of space in the 
                // hash entry pool:
                if (!_resize(num_buckets * 2)) {
                    return false;
                }
                return insert(key, value);
            }
            buckets[index] = _get_hash_entry_from_pool();
            buckets[index]->key = key;
            buckets[index]->value = value;
            buckets[index]->next = nullptr;
            num_entries++;
            return true;
        }

        if (string_are_equal(key, cursor->key)) {
            buckets[index]->value = value;
            return true;
        }

        while (cursor->next) {
            cursor = cursor->next;
            if (string_are_equal(key, cursor->key)) {
                cursor->value = value;
                return true;
            }
        }

        if ((num_entries + 1) == num_buckets * 2) {
            // need to resize due to high load factor and running out of space in the 
            // hash entry pool:
            if (!_resize(num_buckets * 2)) {
                return false;
            }
            return insert(key, value);
        }

        cursor->next = _get_hash_entry_from_pool();
        cursor = cursor->next;
        cursor->key = key;
        cursor->value = value;
        cursor->next = nullptr;
        num_entries++;

        return true;

    }

    void remove(const char* key) {

        // ref: https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
        uint32_t index = ((uint64_t)hash(key) * (uint64_t)num_buckets) >> 32;
        HashMapEntry* cursor = buckets[index];
        if (!cursor) {
            return;
        }

        if (string_are_equal(key, cursor->key)) {
            buckets[index] = cursor->next;
            _free_hash_entry(cursor);
            num_entries--;
            if (num_entries * 2 == num_buckets && num_buckets > 16) {
                // not the end of the world if the resize fails so ignore...
                _resize(num_buckets / 2);
            }
            return;
        }

        while (cursor->next) {
            if (string_are_equal(key, cursor->next->key)) {
                HashMapEntry* to_delete = cursor->next;
                cursor->next = cursor->next->next;
                _free_hash_entry(to_delete);
                num_entries--;
                if (num_entries * 2 == num_buckets && num_buckets > 16) {
                    // not the end of the world if the resize fails so ignore...
                    _resize(num_buckets / 2);
                }
                return;
            }
            cursor = cursor->next;
        }

    }

    bool find(const char* key, void** result) {

        // ref: https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
        uint32_t index = ((uint64_t)hash(key) * (uint64_t)num_buckets) >> 32;
        HashMapEntry* cursor = buckets[index];

        while (cursor) {
            if (string_are_equal(key, cursor->key)) {
                *result = cursor->value;
                return true;
            }
            cursor = cursor->next;
        }

        return false;

    }

    NO_COPY_OR_MOVE(HashMap);

};

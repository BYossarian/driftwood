
#include <iostream>
#include <thread>
#include <tuple>
#include <chrono>
#include <mutex>
#include <random>
#include <functional>
#include <vector>
#include <atomic>
#include <future>

#include "./multi-threaded/threadsafe-queue.h"
#include "./multi-threaded/thread-pool.h"

threadsafe_queue<std::tuple<std::thread::id, int, int>> queue;
std::mutex cout_mutex;

void producer() {

    std::thread::id thread_id = std::this_thread::get_id();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1000);
    auto rnd = std::bind(dis, gen);

    for (int i = 0; i < 5; i++) {
        int wait = rnd();
        std::this_thread::sleep_for(std::chrono::milliseconds(wait));
        queue.push(std::make_tuple(thread_id, i, wait));
    }

}

void consumer() {

    std::thread::id thread_id = std::this_thread::get_id();

    while (true) {
        std::tuple<std::thread::id, int, int> result;
        bool more_work = queue.wait_and_pop(result);
        if (!more_work) { break; }
        std::lock_guard lock(cout_mutex);
        std::cout << thread_id << ": consumed " << std::get<1>(result) << ", " << std::get<2>(result) << " from " << std::get<0>(result) << "\n"; 
    }

}

int main() {

    {

        std::vector<std::thread> producers;
        std::vector<std::thread> consumers;

        for (int i = 0; i < 3; i++) {
            producers.emplace_back(producer);
        }

        for (int i = 0; i < 3; i++) {
            consumers.emplace_back(consumer);
        }

        for (int i = 0, l = producers.size(); i < l; i++) {
            producers[i].join();
        }

        queue.finish();

        for (int i = 0, l = consumers.size(); i < l; i++) {
            consumers[i].join();
        }

    }

    {

        thread_pool pool;

        std::cout << "thread count: " << pool.get_thread_count() << "\n";

        for (int i = 0; i < 10; i++) {
            pool.submit([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::lock_guard lock(cout_mutex);
                std::cout << std::this_thread::get_id() << " did nothing of use (" << i << ")\n"; 
            });
        }

        std::lock_guard lock(cout_mutex);
        std::cout << "all work submitted...\n";

    }

    std::cout << "done...\n";

    {

        // TODO: use latch here...

        thread_pool pool;
        std::atomic<int> counter = 10;
        std::promise<void> work_promise;
        auto work_future = work_promise.get_future();
        std::vector<int> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        for (int i = 0; i < 10; i++) {
            pool.submit([&array, i, &counter, &work_promise]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                array[i] *= 2;
                counter--;
                if (counter == 0) {
                    work_promise.set_value();
                }
            });
        }

        std::cout << "all work submitted...\n";

        work_future.get();

        std::cout << "done...\n";

    }

    std::cout << "done...\n";

}

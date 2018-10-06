
#include <chrono>

template <class T = std::chrono::milliseconds>
class timer {

public:
    timer(): start(std::chrono::steady_clock::now()) {}

    void reset() {
        start = std::chrono::steady_clock::now();
    }

    int get_ticks() {
        return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - start).count();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start;

};


#include <random>
#include <algorithm>

class weighted_coin {

public:
    weighted_coin(double true_probability):
        generator(rd()), distribution(0.0, 1.0), true_probability(true_probability)
        {
            // clamp trueProbability to [0, 1]:
            true_probability = std::max(0.0, std::min(true_probability, 1.0));
        }

    bool toss() {

        return distribution(generator) < true_probability;

    }

private:
    static std::random_device rd;  // used to obtain a seed for the random number engine
    std::mt19937 generator;  // standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<double> distribution;
    float true_probability;

};

std::random_device weighted_coin::rd = std::random_device();

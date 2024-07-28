#pragma once

#include <limits>
#include <random>
#include <type_traits>

namespace Utility {

    template <typename T, T Min = std::numeric_limits<T>::min(), T Max = std::numeric_limits<T>::max()> requires std::is_arithmetic_v<T>
    class Random {
    public:
        static T Gen() noexcept {
            static std::random_device rand_device;
            static std::mt19937_64 generator(rand_device());

            if constexpr (std::is_floating_point_v<T>) {
                std::uniform_real_distribution<T> distribution(Min, Max);
                return distribution(generator);
            }
            else if constexpr (std::is_integral_v<T>) {
                std::uniform_int_distribution<T> distribution(Min, Max);
                return distribution(generator);
            }
        }

        operator T() const noexcept {
            return Random<T, Min, Max>::Gen();
        }
    };

}
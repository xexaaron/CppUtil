#include <random>
#include <type_traits>

namespace Utility { 
    template <typename T> requires (std::is_arithmetic_v<T>)
    class Range {
    public:
        Range(const Range<T>& other) : 
            Min(other.Min), 
            Max(other.Max) 
        {

        }
        
        template <typename U> requires (!std::is_same_v<U, T>)
        Range(const Range<U>& other) : 
            Min(static_cast<T>(other.Min)),
            Max(static_cast<T>(other.Max))
        {

        }

        Range(T min = Type<T>::Min(), T max = Type<T>::Max()) : 
            Min(min), 
            Max(max)
        {

        }

        bool Contains(T value) const {
            return Max >= value && Min <= value;
        }

        bool Intersects(const Range<T>& other) const {
            return !(Max < other.Min || Min > other.Max);
        }
        
        T Random() const {
            std::random_device rd;  
            std::mt19937 gen(rd()); 
            if constexpr (std::is_integral_v<T>) {
                std::uniform_int_distribution<T> dis(Min, Max);
                return dis(gen);
            } else {
                std::uniform_real_distribution<T> dis(Min, Max);
                return dis(gen);
            }
        }

        T Clamp(T value) const {
            return std::clamp(value, Min, Max);
        }

        T Length() const {
            return Max - Min;
        }

        T MidPoint() const {
            return (Min + Max) / 2;
        }

    public:
        bool operator==(const Range<T>& other) const {
            return Min == other.Min && Max == other.Max;
        }

        // Inequality operator
        bool operator!=(const Range<T>& other) const {
            return !(*this == other);
        }
    public:
        T Min;
        T Max;
    };
}
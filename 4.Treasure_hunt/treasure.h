#ifndef TREASURE_H
#define TREASURE_H

#include <concepts>
#include <cstdint>

template<class T>
concept LootType = std::integral<T>;

template <LootType ValueType, bool IsTrapped>
class Treasure {
public:
    constexpr static bool isTrapped = IsTrapped;
    constexpr Treasure(ValueType value) : v(value) { };
    constexpr ValueType evaluate() const { return v; }
    constexpr ValueType getLoot() {
        ValueType res = v;
        v = 0;
        return res;
    }
private:
    ValueType v;
};

template<LootType ValueType>
using SafeTreasure = Treasure<ValueType, false>;

template<LootType ValueType>
using TrappedTreasure = Treasure<ValueType, true>;

#endif // TREASURE_H

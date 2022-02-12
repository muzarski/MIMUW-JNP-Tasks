#ifndef TREASURE_HUNT_H
#define TREASURE_HUNT_H

#include <concepts>
#include <cassert>
#include <utility>
#include "treasure.h"
#include "member.h"

using namespace std;

template <class T>
struct isTreasure : std::false_type {};

template <LootType ValueType, bool IsTrapped>
struct isTreasure<Treasure<ValueType, IsTrapped>> : std::true_type {};

template<class T>
concept EncounterSideTreasure = isTreasure<T>::value;

template<class T>
concept EncounterSideParticipant = requires(T ep) {
    typename T::strength_t;
    { [] () constexpr { return T::isArmed; }() };
    { ep.isArmed } -> convertible_to<bool>;
    { ep.pay() } -> LootType;
    ep.loot(Treasure<decltype(ep.pay()), true>(0));
    ep.loot(Treasure<decltype(ep.pay()), false>(0));
};

template<class T>
concept EncounterSide = EncounterSideTreasure<T> || EncounterSideParticipant<T>;

template<EncounterSide sideA, EncounterSide sideB>
class Encounter {
    public:
        sideA &first;
        sideB &second;
        constexpr Encounter(sideA &s1, sideB &s2) : first(s1), second(s2) { };
};

template<EncounterSideParticipant P, EncounterSideTreasure T>
constexpr void run(Encounter<P, T> e) {
    e.first.loot(move(e.second));
}

template<EncounterSideParticipant P, EncounterSideTreasure T>
constexpr void run(Encounter<T, P> e) {
    run(Encounter{e.second, e.first});
}

template<EncounterSideParticipant P1, EncounterSideParticipant P2>
constexpr void run(Encounter<P1, P2> e) {
    if (P1::isArmed || P2::isArmed) {
        if (!P1::isArmed) {
            e.second.takeOverTreasures(e.first);
        }
        else if (!P2::isArmed) {
            e.first.takeOverTreasures(e.second);
        }
    }
}

template<EncounterSideParticipant P1, EncounterSideParticipant P2>
constexpr void run(Encounter<P1, P2> e) requires (P1::isArmed && P2::isArmed) {
    if (e.first.getStrength() > e.second.getStrength()) {
        e.first.takeOverTreasures(e.second);
    }
    else if (e.first.getStrength() < e.second.getStrength()) {
        e.second.takeOverTreasures(e.first);
    }
}

void expedition() {}

template<class Arg>
constexpr void expedition(Arg arg) {
    run (arg);
}

template<class Head, class... Tail>
constexpr void expedition(Head head, Tail... tail) {
    expedition(head);
    expedition(tail...);
}

#endif // TREASURE_HUNT_H

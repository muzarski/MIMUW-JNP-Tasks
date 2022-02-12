#ifndef MEMBER_H
#define MEMBER_H

#include <concepts>
#include <cassert>
#include "treasure.h"

using namespace std;

namespace {
    constexpr uint32_t fib(size_t n) {
        if (n == 0) return 0;
        if (n == 1) return 1;
        int prev = 1;
        int cur = 1;
        int tmp;
        for (size_t i = 3; i <= n; i++) {
            tmp = prev + cur;
            prev = cur;
            cur = tmp;
        }
        return cur;
    }
}

template<LootType ValueType, bool IsArmed, size_t CompletedExpeditions, bool IsVeteran>
class ExpedtitionParticipant {
    public:
        using strength_t = uint32_t;
        constexpr static bool isArmed = IsArmed;

        constexpr ExpedtitionParticipant() requires (!IsArmed || (IsVeteran && CompletedExpeditions < 25)) {
            possession = 0;
            strength_ = 0;
            if (IsArmed) strength_ = fib(CompletedExpeditions);
        }

        constexpr ExpedtitionParticipant(strength_t strength) requires (IsArmed && !IsVeteran) {
            possession = 0;
            strength_ = strength;
        }

        constexpr strength_t getStrength() const requires IsArmed {
            return strength_;
        }

        template<bool IsTrapped>
        constexpr void loot(Treasure<ValueType, IsTrapped> &&treasure) {
            if (IsTrapped) {
                if (IsVeteran && strength_ > 0) {
                    possession += treasure.getLoot();
                }
                else if (IsArmed && strength_ > 0) {
                    strength_ /= 2;
                    possession += treasure.getLoot();
                }
            }
            else {
                possession += treasure.getLoot();
            }
        }

        constexpr ValueType pay() {
            ValueType res = possession;
            possession = 0;
            return res;
        }

        template<bool IsOtherArmed, size_t OtherExpeditions, bool IsOtherVeteran>
        constexpr void takeOverTreasures(ExpedtitionParticipant<ValueType, IsOtherArmed,
                OtherExpeditions, IsOtherVeteran> &otherParticipant) {
            possession += otherParticipant.pay();
        }
    private:
        ValueType possession;
        strength_t strength_;
};

template<LootType ValueType, bool IsArmed>
using Adventurer = ExpedtitionParticipant<ValueType, IsArmed, 0, false>;

template<LootType ValueType>
using Explorer = Adventurer<ValueType, false>;

template<LootType ValueType, size_t CompletedExpeditions>
using Veteran = ExpedtitionParticipant<ValueType, true, CompletedExpeditions, true>;

#endif // MEMBER_H

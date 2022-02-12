#ifndef TRI_LIST_H
#define TRI_LIST_H

#include "tri_list_concepts.h"
#include <vector>
#include <variant>
#include <functional>
#include <tuple>

using std::ranges::views::transform;
using std::ranges::views::filter;

template<typename T>
auto identity = [](T t) { return t; };

template<typename T>
auto compose(modifier<T> auto m1, modifier<T> auto m2) {
    return [_m1 = std::move(m1), _m2 = std::move(m2)](T t) mutable {
        return _m1(_m2(t));
    };
}

// Checks whether T is same_as the EXACTLY ONE of the T1, T2, T3.
template<typename T, typename T1, typename T2, typename T3>
concept valid_arg = (std::same_as<T, T1> && !std::same_as<T, T2> && !std::same_as<T, T3>)
                 || (std::same_as<T, T2> && !std::same_as<T, T1> && !std::same_as<T, T3>)
                 || (std::same_as<T, T3> && !std::same_as<T, T1> && !std::same_as<T, T2>);

template<typename T1, typename T2, typename T3>
class tri_list {
private:
    using variant_t = std::variant<T1, T2, T3>;
    using variant_mod_t = std::function<variant_t(variant_t)>;

    std::vector<std::variant<T1, T2, T3>> l;

    // Represent modifications over the types. funcs[x - 1] modifies values of the type Tx.
    std::vector<variant_mod_t> funcs{identity<variant_t>, identity<variant_t>, identity<variant_t>};

    // Returns a reference to the function that modifies T.
    template <typename T>
    variant_mod_t &get_fun() {
        if constexpr (std::same_as<T, T1>) {
            return funcs[0];
        }
        else if constexpr (std::same_as<T, T2>) {
            return funcs[1];
        }
        else {
            return funcs[2];
        }
    }

    // Maps every element (based on the type).
    const std::function<variant_t(variant_t)> _map = [this](variant_t e) -> variant_t {
        return funcs[e.index()](e);
    };

    // View over the modified elements.
    using view_t = decltype(transform(l, _map));
    const view_t view = transform(l, _map);

public:
    tri_list() = default;
    tri_list(std::initializer_list<std::variant<T1, T2, T3>> args) {
        for (const auto &e : args) {
            l.push_back(e);
        }
    };

    template<valid_arg<T1, T2, T3> T>
    void push_back(const T &t) {
        l.push_back(t);
    }

    template<valid_arg<T1, T2, T3> T, modifier<T> F>
    void modify_only(F m = F{}) {
        auto variant_mod = [&](variant_t e) -> variant_t { return m(std::get<T>(e)); };
        get_fun<T>() = compose<variant_t>(variant_mod, std::move(get_fun<T>()));
    }

    template<valid_arg<T1, T2, T3> T>
    void reset() {
        get_fun<T>() = identity<variant_t>;
    }

    template<valid_arg<T1, T2, T3> T>
    auto range_over() const {
        return view | filter([](auto e) { return std::holds_alternative<T>(e); })
                    | transform([](auto e) { return std::get<T>(e); });
    }

    auto begin() const {
        return view.begin();
    }

    auto end() const {
        return view.end();
    }
};

#endif // TRI_LIST_H

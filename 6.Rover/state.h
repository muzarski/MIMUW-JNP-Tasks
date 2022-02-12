#ifndef STATE_H
#define STATE_H

#include "direction.h"
#include <set>
#include <utility>
#include <vector>

class RoverDidNotLand : public std::logic_error {
public:
    RoverDidNotLand() : std::logic_error("RoverDidNotLand") {};
};

class State {
private:
    position_t position;
    std::shared_ptr<Direction> direction;
    bool is_stopped;

protected:
    bool is_unknown;

public:
    State(bool _is_unknown) : is_stopped(true), is_unknown(_is_unknown) {};

    State(position_t _position, std::shared_ptr<Direction> _direction)
            : position(std::move(_position)), direction(std::move(_direction)), is_stopped(false), is_unknown(false) { };

    const bool& stopped() const { return is_stopped; };
    void stop() { is_stopped = true; };

    virtual void move_forward(const sensors_t &sensors) {
        direction->move_forward(is_stopped, position, sensors);
    }

    virtual void move_backward(const sensors_t &sensors) {
        direction->move_backward(is_stopped, position, sensors);
    }

    virtual void rotate_right() {
        direction->rotate_right(direction);
    }

    virtual void rotate_left() {
        direction->rotate_left(direction);
    }

    friend std::ostream &operator<<(std::ostream &out, const State &state);
};

using state_ptr = std::shared_ptr<State>;

std::ostream &operator<<(std::ostream &out, const State &state) {

    if (state.is_unknown) {
        out << "unknown";
    }
    else {
        out << "("
            << state.position.first << ", "
            << state.position.second
            << ") "
            << *state.direction;

        if (state.is_stopped) {
            out << " stopped";
        }
    }

    return out;
}

class StateUNKNOWN : public State {
public:
    StateUNKNOWN() : State(true) {};

    void move_forward([[maybe_unused]] const sensors_t &sensors) override {
        throw RoverDidNotLand();
    }

    void move_backward([[maybe_unused]] const sensors_t &sensors) override {
        throw RoverDidNotLand();
    }

    void rotate_right() override {
        throw RoverDidNotLand();
    }

    void rotate_left() override {
        throw RoverDidNotLand();
    }
};

#endif // STATE_H

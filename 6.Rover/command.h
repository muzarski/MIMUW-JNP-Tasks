#ifndef COMMAND_H
#define COMMAND_H

#include "state.h"

class Command {
public:
    virtual void execute(const state_ptr &state, const sensors_t &sensors) = 0;
};

using command_ptr = std::shared_ptr<Command>;

class MoveForward : public Command {
public:
    void execute(const state_ptr &state, const sensors_t &sensors) override {
        state->move_forward(sensors);
    }
};

class MoveBackward : public Command {
public:
    void execute(const state_ptr &state, const sensors_t &sensors) override {
        state->move_backward(sensors);
    }
};

class RotateRight : public Command {
public:
    void execute(const state_ptr &state, [[maybe_unused]] const sensors_t &sensors) override {
        state->rotate_right();
    }
};

class RotateLeft : public Command {
public:
    void execute(const state_ptr &state, [[maybe_unused]] const sensors_t &sensors) override {
        state->rotate_left();
    }
};

class Compose : public Command {
private:
    std::vector<command_ptr> commands;
    
public:

    Compose(std::vector<command_ptr> _commands) : commands(std::move(_commands)) { }

    void execute(const state_ptr &state, const sensors_t &sensors) override {

        for (const command_ptr &command : commands) {
            command->execute(state, sensors);
            if (state->stopped())
                return;
        }
    }
};

std::shared_ptr<MoveForward> move_forward() {
    return std::make_shared<MoveForward>();
}

std::shared_ptr<MoveBackward> move_backward() {
    return std::make_shared<MoveBackward>();
}

std::shared_ptr<RotateLeft> rotate_left() {
    return std::make_shared<RotateLeft>();
}

std::shared_ptr<RotateRight> rotate_right() {
    return std::make_shared<RotateRight>();
}

std::shared_ptr<Compose> compose(std::vector<command_ptr> &&commands) {
    return std::make_shared<Compose>(std::move(commands));
}

#endif // COMMAND_H

#ifndef ROVER_H
#define ROVER_H

#include "command.h"
#include <map>
#include <set>

class Rover {
private:
    state_ptr state = std::make_shared<StateUNKNOWN>();
    std::map<char, command_ptr> commands;
    sensors_t sensors;

    friend class RoverBuilder;

    void add_command(char c, const command_ptr &command) {
        commands[c] = command;
    }

    void add_sensor(std::unique_ptr<Sensor> &&sensor) {
        sensors.insert(std::move(sensor));
    }

public:
    void execute(const std::string &list) const {
        for (char command : list) {
            if (commands.contains(command)) {
                commands.find(command)->second->execute(state, sensors);
            }
            else {
                state->stop();
                return;
            }
        }
    }

    void land(position_t &&position, const std::shared_ptr<Direction> &direction) {
        state = std::make_shared<State>(position, direction);
    }

    friend std::ostream &operator<<(std::ostream &out, const Rover &rover);
};

std::ostream &operator<<(std::ostream &out, const Rover &rover) {
    out << *rover.state;
    return out;
}

class RoverBuilder {
private:
    Rover built;

public:
    RoverBuilder& program_command(char c, const command_ptr &command) {
        built.add_command(c, command);
        return *this;
    }

    RoverBuilder& add_sensor(std::unique_ptr<Sensor> &&sensor) {
        built.add_sensor(std::move(sensor));
        return *this;
    }

    Rover build() {
        return built;
    }
};

#endif // ROVER_H

#ifndef DIRECTION_H
#define DIRECTION_H

#include <memory>
#include <set>
#include <utility>
#include <vector>

using coordinate_t = int;
using position_t = std::pair<coordinate_t, coordinate_t>;

class Sensor {
public:
    virtual bool is_safe(coordinate_t x, coordinate_t y) = 0;
};

using sensors_t = std::set<std::shared_ptr<Sensor>>;
using sensor_ptr = std::shared_ptr<Sensor>;

class Direction {
protected:
    static bool is_safe(const sensors_t &sensors, coordinate_t x, coordinate_t y) {
        for (const sensor_ptr &sensor : sensors) {
            if (!sensor->is_safe(x, y))
                return false;
        }
        return true;
    }

    [[nodiscard]] virtual std::string get_direction() const = 0;

public:
    static const std::shared_ptr<Direction> NORTH;
    static const std::shared_ptr<Direction> SOUTH;
    static const std::shared_ptr<Direction> WEST;
    static const std::shared_ptr<Direction> EAST;

    friend std::ostream &operator<<(std::ostream &out, const Direction &direction);

    virtual void move_forward(bool &is_stopped, position_t &position, const sensors_t &sensors) = 0;
    virtual void move_backward(bool &is_stopped, position_t &position, const sensors_t &sensors) = 0;
    virtual void rotate_right(std::shared_ptr<Direction> &rovers_direction) = 0;
    virtual void rotate_left(std::shared_ptr<Direction> &rovers_direction) = 0;
};

class DirectionNORTH : public Direction {
protected:
    [[nodiscard]] std::string get_direction() const override { return "NORTH"; }

public:
    void move_forward(bool &is_stopped, position_t &position, const sensors_t &sensors) override {
        if (!is_safe(sensors, position.first, position.second + 1)) {
            is_stopped = true;
            return;
        }
        ++position.second;
        is_stopped = false;
    }

    void move_backward(bool &is_stopped, position_t &position, const sensors_t &sensors) override {
        if (!is_safe(sensors, position.first, position.second - 1)) {
            is_stopped = true;
            return;
        }
        --position.second;
        is_stopped = false;
    }

    void rotate_right(std::shared_ptr<Direction> &rovers_direction) override {
        rovers_direction = Direction::EAST;
    }

    void rotate_left(std::shared_ptr<Direction> &rovers_direction) override {
        rovers_direction = Direction::WEST;
    }
};

class DirectionSOUTH : public Direction {
protected:
    [[nodiscard]] std::string get_direction() const override { return "SOUTH"; }

public:
    void move_forward(bool &is_stopped, position_t &position, const sensors_t &sensors) override {
        if (!is_safe(sensors, position.first, position.second - 1)) {
            is_stopped = true;
            return;
        }
        --position.second;
        is_stopped = false;
    }

    void move_backward(bool &is_stopped, position_t &position, const sensors_t &sensors) override {
        if (!is_safe(sensors, position.first, position.second + 1)) {
            is_stopped = true;
            return;
        }
        ++position.second;
        is_stopped = false;
    }

    void rotate_right(std::shared_ptr<Direction> &rovers_direction) override {
        rovers_direction = Direction::WEST;
    }

    void rotate_left(std::shared_ptr<Direction> &rovers_direction) override {
        rovers_direction = Direction::EAST;
    }
};

class DirectionWEST : public Direction {
protected:
    [[nodiscard]] std::string get_direction() const override { return "WEST"; }

public:
    void move_forward(bool &is_stopped, position_t &position, const sensors_t &sensors) override {
        if (!is_safe(sensors, position.first - 1, position.second)) {
            is_stopped = true;
            return;
        }
        --position.first;
        is_stopped = false;
    }

    void move_backward(bool &is_stopped, position_t &position, const sensors_t &sensors) override {
        if (!is_safe(sensors, position.first + 1, position.second)) {
            is_stopped = true;
            return;
        }
        ++position.first;
        is_stopped = false;
    }

    void rotate_right(std::shared_ptr<Direction> &rovers_direction) override {
        rovers_direction = Direction::NORTH;
    }

    void rotate_left(std::shared_ptr<Direction> &rovers_direction) override {
        rovers_direction = Direction::SOUTH;
    }
};

class DirectionEAST : public Direction {
protected:
    [[nodiscard]] std::string get_direction() const override { return "EAST"; }

public:
    void move_forward(bool &is_stopped, position_t &position, const sensors_t &sensors) override {
        if (!is_safe(sensors, position.first + 1, position.second)) {
            is_stopped = true;
            return;
        }
        ++position.first;
        is_stopped = false;
    }

    void move_backward(bool &is_stopped, position_t &position, const sensors_t &sensors) override {
        if (!is_safe(sensors, position.first - 1, position.second)) {
            is_stopped = true;
            return;
        }
        --position.first;
        is_stopped = false;
    }

    void rotate_right(std::shared_ptr<Direction> &rovers_direction) override {
        rovers_direction = Direction::SOUTH;
    }

    void rotate_left(std::shared_ptr<Direction> &rovers_direction) override {
        rovers_direction = Direction::NORTH;
    }
};

std::ostream &operator<<(std::ostream &out, const Direction &direction) {
    out << direction.get_direction();
    return out;
}

const std::shared_ptr<Direction> Direction::NORTH = std::make_shared<DirectionNORTH>();
const std::shared_ptr<Direction> Direction::SOUTH = std::make_shared<DirectionSOUTH>();
const std::shared_ptr<Direction> Direction::WEST = std::make_shared<DirectionWEST>();
const std::shared_ptr<Direction> Direction::EAST = std::make_shared<DirectionEAST>();

#endif // DIRECTION_H

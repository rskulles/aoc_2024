#include <iostream>
#include<fstream>
#include <ostream>
#include<string>
#include<vector>
#include<sstream>

#define CHANGE_TO_REBUILD 12

class Position {
public:
    friend bool operator==(const Position &lhs, const Position &rhs) {
        return lhs.x == rhs.x
               && lhs.y == rhs.y
               && lhs.altitude == rhs.altitude;
    }

    friend bool operator!=(const Position &lhs, const Position &rhs) {
        return !(lhs == rhs);
    }

    int64_t x;
    int64_t y;
    int64_t altitude;
};


struct PossibleMovePositions {
    Position up;
    Position down;
    Position left;
    Position right;
};

PossibleMovePositions get_possible_moves(const Position &pos) {
    return PossibleMovePositions{
        {pos.x, pos.y - 1},
        {pos.x, pos.y + 1},
        {pos.x - 1, pos.y},
        {pos.x + 1, pos.y}
    };
}

class TrailCursor {
public:
    enum class Direction {
        Up,
        Down,
        Left,
        Right
    };

    explicit TrailCursor(const Direction &direction, const Position &start_position): _direction(direction),
        _last_position(start_position),
        _start_position(start_position), _current_position(start_position) {
    }

    [[nodiscard]] const Position &get_start_position() const {
        return _start_position;
    }

    [[nodiscard]] const Position &current_position() const {
        return _current_position;
    }

    [[nodiscard]] const Position &last_position() const {
        return _last_position;
    }

    [[nodiscard]] Direction direction() const {
        return _direction;
    }

    void set_position(const Position &position) {
        _current_position = {position.x, position.y, position.altitude};
    }

    void move() {
        if (stopped()) {
            return;
        }
        _last_position = {_current_position.x, _current_position.y, _current_position.altitude};
        switch (_direction) {
            case Direction::Up:
                _current_position.y--;
                break;
            case Direction::Down:
                _current_position.y++;
                break;
            case Direction::Left:
                _current_position.x--;
                break;
            case Direction::Right:
                _current_position.x++;
                break;
        }
    }

    void set_altitude(const int64_t &altitude) {
        _current_position.altitude = altitude;
    }

    [[nodiscard]] bool moved_legally() const {
        return (_current_position.altitude - _last_position.altitude) == 1;
    }

    [[nodiscard]] bool stopped() const {
        return _current_position.altitude == 9;
    }

private:
    Direction _direction;
    Position _start_position;
    Position _current_position;
    Position _last_position;
};

class TopologyMap : public std::vector<std::vector<int> > {
public:
    explicit TopologyMap(const std::vector<std::string> &strings) {
        for (const auto &row: strings) {
            std::vector<int> temp;
            temp.reserve(row.size());
            for (int i = 0; i < row.size(); ++i) {
                temp.push_back(std::strtol(row.substr(i, 1).c_str(), nullptr, 10));
            }
            push_back(temp);
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const TopologyMap &obj) {
        std::stringstream ss;
        for (const auto &row: obj) {
            for (const auto &col: row) {
                ss << col;
            }
            ss << "\n";
        }
        return os << ss.str();
    }

    [[nodiscard]] uint64_t walk(const int &col, const int &row, const bool &unique = true) const {
        std::vector<TrailCursor *> trail_cursors;

        trail_cursors.push_back(new TrailCursor(TrailCursor::Direction::Down, Position{col, row,0}));
        trail_cursors.push_back(new TrailCursor(TrailCursor::Direction::Up, Position{col, row,0}));
        trail_cursors.push_back(new TrailCursor(TrailCursor::Direction::Left, Position{col, row,0}));
        trail_cursors.push_back(new TrailCursor(TrailCursor::Direction::Right, Position{col, row,0}));

        do {
            auto len = trail_cursors.size();
            for (auto i = 0; i < len; ++i) {
                auto &trail_cursor = trail_cursors[i];

                if (trail_cursor == nullptr) {
                    continue;
                }

                trail_cursor->move();

                auto current_position = trail_cursor->current_position();
                if (valid_position(current_position)) {
                    trail_cursor->set_altitude((*this)[current_position.y][current_position.x]);
                    current_position = trail_cursor->current_position();
                }

                if (!trail_cursor->moved_legally()) {
                    delete trail_cursor;
                    trail_cursor = nullptr;
                }

                if (trail_cursor != nullptr && !trail_cursor->stopped()) {
                    auto new_positions = get_possible_moves(trail_cursor->current_position());
                    auto current_direction = trail_cursor->direction();
                    if (valid_position(new_positions.up) && current_direction != TrailCursor::Direction::Up &&
                        current_direction != TrailCursor::Direction::Down) {
                        trail_cursors.push_back(new TrailCursor(TrailCursor::Direction::Up, {col, row, 0}));
                        trail_cursors.back()->set_altitude((*this)[current_position.y][current_position.x]);
                        trail_cursors.back()->set_position(current_position);
                    }

                    if (valid_position(new_positions.down) && current_direction != TrailCursor::Direction::Up &&
                        current_direction != TrailCursor::Direction::Down) {
                        trail_cursors.push_back(new TrailCursor(TrailCursor::Direction::Down, {col, row, 0}));
                        trail_cursors.back()->set_altitude((*this)[current_position.y][current_position.x]);
                        trail_cursors.back()->set_position(current_position);
                    }

                    if (valid_position(new_positions.left) && current_direction != TrailCursor::Direction::Left &&
                        current_direction != TrailCursor::Direction::Right) {
                        trail_cursors.push_back(new TrailCursor(TrailCursor::Direction::Left, {col, row, 0}));
                        trail_cursors.back()->set_altitude((*this)[current_position.y][current_position.x]);
                        trail_cursors.back()->set_position(current_position);
                    }

                    if (valid_position(new_positions.right) && current_direction != TrailCursor::Direction::Left &&
                        current_direction != TrailCursor::Direction::Right) {
                        trail_cursors.push_back(new TrailCursor(TrailCursor::Direction::Right, {col, row, 0}));
                        trail_cursors.back()->set_altitude((*this)[current_position.y][current_position.x]);
                        trail_cursors.back()->set_position(current_position);
                    }
                }
            }
        } while (can_continue(trail_cursors));

        std::vector<TrailCursor *> output_trail_cursors;
        if (unique) {
            for (auto &trail_cursor: trail_cursors) {
                if (trail_cursor != nullptr && trail_cursor->moved_legally() && trail_cursor->stopped()) {
                    auto in_vector = false;
                    for (const auto &trail_cursor2: output_trail_cursors) {
                        if (trail_cursor->current_position() == trail_cursor2->current_position() && trail_cursor2->
                            get_start_position() == trail_cursor->get_start_position()) {
                            in_vector = true;
                        }
                    }
                    if (!in_vector) {
                        output_trail_cursors.push_back(trail_cursor);
                    }
                }
            }
        }else {

            for (auto &trail_cursor: trail_cursors) {
                if (trail_cursor != nullptr && trail_cursor->moved_legally() && trail_cursor->stopped()) {
                    output_trail_cursors.push_back(trail_cursor);
                }
            }
        }
        const auto size = output_trail_cursors.size();
        for (const auto &trail_cursor: output_trail_cursors) {
            delete trail_cursor;
        }
        return size;
    }

private:
    [[nodiscard]] bool valid_position(const Position &position) const {
        if (position.x < 0 || position.y < 0 || position.y >= size() || position.x >= (*this)[position.y].size()) {
            return false;
        }
        return true;
    }

    [[nodiscard]] static bool can_continue(const std::vector<TrailCursor *> &cursors) {
        auto all_empty = true;
        auto all_stopped = true;
        for (const auto &cursor: cursors) {
            if (cursor != nullptr) {
                all_empty = false;
                if (!cursor->stopped()) {
                    all_stopped = false;
                }
            }
        }
        return !all_empty && !all_stopped;
    }
};


uint64_t day_10_1(const TopologyMap &map);
uint64_t day_10_2(const TopologyMap &map);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
        lines.push_back(line);
    }

    TopologyMap map(lines);
    const auto result_1 = day_10_1(map);
    std::cout << "Result 1: " << result_1 << std::endl;

    const auto result_2 = day_10_2(map);
    std::cout << "Result 2: " << result_2 << std::endl;
}

uint64_t day_10_1(const TopologyMap &map) {
    uint64_t sum = 0;
    for (int row = 0; row < map.size(); ++row) {
        for (int col = 0; col < map[row].size(); ++col) {
            if (map[row][col] == 0) {
                sum += map.walk(col, row);
            }
        }
    }
    return sum;
}

uint64_t day_10_2(const TopologyMap &map) {
    uint64_t sum = 0;
    for (int row = 0; row < map.size(); ++row) {
        for (int col = 0; col < map[row].size(); ++col) {
            if (map[row][col] == 0) {
                sum += map.walk(col, row,false);
            }
        }
    }
    return sum;
}

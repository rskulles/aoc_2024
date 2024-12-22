#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdint>
#include <map>
#include<sstream>
#define CHANGE_TO_REBUILD 12

struct Vector2 {
    friend bool operator==(const Vector2 &lhs, const Vector2 &rhs) {
        return lhs.x == rhs.x
               && lhs.y == rhs.y;
    }

    friend bool operator!=(const Vector2 &lhs, const Vector2 &rhs) {
        return !(lhs == rhs);
    }

    int x;
    int y;
};

struct Sequence {
    std::string inputs_x;
    std::string inputs_y;

    [[nodiscard]] std::string position_primary() const {
        if (primary == 'x') {
            return inputs_x + inputs_y + 'A';
        }
        return inputs_y + inputs_x + 'A';
    }

    [[nodiscard]] std::string position_secondary() const {
        if (locked) {
            return position_primary();
        }
        if (primary == 'x') {
            return inputs_y + inputs_x + 'A';
        }
        return inputs_x + inputs_y + 'A';
    }

    char primary = 'x';
    bool locked{false};
};

class Sequences : public std::vector<Sequence> {
public:
    std::string to_string() const {
        std::stringstream ss;
        for (const Sequence &seq: *this) {
            ss << seq.position_primary();
        }
        return ss.str();
    }
};

class Puzzle {
public:
    Puzzle() = delete;

    explicit Puzzle(const std::vector<std::string> &input): _codes(input) {
    }

    [[nodiscard]] uint64_t get_code_complexity(const size_t &key_pad_count) const {
        uint64_t complexity = 0;
        for (const auto &code: _codes) {
            uint64_t sequence_size = 0;
            auto door_pad_inputs = get_direction_pad_input_for_door(code);
            std::map<char,std::map<char,std::string>> direction_cache;
            for (const auto &door_pad_input: door_pad_inputs) {
                auto p = door_pad_input.position_primary();
                auto s = door_pad_input.position_secondary();

                for (int i = 0; i < key_pad_count; ++i) {
                    p = get_direction_pad_input_for_direction_pad(p,direction_cache);
                    s = get_direction_pad_input_for_direction_pad(s,direction_cache);
                }

                const auto primary_steps = p.size();
                const auto secondary_steps = s.size();;

                if (primary_steps < secondary_steps) {
                    sequence_size += primary_steps;
                } else {
                    sequence_size += secondary_steps;
                }
            }
            const auto number = get_int_from_string(code);
            const auto current_complexity = number * sequence_size;
            complexity += current_complexity;
        }
        return complexity;
    }

private:
    [[nodiscard]] static int get_int_from_string(const std::string &s) {
        std::stringstream ss;
        for (const auto &c: s) {
            if (std::isdigit(c)) {
                ss << c;
            }
        }
        return std::stoi(ss.str());
    }

    [[nodiscard]] static Vector2 get_direction_pad_key_position(const char &key) {
        switch (key) {
            case 'A':
                return Vector2{2, 0};
            case '^':
                return Vector2{1, 0};
            case '>':
                return Vector2{2, 1};
            case 'v':
                return Vector2{1, 1};
            case '<':
                return Vector2{0, 1};
            default:
                return Vector2{-1, -1};
        }
    }

    [[nodiscard]] static Vector2 get_door_key_position(const char &key) {
        switch (key) {
            case 'A':
                return Vector2{2, 3};
            case '0':
                return Vector2{1, 3};
            case '3':
                return Vector2{2, 2};
            case '2':
                return Vector2{1, 2};
            case '1':
                return Vector2{0, 2};
            case '6':
                return Vector2{2, 1};
            case '5':
                return Vector2{1, 1};
            case '4':
                return Vector2{0, 1};
            case '9':
                return Vector2{2, 0};
            case '8':
                return Vector2{1, 0};
            case '7':
                return Vector2{0, 0};
            default:
                return Vector2{-1, -1};
        }
    }

    [[nodiscard]] static Vector2 get_distance_direction_keypad(const char &current_key, const char &next_key) {
        Vector2 distance{0, 0};
        const Vector2 current_position = get_direction_pad_key_position(current_key);
        const Vector2 next_position = get_direction_pad_key_position(next_key);
        distance.x = next_position.x - current_position.x;
        distance.y = next_position.y - current_position.y;
        return distance;
    }

    [[nodiscard]] static Vector2 get_distance_door_keypad(const char &current_key, const char &next_key) {
        Vector2 distance{0, 0};
        const Vector2 current_position = get_door_key_position(current_key);
        const Vector2 next_position = get_door_key_position(next_key);
        distance.x = next_position.x - current_position.x;
        distance.y = next_position.y - current_position.y;
        return distance;
    }

    [[nodiscard]] static std::string get_direction_pad_input_for_direction_pad(const std::string &directions, std::map<char,std::map<char,std::string>>& direction_cache) {
        std::stringstream ss;
        std::stringstream s;
        char current_key = 'A';
        constexpr Vector2 invalid_position{0, 0};

        for (const auto &direction: directions) {
            s.str(std::string());
            if (!direction_cache[current_key][direction].empty()) {
                ss<<direction_cache[current_key][direction];
                current_key = direction;
                continue;
            }
            auto [x,y] = get_distance_direction_keypad(current_key, direction);
            if (x == 0 && y == 0) {
                s << 'A';
            } else if (x == 0) {
                const auto opposite_direction = y < 0 ? 1 : -1;
                while (y != 0) {
                    s << (opposite_direction > 0 ? '^' : 'v');
                    y += opposite_direction;
                }
                s << "A";
            } else if (y == 0) {
                const auto opposite_direction = x < 0 ? 1 : -1;
                while (x != 0) {
                    s << ((opposite_direction > 0 ? "<" : ">"));
                    x += opposite_direction;
                }
                s << "A";
            } else {
                const auto opposite_x = x < 0 ? 1 : -1;
                const auto opposite_y = y < 0 ? 1 : -1;
                const auto current_position = get_direction_pad_key_position(current_key);
                const Vector2 x_check{current_position.x + x, current_position.y};
                const Vector2 y_check{current_position.x, current_position.y + y};

                if (x_check != invalid_position && y_check != invalid_position) {
                    while (x != 0) {
                        s << (opposite_x > 0 ? '<' : '>');
                        x += opposite_x;
                    }
                    while (y != 0) {
                        s << (opposite_y > 0 ? '^' : 'v');
                        y += opposite_y;
                    }
                } else if (x_check == invalid_position) {
                    while (y != 0) {
                        s << (opposite_y > 0 ? '^' : 'v');
                        y += opposite_y;
                    }

                    while (x != 0) {
                        s << (opposite_x > 0 ? '<' : '>');
                        x += opposite_x;
                    }
                } else if (y_check == invalid_position) {
                    while (x != 0) {
                        s << (opposite_x > 0 ? '<' : '>');
                        x += opposite_x;
                    }
                    while (y != 0) {
                        s << (opposite_y > 0 ? '^' : 'v');
                        y += opposite_y;
                    }
                }

                s << 'A';
            }
            direction_cache[current_key][direction]=s.str();
            ss<<s.str();
            current_key = direction;
        }
        return ss.str();
    }

    [[nodiscard]] static Sequences get_direction_pad_input_for_door(const std::string &code) {
        Sequences s;
        char current_key = 'A';
        const Vector2 invalid_position{0, 3};

        for (const char &key: code) {
            Sequence &current_sequence = s.emplace_back();
            std::string input;
            auto [x, y] = get_distance_door_keypad(current_key, key);
            if (x == 0 && y == 0) {
                current_sequence.locked = true;
            } else if (x == 0) {
                current_sequence.primary = 'y';
                const auto opposite_direction = y < 0 ? 1 : -1;
                while (y != 0) {
                    current_sequence.inputs_y += (opposite_direction > 0 ? '^' : 'v');
                    y += opposite_direction;
                }
                current_sequence.locked = true;
            } else if (y == 0) {
                const auto opposite_direction = x < 0 ? 1 : -1;
                while (x != 0) {
                    current_sequence.inputs_x += (opposite_direction > 0 ? '<' : '>');
                    x += opposite_direction;
                }
                current_sequence.locked = true;
            } else {
                const auto opposite_x = x < 0 ? 1 : -1;
                const auto opposite_y = y < 0 ? 1 : -1;
                const auto current_position = get_door_key_position(current_key);
                Vector2 x_check{current_position.x + x, current_position.y};
                Vector2 y_check{current_position.x, current_position.y + y};

                if (x_check != invalid_position && y_check != invalid_position) {
                    while (x != 0) {
                        current_sequence.inputs_x += (opposite_x > 0 ? '<' : '>');
                        x += opposite_x;
                    }

                    while (y != 0) {
                        current_sequence.inputs_y += (opposite_y > 0 ? '^' : 'v');
                        y += opposite_y;
                    }
                } else if (x_check == invalid_position) {
                    current_sequence.primary = 'y';
                    while (y != 0) {
                        current_sequence.inputs_y += (opposite_y > 0 ? '^' : 'v');
                        y += opposite_y;
                    }

                    while (x != 0) {
                        current_sequence.inputs_x += (opposite_x > 0 ? '<' : '>');
                        x += opposite_x;
                    }
                    current_sequence.locked = true;
                } else if (y_check == invalid_position) {
                    while (x != 0) {
                        current_sequence.inputs_x += (opposite_x > 0 ? '<' : '>');
                        x += opposite_x;
                    }
                    while (y != 0) {
                        current_sequence.inputs_y += (opposite_y > 0 ? '^' : 'v');
                        y += opposite_y;
                    }
                    current_sequence.locked = true;
                }
            }
            current_key = key;
        }
        return s;
    }

    const std::vector<std::string> _codes;
};

uint64_t day_x_1(const Puzzle &puzzle);

uint64_t day_x_2(const Puzzle &puzzle);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
        lines.push_back(line);
    }
    const Puzzle puzzle(lines);

    const auto result_1 = day_x_1(puzzle);
    std::cout << "Result 1: " << result_1 << "\n"<<std::flush;
    const auto result_2 = day_x_2(puzzle);
    std::cout << "Result 2: " << result_2 << "\n"<<std::flush;;
}

uint64_t day_x_1(const Puzzle &puzzle) {
    return puzzle.get_code_complexity(2);
}

uint64_t day_x_2(const Puzzle &puzzle) {
    return puzzle.get_code_complexity(26);
}

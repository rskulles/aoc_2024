#include <iostream>
#include<fstream>
#include <map>
#include <ostream>
#include<string>
#include<vector>
#include<SFML/Graphics.hpp>
#define PUZZLE_WIDTH 101
#define PUZZLE_HEIGHT 103
#define PUZZLE_MID_WIDTH  PUZZLE_WIDTH / 2
#define PUZZLE_MID_HEIGHT PUZZLE_HEIGHT / 2
#define CHANGE_TO_REBUILD 1

#define START_POS "\033[1;1H"
#define CLEAR_SCREEN "\033[2J\033[H"

struct Vector2 {
    int x;
    int y;
};

class Robot {
public:
    explicit Robot(const Vector2 &position, const Vector2 &velocity): _position(position), _velocity(velocity) {
    }

    explicit Robot(const std::string &str): _position({0, 0}), _velocity({0, 0}) {
        auto position_velocity_split = str.find(" ");
        if (position_velocity_split == std::string::npos) {
            throw std::invalid_argument("Not a valid definition");
        }
        const auto pos = str.substr(2, position_velocity_split - 1);
        auto split = pos.find(",");
        _position.x = std::stoi(pos.substr(0, split));
        _position.y = std::stoi(pos.substr(split + 1));
        auto vel = str.substr(position_velocity_split + 3);
        split = vel.find(",");
        _velocity.x = std::stoi(vel.substr(0, split));
        _velocity.y = std::stoi(vel.substr(split + 1));
    }

    [[nodiscard]] Vector2 get_position() const {
        return _position;
    }

    void update(float dt) {
        _position.x += _velocity.x * dt;
        _position.y += _velocity.y * dt;
    }

    void teleport_to(const Vector2 &vector2) {
        _position = vector2;
    }

private:
    Robot() = default;

    Vector2 _position;
    Vector2 _velocity;
};

class Puzzle {
public:
    std::vector<std::vector<int> > get_puzzle_state() const {
        std::vector<std::vector<int> > puzzle;
        puzzle.resize(_height);
        for (int row = 0; row < _height; ++row) {
            puzzle[row].resize(_width);
            for (int col = 0; col < _width; ++col) {
                puzzle[row][col] = 0;
            }
        }

        std::map<std::string, std::vector<Robot> > robots;
        for (const auto &r: _robots) {
            auto p = r.get_position();
            auto key = std::to_string(p.x) + ":" + std::to_string(p.y);
            if (robots.find(key) == robots.end()) {
                robots[key] = std::vector<Robot>();
            }
            robots[key].push_back(r);
        }
        std::vector<std::vector<int> > positions;
        for (auto row = 0; row < PUZZLE_HEIGHT; row++) {
            for (auto col = 0; col < PUZZLE_WIDTH; col++) {
                auto key = std::to_string(col) + ":" + std::to_string(row);
                if (auto kvp = robots.find(key); kvp != robots.end()) {
                    puzzle[row][col] = kvp->second.size();
                }
            }
        }
        return puzzle;
    }

    friend std::ostream &operator<<(std::ostream &os, const Puzzle &obj) {
        std::map<std::string, std::vector<Robot> > robots;
        for (const auto &r: obj._robots) {
            auto p = r.get_position();
            auto key = std::to_string(p.x) + ":" + std::to_string(p.y);
            if (robots.find(key) == robots.end()) {
                robots[key] = std::vector<Robot>();
            }
            robots[key].push_back(r);
        }
        std::vector<std::vector<int> > positions;
        positions.resize(PUZZLE_HEIGHT);
        for (auto &p: positions) {
            p = std::vector<int>(PUZZLE_WIDTH);
        }
        for (auto row = 0; row < PUZZLE_HEIGHT; row++) {
            for (auto col = 0; col < PUZZLE_WIDTH; col++) {
                auto key = std::to_string(col) + ":" + std::to_string(row);
                if (auto kvp = robots.find(key); kvp != robots.end()) {
                    os << kvp->second.size();
                } else {
                    os << '.';
                }
            }
            os << '\n';
        }
        return os << std::flush;
    }

public:
    explicit Puzzle(const int width, const int height): _width(width), _height(height) {
    }

    void add_robot(Robot robot) {
        _robots.push_back(robot);
    }

    void update(const float &dt) {
        for (auto &robot: _robots) {
            robot.update(dt);
            auto [x, y] = robot.get_position();
            const auto off_x = x >= _width || x < 0;
            const auto off_y = y >= _height || y < 0;
            if (off_x || off_y) {
                Vector2 new_position{x, y};
                if (off_x) {
                    if (x >= _width) {
                        new_position.x = x - _width;
                    } else {
                        new_position.x = x + _width;
                    }
                }
                if (off_y) {
                    if (y >= _height) {
                        new_position.y = y - _height;
                    } else {
                        new_position.y = y + _height;
                    }
                }
                robot.teleport_to(new_position);
            }
        }
    }

    std::vector<int> get_robots_in_quadrants() {
        std::vector<int> robots_in_quadrants = {0, 0, 0, 0};
        for (const auto &robot: _robots) {
            auto [x, y] = robot.get_position();
            if (x == PUZZLE_MID_WIDTH || y == PUZZLE_MID_HEIGHT) {
                continue;
            }

            if (x < PUZZLE_MID_WIDTH && y < PUZZLE_MID_HEIGHT) {
                robots_in_quadrants[0]++;
            }

            if (x > PUZZLE_MID_WIDTH && y < PUZZLE_MID_HEIGHT) {
                robots_in_quadrants[1]++;
            }

            if (x < PUZZLE_MID_WIDTH && y > PUZZLE_MID_HEIGHT) {
                robots_in_quadrants[2]++;
            }

            if (x > PUZZLE_MID_WIDTH && y > PUZZLE_MID_HEIGHT) {
                robots_in_quadrants[3]++;
            }
        }
        return robots_in_quadrants;
    }

private:
    int _width;
    int _height;
    std::vector<Robot> _robots;
};


unsigned long day_14_1(Puzzle puzzle);

unsigned long day_14_2(Puzzle puzzle);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
        lines.push_back(line);
    }

    Puzzle puzzle(PUZZLE_WIDTH,PUZZLE_HEIGHT);

    for (const auto &line: lines) {
        puzzle.add_robot(Robot(line));
    }

    //puzzle.add_robot(Robot({0,0},{0,-1}));
    std::cout << CLEAR_SCREEN;
    std::cout << puzzle;
    const auto result_1 = day_14_1(puzzle);
    const auto result_2 = day_14_2(puzzle);
    std::cout << "Result 1: " << result_1 << "\n";
    std::cout << "Result 2: " << result_2 << "\n";
}

unsigned long day_14_1(Puzzle puzzle) {
    for (auto i = 0; i < 100; ++i) {
        puzzle.update(1);
    }
    auto quadrants = puzzle.get_robots_in_quadrants();
    auto factor = quadrants[0];
    for (int i = 1; i < quadrants.size(); ++i) {
        factor *= quadrants[i];
    }
    return factor;
}

unsigned long day_14_2(Puzzle puzzle) {
    sf::Image image;
    auto red = sf::Color(0x9E, 0x18, 0x06);
    auto green = sf::Color(0x30, 0x44, 0x34);
    auto orange = sf::Color(0xD7, 0x86, 0x28);
    auto orange_red = sf::Color(0x9F, 0x42, 0x05);
    image.create(PUZZLE_WIDTH,PUZZLE_HEIGHT);
    unsigned long factor = 0;
    for (auto i = 0; i < 30000; ++i) {
        std::string file_name = "./output_" + std::to_string(i + 1) + ".png";
        puzzle.update(1);

        auto quadrants = puzzle.get_robots_in_quadrants();
        factor = quadrants[0];
        for (int i = 1; i < quadrants.size(); ++i) {
            factor *= quadrants[i];
        }
        if (factor < 50000000)
            std::cout << i << ":" << factor << "\n";
        if (factor < 50000000) {
            auto state = puzzle.get_puzzle_state();
                sf::Color* color = nullptr;
            for (int row = 0; row < PUZZLE_HEIGHT; ++row) {
                for (int col = 0; col < PUZZLE_WIDTH; ++col) {
                    const auto val = state[row][col];
                    if (val == 0) {
                        if (row%10==0) {
                            if (color == &green) {
                                color = &orange_red;
                            }else if(color==&orange_red) {
                               color = &orange;
                            }else if (color == &orange) {
                                color = &red;

                            }else  {
                               color = &green;
                            }
                        }
                       image.setPixel(col, row, *color);
                    } else {
                        image.setPixel(col, row, sf::Color(0xDB, 0xD2, 0xC7));
                    }
                }
            }
            image.saveToFile(file_name);
        }
    }
    return factor;
}

/*
unsigned long day_14_2(Puzzle puzzle) {
    for (auto i =0; i < 10000; ++i) {
        std::string file_name="./output_"+std::to_string(i+1)+".png";
        puzzle.update(1);
        std::cout<<puzzle;
    }
    auto quadrants = puzzle.get_robots_in_quadrants();
    auto factor = quadrants[0];
    for (int i=1; i < quadrants.size(); ++i) {
        factor *= quadrants[i];
    }
    return factor;
}
*/

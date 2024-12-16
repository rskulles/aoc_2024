#include <iostream>
#include<fstream>
#include <ostream>
#include<string>
#include<vector>
#define CHANGE_TO_REBUILD 1

struct Vector2 {
    int x;
    int y;
};

enum class Direction { UP, DOWN, LEFT, RIGHT };

class Puzzle {
    friend std::ostream & operator<<(std::ostream &os, const Puzzle &obj) {
        for (auto row: obj._board) {
            for (auto col: row) {
                os << col ;
            }
            os << std::endl;
        }
        return os<<std::flush;
    }

public:
    explicit Puzzle(const std::vector<std::string>& input,const bool& extend=false ) {
        auto adding_moves = false;
        for (auto row = 0; row < input.size(); ++row) {
            if (input[row].empty()) {
                adding_moves = true;
                continue;
            }
            if (adding_moves) {
                for (const auto &c: input[row]) {
                    switch (c) {
                        case '<':
                            _moves.push_back(Direction::LEFT);
                            break;
                        case '^':
                            _moves.push_back(Direction::UP);
                            break;
                        case '>':
                            _moves.push_back(Direction::RIGHT);
                            break;
                        case 'v':
                            _moves.push_back(Direction::DOWN);
                            break;
                        default:
                            break;
                    }
                }
            } else {
                std::vector<char> puzzle_row;
                for (auto col = 0; col < input[row].size(); ++col) {
                    if (!extend) {
                        puzzle_row.push_back(input[row][col]);
                        if (puzzle_row[col] == '@') {
                            _robot_location = Vector2{col, row};
                        }
                    }else {
                        const auto c = input[row][col];

                        if (c=='#') {
                            puzzle_row.push_back('#');
                            puzzle_row.push_back('#');
                        }

                        if (c=='O') {
                          puzzle_row.push_back('[');
                          puzzle_row.push_back(']');
                        }
                        if (c=='.') {
                            puzzle_row.push_back('.');
                            puzzle_row.push_back('.');
                        }

                        if (c=='@') {

                            puzzle_row.push_back('@');
                            puzzle_row.push_back('.');
                            _robot_location = Vector2{static_cast<int>(puzzle_row.size())-2, row};
                        }
                    }

                }
                _board.push_back(puzzle_row);
            }
        }
    }

    void run() {
        for (auto &move: _moves) {
            std::cout<<*this;
            auto [x, y] = _robot_location;
            auto check_direction= Vector2{0, 0};
            switch (move) {
                case Direction::UP:
                    y--;
                check_direction = Vector2{0, -1};
                    break;
                case Direction::DOWN:
                    y++;
                check_direction = Vector2{0, 1};
                    break;
                case Direction::LEFT:
                    x--;
                check_direction = Vector2{-1, 0};
                    break;
                case Direction::RIGHT:
                    x++;
                check_direction = Vector2{1, 0};
                    break;
                default:
                    break;
            }

            const auto check_tile = _board[y][x];
            if (check_tile== '#') {
                continue;
            }

            if (check_tile=='.') {
                _board[y][x] = '@';
                _board[_robot_location.y][_robot_location.x] = '.';
                _robot_location= Vector2{x,y};
            }

            if (check_tile=='O') {
                auto check_position=Vector2{x,y};
                while (_board[check_position.y][check_position.x]=='O') {
                  check_position=Vector2{check_position.x+check_direction.x,check_position.y+check_direction.y};
                }

                if (_board[check_position.y][check_position.x]=='#') {
                   continue;
                }

               if (_board[check_position.y][check_position.x]=='.') {

                   while (_board[check_position.y+check_direction.y][check_position.x+check_direction.x]!='@') {
                       const auto swap = _board[check_position.y][check_position.x];
                       _board[check_position.y][check_position.x]=_board[check_position.y-check_direction.y][check_position.x-check_direction.x];
                       _board[check_position.y-check_direction.y][check_position.x-check_direction.x]=swap;
                       check_position.x-=check_direction.x;
                       check_position.y-=check_direction.y;
                   }
                   _robot_location = Vector2{check_position.x+check_direction.x,check_position.y+check_direction.y};
               }
            }

            if (check_tile=='['|| check_tile==']') {
                auto check_position=Vector2{x,y};
                while (_board[check_position.y][check_position.x]=='[' || _board[check_position.y][check_position.x]==']') {
                  check_position=Vector2{check_position.x+check_direction.x,check_position.y+check_direction.y};
                }

                if (_board[check_position.y][check_position.x]=='#') {
                   continue;
                }

               if (_board[check_position.y][check_position.x]=='.') {

                   while (_board[check_position.y+check_direction.y][check_position.x+check_direction.x]!='@') {
                       const auto swap = _board[check_position.y][check_position.x];
                       _board[check_position.y][check_position.x]=_board[check_position.y-check_direction.y][check_position.x-check_direction.x];
                       _board[check_position.y-check_direction.y][check_position.x-check_direction.x]=swap;
                       check_position.x-=check_direction.x;
                       check_position.y-=check_direction.y;
                   }
                   _robot_location = Vector2{check_position.x+check_direction.x,check_position.y+check_direction.y};
               }
            }
        }
        std::cout<<*this;
    }

    unsigned long sum()const {
        unsigned long sum = 0;
        for (auto row = 0; row < _board.size(); ++row) {
            for (auto col = 0; col < _board[row].size(); ++col) {
                if (_board[row][col]=='O') {
                    sum += (100*row)+col;
                }
            }
        }
        return sum;
    }

private:
    std::vector<Direction> _moves;
    std::vector<std::vector<char> > _board;
    Vector2 _robot_location;
};

unsigned long day_15_1(Puzzle& puzzle);
unsigned long day_15_2(Puzzle& puzzle);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
        lines.push_back(line);
    }

    auto puzzle_1 = Puzzle(lines);
    auto puzzle_2 = Puzzle(lines,true);
    const auto result_1 = day_15_1(puzzle_1);
    const auto result_2 = day_15_2(puzzle_2);
    std::cout << "Result 1: " << result_1 << "\n";
    std::cout << "Result 2: " << result_2 << "\n";
}

unsigned long day_15_1(Puzzle& puzzle) {
    puzzle.run();
    unsigned long result = puzzle.sum();
    return result;
}

unsigned long day_15_2(Puzzle& puzzle) {
    puzzle.run();
    return 0;
}

#include <iostream>
#include<fstream>
#include<string>
#include<chrono>
#include<thread>
#include<vector>
#define DRAW 1
//Terminal codes that help with drawing
#define START_POS "\033[1;1H"
#define CLEAR_SCREEN "\x1B[2J\x1B[H"
#define SLEEP_TIME_MS 50

typedef std::vector<std::string> board_t;
enum class Direction { up, down, left, right, none };

struct Position {
    int x;
    int y;
};

typedef std::vector<Position> positions_t;

struct GuardState {
    Position position;
    Direction direction;
};

struct ObstacleLoopPositions {
    Position positions[4];
};

enum class OffsetType {
    numeric,
    any
};

struct Offset {
    OffsetType type;
    int value;
};

struct PositionOffset{
    Offset x;
    Offset y;
};


std::string get_obstacle_loop_positions_hash(const ObstacleLoopPositions &obstacle_loop_positions) {
    std::string hash;
    for (auto position: obstacle_loop_positions.positions) {
        hash += std::to_string(position.x) + "|" + std::to_string(position.y) + "|";
    }
    return hash;
}

bool are_obstacle_loop_positions_equal(const ObstacleLoopPositions &a, const ObstacleLoopPositions &b) {
    return get_obstacle_loop_positions_hash(a) == get_obstacle_loop_positions_hash(b);
}

void sort_obstacle_positions(ObstacleLoopPositions &obstacle_loop_positions) {
    auto sorted = false;
    while (!sorted) {
        auto swapped = false;
        for (int i = 0; i < 3; ++i) {
            auto position = obstacle_loop_positions.positions[i];
            auto j=i+1;
            auto next_position = obstacle_loop_positions.positions[j];

            if ((position.y == next_position.y && position.x > next_position.x) || position.y > next_position.y) {
                swapped = true;
                obstacle_loop_positions.positions[j].x = position.x;
                obstacle_loop_positions.positions[j].y = position.y;
                obstacle_loop_positions.positions[j - 1].x = next_position.x;
                obstacle_loop_positions.positions[j - 1].y = next_position.y;
            }
        }
        sorted = !swapped;
    }
}

bool ObstacleLoopPositionsLoopable(const ObstacleLoopPositions &obstacle_loop_positions) {
    auto bad_position_count = 0;
    for (const auto &position: obstacle_loop_positions.positions) {
        if (position.x == -1 && position.y == -1) {
            ++bad_position_count;
        }
    }
    return bad_position_count == 1;
}

int get_bad_position_index(const ObstacleLoopPositions &obstacle_loop_positions) {
    for (int i = 0; i < 4; ++i) {
        const auto pos = obstacle_loop_positions.positions[i];
        if (pos.x == -1 && pos.y == -1) {
            return i;
        }
    }
    return -1;
}

void rotate_guard(GuardState &state) {
    switch (state.direction) {
        case Direction::up:
            state.direction = Direction::right;
            break;
        case Direction::down:
            state.direction = Direction::left;
            break;
        case Direction::left:
            state.direction = Direction::up;
            break;
        case Direction::right:
            state.direction = Direction::down;
            break;
        default:
            break;
    }
}

Position next_guard_move(const GuardState &state) {
    switch (state.direction) {
        case Direction::up:
            return Position{state.position.x, state.position.y - 1};
        case Direction::down:
            return Position{state.position.x, state.position.y + 1};
        case Direction::left:
            return Position{state.position.x - 1, state.position.y};
        case Direction::right:
            return Position{state.position.x + 1, state.position.y};
        default:
            return Position{state.position.x, state.position.y};
    }
}

void move_guard(GuardState &state, Position pos) {
    state.position.x = pos.x;
    state.position.y = pos.y;
}

bool guard_on_board(const GuardState &state, const board_t &board) {
    const auto pos = state.position;
    if (pos.x < 0 || pos.y < 0) {
        return false;
    }

    if (pos.y >= board.size()) {
        return false;
    }

    if (pos.x >= board[pos.y].size()) {
        return false;
    }

    return true;
}

char guard_char(const GuardState &state) {
    switch (state.direction) {
        case Direction::up:
            return '^';
        case Direction::down:
            return 'v';
        case Direction::left:
            return '<';
        case Direction::right:
            return '>';
        default:
            return '$';
    }
}

int day_6_1(const board_t &board, const GuardState &state);

int day_6_2(const board_t &board, const GuardState &state);


void print_board(const std::vector<std::string> &board) {
    std::cout << START_POS;
    for (auto &row: board) {
        std::cout << row << "\n";
    }
    std::cout << std::flush;
}

void sleep_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    board_t board;
    int row = 0;
    GuardState state{{0, 0}, Direction::none};
    auto up = '^';
    auto down = 'v';
    auto right = '>';
    auto left = '<';

    while (std::getline(in_file, line)) {
        auto found_up = line.find(up);
        auto found_down = line.find(down);
        auto found_right = line.find(right);
        auto found_left = line.find(left);
        if (found_up != std::string::npos) {
            state.position.x = static_cast<int>(found_up);
            state.position.y = row;
            state.direction = Direction::up;
        }
        if (found_down != std::string::npos) {
            state.position.x = static_cast<int>(found_down);
            state.position.y = row;
            state.direction = Direction::up;
        }
        if (found_right != std::string::npos) {
            state.position.x = static_cast<int>(found_right);
            state.position.y = row;
            state.direction = Direction::up;
        }
        if (found_left != std::string::npos) {
            state.position.x = static_cast<int>(found_left);
            state.position.y = row;
            state.direction = Direction::up;
        }
        board.push_back(line);
        ++row;
    }

    ObstacleLoopPositions obstacle_loop_positions = {};
    obstacle_loop_positions.positions[0] = {12, 30};
    obstacle_loop_positions.positions[1] = {30, 1};
    obstacle_loop_positions.positions[2] = {20, 1};
    obstacle_loop_positions.positions[3] = {10, 1};
    std::cout << get_obstacle_loop_positions_hash(obstacle_loop_positions) << std::endl;
    sort_obstacle_positions(obstacle_loop_positions);
    std::cout << get_obstacle_loop_positions_hash(obstacle_loop_positions) << std::endl;
    std::cout << CLEAR_SCREEN << std::flush;
    const auto result_1 = day_6_1(board, state);
    const auto result_2 = day_6_2(board, state);

    std::cout << "RESULT 1: " << result_1 << std::endl;
    std::cout << "RESULT 2: " << result_2 << std::endl;
}

int day_6_1(const board_t &board, const GuardState &state) {
    board_t board_copy(board.begin(), board.end());
    GuardState state_copy{{state.position.x, state.position.y}, state.direction};
#if DRAW
    print_board(board_copy);
#endif
    while (guard_on_board(state_copy, board_copy)) {
        const auto current_spot = state_copy.position;
        const auto next_move = next_guard_move(state_copy);
        GuardState test_state = {{next_move.x, next_move.y}};

        if (guard_on_board(test_state, board) && board[next_move.y][next_move.x] == '#') {
            rotate_guard(state_copy);
        } else {
            board_copy[current_spot.y][current_spot.x] = 'X';
            move_guard(state_copy, next_move);
            if (guard_on_board(state_copy, board_copy)) {
                board_copy[next_move.y][next_move.x] = guard_char(state_copy);
            }
        }
#if DRAW
        print_board(board_copy);
        sleep_ms(SLEEP_TIME_MS);
#endif
    }
    auto sum = 0;
    for (auto &row: board_copy) {
        for (auto &col: row) {
            if (col == 'X') {
                ++sum;
            }
        }
    }
    return sum;
}

int day_6_2(const board_t &board, const GuardState &state) {
    auto sum = 0;
    positions_t obstacle_positions;
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            if (board[i][j] == '#') {
                obstacle_positions.push_back({j, i});
            }

        }
    }

    return sum;
}

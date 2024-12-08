#include <iostream>
#include<fstream>
#include<string>
#include<chrono>
#include<thread>
#include<vector>
//#define DRAW_1 1
#define DRAW_2 1
//Terminal codes that help with drawing
#define START_POS "\033[1;1H"
#define CLEAR_SCREEN "\033[2J\033[H"
#define SLEEP_TIME_MS 50

typedef std::vector<std::string> board_t;

enum class Direction { up, down, left, right, none };

struct Position {
    int x;
    int y;
};

typedef std::vector<std::vector<Position> > positions_t;

struct VisitedObstacle {
    Position position;
    Direction direction;
};

bool was_visited(const VisitedObstacle new_obstacle, const std::vector<VisitedObstacle> &obstacles) {
    for (const auto&[position, direction] : obstacles) {
        if (new_obstacle.position.x == position.x &&
            new_obstacle.position.y == position.y &&
            new_obstacle.direction == direction) {
            return true;
        }
    }
    return false;
}
struct GuardState {
    Position position;
    Direction direction;
    char tile_on = '\0';
    std::vector<VisitedObstacle> visited_obstacles;
};


enum class LoopDirections {
    RightUp,
    RightDown,
};

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

char guard_trail_char(const GuardState &state) {
    char c = '\0';
    if (state.tile_on == '+') {
        return '+';
    }
    switch (state.direction) {
        case Direction::up:
        case Direction::down:
            c = '|';
            break;
        case Direction::left:
        case Direction::right:
            c = '-';
            break;
        case Direction::none:
            break;
    }
    return c;
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

    const auto up = '^';
    const auto down = 'v';
    const auto right = '>';
    const auto left = '<';

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

    std::cout << CLEAR_SCREEN << std::flush;
    //    const auto result_1 = day_6_1(board, state);
    const auto result_2 = day_6_2(board, state);

    //    std::cout << "RESULT 1: " << result_1 << std::endl;
    std::cout << "RESULT 2: " << result_2 << std::endl;
    return 0;
}

int day_6_1(const board_t &board, const GuardState &state) {
    board_t board_copy(board.begin(), board.end());
    GuardState state_copy{{state.position.x, state.position.y}, state.direction};
#if DRAW_1
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
#if DRAW_1
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

bool escape_board(GuardState &state, board_t &board) {

#if DRAW_2
    print_board(board);
#endif

    while (guard_on_board(state, board)) {
        const auto current_spot = state.position;
        const auto next_move = next_guard_move(state);
        Position next_position = {next_move.x, next_move.y};
        GuardState test_state = { next_position};

        if (guard_on_board(test_state, board) && board[next_move.y][next_move.x] == '#') {
            VisitedObstacle visited = {{next_position.x, next_position.y}, state.direction};
            if (state.visited_obstacles.empty() ||
                !was_visited(visited,state.visited_obstacles)) {
                state.visited_obstacles.push_back(visited);
            }else {
                return false;
            }
            rotate_guard(state);
           state.tile_on='+';
            board[state.position.y][state.position.x] = guard_char(state);
        } else {
            auto trail_char = guard_trail_char(state);
            if (state.tile_on =='\0' || state.tile_on=='.' || state.tile_on==trail_char) {
                board[current_spot.y][current_spot.x] = trail_char;
            }else {
                board[current_spot.y][current_spot.x] = '+';
            }
            move_guard(state, next_move);
            if (guard_on_board(state, board)) {
                state.tile_on = board[next_move.y][next_move.x];
                board[next_move.y][next_move.x] = guard_char(state);
            }
        }
#if DRAW_2
        print_board(board);
        sleep_ms(SLEEP_TIME_MS);
#endif
    }
    return true;
}

int day_6_2(const board_t &board, const GuardState &state) {
    board_t board_copy(board.begin(), board.end());
    GuardState state_copy{{state.position.x, state.position.y}, state.direction};
    if (auto escaped = escape_board(state_copy, board_copy); !escaped) {
        return 0;
    }
    auto sum =0;

    for (int row =0;row<board.size();++row) {
        for (int col=0;col<board[row].size();++col) {
            if (board_copy[row][col] != '#' &&
                board_copy[row][col] !='.' &&
                !(state.position.x == col && state.position.y == row)) {
                auto new_board = board_t(board.begin(), board.end());
                new_board[row][col] = '#';
                auto new_state = GuardState{{state.position.x, state.position.y}, state.direction};
                if (!escape_board(new_state, new_board)) {
                   ++sum;
                }
            }
        }
    }

    return sum;
}

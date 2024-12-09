#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>

#define CHANGE_TO_REBUILD 1

struct Position {
    int x;
    int y;
};

typedef Position Direction;
typedef std::vector<std::string> board_t;
typedef std::map<char, std::vector<Position> > antenna_positions_t;

unsigned long day_8_1(const board_t &board, const antenna_positions_t& antenna_positions);
unsigned long day_8_2(const board_t &board, const antenna_positions_t& antenna_positions);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    board_t board;
    antenna_positions_t antenna_positions;

    while (std::getline(in_file, line)) {
        board.push_back(line);
    }
    for (int row = 0; row < board.size(); ++row) {
        for (int col = 0; col < board[row].size(); ++col) {
            if (const auto c = board[row][col]; c != '.') {
                if (auto it = antenna_positions.find(c); it == antenna_positions.end()) {
                    antenna_positions[c] = {};
                }
                antenna_positions[c].push_back({col, row});
            }
        }
    }
    const auto result_1 = day_8_1(board, antenna_positions);
    std::cout << "RESULT 1: " << result_1 << std::endl;

    const auto result_2 = day_8_2(board, antenna_positions);
    std::cout << "RESULT 2: " << result_2 << std::endl;
}

bool position_on_board(const Position& pos, const board_t &board) {
    if (pos.x < 0 || pos.y < 0) {
        return false;
    }

    if (pos.x>=board[pos.y].size()) {
        return false;
    }
    return true;
}
bool is_anti_node_spot(const Position& pos1, const Position& pos2, const Position& test_position) {

    /*
     * I do not usually write comments like this, but I figured this out on my own. So, I am writing it down.
     * A point that is an anti-node for part I satisfies the equations:
     *
     *     pos1.x - test_position.x
     * 0 = ________________________ - 2
     *     pos2.x - test_position.x
     *          ~~and~~
     *     pos1.y - test_position.y
     * 0 = ________________________ - 2
     *     pos2.y - test_position.y
     *
     *     Noticed that it was easy to determine if a spot on the board was an anti-node to 2 antenna positions.
     *     I believe it might be doable to get the actual anti-nodes of a pair of antennas through distance of the pair,
     *     and drop checking every board position.
     *
     */
    if (test_position.x==pos2.x ||
        test_position.y == pos2.y) {
        return false;
    }
    const auto test_1 = static_cast<double>(pos1.x-test_position.x)/(pos2.x-test_position.x)-2 ==0;
    const auto test_2 = static_cast<double>(pos1.y-test_position.y)/(pos2.y-test_position.y)-2 ==0;
    return  test_1 && test_2;
}

void add_position(const int x, const int y, std::vector<Position> &positions) {
    for (const auto& pos: positions) {
        if (pos.x == x && pos.y == y) {
            return;
        }
    }
    positions.push_back({x, y});
}

// These methods should probably be written to go over the board once.
unsigned long day_8_1(const board_t &board, const antenna_positions_t& antenna_positions) {
    std::vector<Position> anti_node_positions;
    for (const auto&[c, positions]: antenna_positions) {
        for (int i = 0; i < positions.size(); ++i) {
           for (int j = i + 1; j < positions.size(); ++j) {
               auto pos1 = positions[i];
               auto pos2 = positions[j];
               for (auto row =0; row < board.size(); ++row) {
                  for (auto col=0;col<board[row].size();++col) {
                      if (is_anti_node_spot(pos1, pos2, {col,  row}) ||
                          is_anti_node_spot(pos2, pos1, {col,  row})) {
                          add_position(col,row,anti_node_positions);
                      }
                  }
              }
           }
        }
    }

    return anti_node_positions.size();
}

bool position_equal(const Position& pos1, const Position& pos2) {
   return pos1.x==pos2.x && pos1.y==pos2.y;
}
bool is_simple_anti_node_spot(const Position& antenna_position_a, const Position& antenna_position_b, const Position& test_position) {
    /*
     * Again, figured this out on my own. Just needed to google the formula. Who says trigonometry is useless?
     */

   if (position_equal(antenna_position_a, test_position) ||
       position_equal(antenna_position_b, test_position)) {
       return true;
   }
    const auto dir_a = std::atan(static_cast<double>(test_position.y-antenna_position_a.y)/static_cast<double>(test_position.x-antenna_position_a.x));
    const auto dir_b = std::atan(static_cast<double>(test_position.y-antenna_position_b.y)/static_cast<double>(test_position.x-antenna_position_b.x));
    return dir_a == dir_b;
}

unsigned long day_8_2(const board_t& board, const antenna_positions_t& antenna_positions) {
    std::vector<Position> anti_node_positions;
    for (const auto&[c, positions]: antenna_positions) {
        for (int i = 0; i < positions.size(); ++i) {
           for (int j = i + 1; j < positions.size(); ++j) {
               auto pos1 = positions[i];
               auto pos2 = positions[j];
              for (auto row =0; row < board.size(); ++row) {
                  for (auto col=0;col<board[row].size();++col) {
                      if (is_simple_anti_node_spot(pos1, pos2, {col, row}) ) {
                          add_position(col,row,anti_node_positions);
                      }
                  }
              }
           }
        }
    }
    return anti_node_positions.size();
}

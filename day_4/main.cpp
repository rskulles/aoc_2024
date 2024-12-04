#include <iostream>
#include<vector>
#include<string>

#define CAUTO const auto

typedef std::string puzzle_line_t;
typedef std::vector<puzzle_line_t> puzzle_t;

std::vector<std::string> search_words_1 = {"XMAS"};

enum class SearchDirection {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    LEFT_UP = 4,
    LEFT_DOWN = 5,
    RIGHT_UP = 6,
    RIGHT_DOWN = 7
};

int day_4_1(const puzzle_t &puzzle);

int day_4_2(const puzzle_t &puzzle);

int main(const int argc, const char *argv[]) {
    if (argc != 1) {
        std::cerr << "Too many arguments\n";
        return 1;
    }

    puzzle_t puzzle;
    puzzle_line_t line;
    while (std::getline(std::cin, line)) {
        puzzle.emplace_back(line);
    }
    for (int i = 1; i < argc; i++) {
        puzzle.emplace_back(line.substr(0, line.length() - 1));
    }

    std::cout << "PUZZLE IS: " << puzzle[0].length() << " X " << puzzle.size() << std::endl;

    CAUTO word_count_1 = day_4_1(puzzle);
    std::cout << "Results for XMAS: " << word_count_1 << std::endl;

    CAUTO word_count_2 = day_4_2(puzzle);
    std::cout << "Results for X-MAS: " << word_count_2 << std::endl;
}

bool word_matches(const puzzle_t &puzzle, const std::string &word, const int &x, const int &y,
                  SearchDirection direction) {
    std::string string_to_test;
    auto y_dir = 0;
    auto x_dir = 0;
    auto dir = "";
    switch (direction) {
        case SearchDirection::UP:
            y_dir = -1;
            dir = "UP";
            break;
        case SearchDirection::DOWN:
            y_dir = 1;
            dir = "DOWN";
            break;
        case SearchDirection::LEFT:
            x_dir = -1;
            dir = "LEFT";
            break;
        case SearchDirection::RIGHT:
            x_dir = 1;
            dir = "RIGHT";
            break;
        case SearchDirection::LEFT_UP:
            x_dir = -1;
            y_dir = -1;
            dir = "LEFT_UP";
            break;
        case SearchDirection::LEFT_DOWN:
            x_dir = -1;
            y_dir = 1;
            dir = "LEFT_DOWN";
            break;
        case SearchDirection::RIGHT_UP:
            x_dir = 1;
            y_dir = -1;
            dir = "RIGHT_UP";
            break;
        case SearchDirection::RIGHT_DOWN:
            x_dir = 1;
            y_dir = 1;
            dir = "RIGHT_DOWN";
            break;
    }


    for (int i = 0; i < word.length(); i++) {
        string_to_test += puzzle[y + (i * y_dir)][x + (i * x_dir)];
    }
    std::cout << "CHECKING [" << x << "][" << y << "](" << dir << "): " << string_to_test << "\n";
    return string_to_test == word;
}

int count_word_occurrences(const puzzle_t &puzzle, const std::string &word) {
    int occurrences = 0;
    CAUTO line_size = static_cast<int>(puzzle[0].size());
    CAUTO word_size = static_cast<int>(word.size());
    CAUTO puzzle_size = static_cast<int>(puzzle.size());
    CAUTO start_character = word[0];

    for (int y = 0; y < puzzle_size; ++y) {
        CAUTO can_check_up = y - (word_size - 1) >= 0;
        CAUTO can_check_down = y + word_size <= puzzle_size;
        CAUTO &line = puzzle[y];

        for (int x = 0; x < line.size(); ++x) {
            CAUTO can_check_left = x - (word_size - 1) >= 0;
            CAUTO can_check_right = x + word_size <= line_size;
            CAUTO can_check_left_up = can_check_left && can_check_up;
            CAUTO can_check_right_up = can_check_right && can_check_up;
            CAUTO can_check_left_down = can_check_left && can_check_down;
            CAUTO can_check_right_down = can_check_right && can_check_down;

            if (puzzle[y][x] == start_character) {
                if (can_check_up &&
                    word_matches(puzzle, word, x, y, SearchDirection::UP)) {
                    ++occurrences;
                }

                if (can_check_down &&
                    word_matches(puzzle, word, x, y, SearchDirection::DOWN)) {
                    ++occurrences;
                }

                if (can_check_left &&
                    word_matches(puzzle, word, x, y, SearchDirection::LEFT)) {
                    ++occurrences;
                }
                if (can_check_right &&
                    word_matches(puzzle, word, x, y, SearchDirection::RIGHT)) {
                    ++occurrences;
                }
                if (can_check_left_up &&
                    word_matches(puzzle, word, x, y, SearchDirection::LEFT_UP)) {
                    ++occurrences;
                }
                if (can_check_right_up &&
                    word_matches(puzzle, word, x, y, SearchDirection::RIGHT_UP)) {
                    ++occurrences;
                }
                if (can_check_left_down &&
                    word_matches(puzzle, word, x, y, SearchDirection::LEFT_DOWN)) {
                    ++occurrences;
                }
                if (can_check_right_down &&
                    word_matches(puzzle, word, x, y, SearchDirection::RIGHT_DOWN)) {
                    ++occurrences;
                }
            }
        }
    }
    return occurrences;
}

int day_4_1(const puzzle_t &puzzle) {
    auto result = 0;
    for (const auto &word: search_words_1) {
        result += count_word_occurrences(puzzle, word);
    }
    return result;
}


int count_cross_mas_occurrences(const puzzle_t &puzzle) {
    int occurrences = 0;
    CAUTO puzzle_size = static_cast<int>(puzzle.size());
    for (int y = 0; y < puzzle_size; ++y) {
        constexpr auto word_extent = 2;
        CAUTO can_check_up = y - (word_extent - 1) >= 0;
        CAUTO can_check_down = y + word_extent <= puzzle_size;
        CAUTO &line = puzzle[y];
        CAUTO line_size = static_cast<int>(line.size());

        for (int x = 0; x < line_size; ++x) {
            CAUTO can_check_left = x - (word_extent - 1) >= 0;
            CAUTO can_check_right = x + word_extent <= line_size;
            CAUTO can_check_left_up = can_check_left && can_check_up;
            CAUTO can_check_right_up = can_check_right && can_check_up;
            CAUTO can_check_left_down = can_check_left && can_check_down;
            CAUTO can_check_right_down = can_check_right && can_check_down;
            const auto current_character = puzzle[y][x];
            if (constexpr auto check_character = 'A'; current_character == check_character &&
                                                      can_check_left_down &&
                                                      can_check_right_down &&
                                                      can_check_left_up &&
                                                      can_check_right_up) {
                std::cout << "CHECKING[" << x << "][" << y << "]" << std::endl;
                auto m_1_found = false;
                auto s_1_found = false;
                auto mas_1_found = false;

                if (puzzle[y - 1][x - 1] == 'M') {
                    m_1_found = true;
                    s_1_found = puzzle[y + 1][x + 1] == 'S';
                }

                if (!m_1_found && puzzle[y - 1][x - 1] == 'S') {
                    s_1_found = true;
                    m_1_found = puzzle[y + 1][x + 1] == 'M';
                }
                mas_1_found = m_1_found && s_1_found;
                if (!mas_1_found) {
                    continue;
                }

                auto m_2_found = false;
                auto s_2_found = false;
                auto mas_2_found = false;

                if (puzzle[y - 1][x + 1] == 'M') {
                    m_2_found = true;
                    s_2_found = puzzle[y + 1][x - 1] == 'S';
                }

                if (!m_2_found && puzzle[y - 1][x + 1] == 'S') {
                    s_2_found = true;
                    m_2_found = puzzle[y + 1][x - 1] == 'M';
                }

                mas_2_found = m_2_found && s_2_found;

                if (mas_2_found) {
                    ++occurrences;
                }
            }
        }
    }
    return occurrences;
}

int day_4_2(const puzzle_t &puzzle) {
    return count_cross_mas_occurrences(puzzle);
}

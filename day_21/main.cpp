#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#define CHANGE_TO_REBUILD 1

class Puzzle{};

unsigned long day_x_1(const Puzzle& puzzle);
unsigned long day_x_2(const Puzzle& puzzle);

int main(const int argc, const char *  argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
       lines.push_back(line);
    }
    const Puzzle puzzle;
    std::cout << "Hello, World!\n";

    const auto result_1 = day_x_1(puzzle);
    const auto result_2 = day_x_2(puzzle);
    std::cout<<"Result 1: " << result_1 << "\n";
    std::cout<<"Result 2: " << result_2 << "\n";
}

unsigned long day_x_1(const Puzzle& puzzle) {
  return 0;
}

unsigned long day_x_2(const Puzzle& puzzle) {
  return 0;
}

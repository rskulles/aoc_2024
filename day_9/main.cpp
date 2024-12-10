#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#define CHANGE_TO_REBUILD 1

int main(const int argc, const char *  argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
       lines.push_back(line);
    }
    std::cout << "Hello, World!\n";
}

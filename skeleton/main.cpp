#include <iostream>
#include<fstream>

int main(const int argc, const char *  argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    while (std::getline(in_file, line)) {
        std::cout << line << std::endl;
    }
    std::cout << "Hello, World!\n";
}

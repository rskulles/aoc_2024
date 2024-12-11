#include <stdexcept>
#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<cstdint>
#include<map>

#define CHANGE_TO_REBUILD 2


std::vector<std::string> string_split(const std::string &s) {
    std::vector<std::string> result;
    auto index = s.find(' ');
    auto offset = 0ul;
    while (index != std::string::npos) {
        result.push_back(s.substr(offset, index));
        ++index;
        offset = index;
        index = s.find(' ', offset);
    }
    result.push_back(s.substr(offset));
    return result;
}

uint64_t get_number_of_digits(const uint64_t &number) {
    auto str = std::to_string(number);
    return str.size();
}

std::vector<uint64_t> split(const uint64_t &number) {
    std::vector<uint64_t> result;
    auto str = std::to_string(number);
    auto size = str.size() / 2;
    auto val_1 = str.substr(0, size);
    auto val_2 = str.substr(size);
    result.push_back(std::stoull(val_1));
    result.push_back(std::stoull(val_2));
    return result;
}

std::vector<uint64_t> blink(const std::vector<uint64_t> &input) {
    std::vector<uint64_t> result;
    for (auto value: input) {
        if (value == 0) {
            result.push_back(1);
        } else if (get_number_of_digits(value) % 2 == 0) {
            auto split_values = split(value);
            for (const auto &sv: split_values) {
                result.push_back(sv);
            }
        } else {
            result.push_back(value * 2024);
        }
    }
    return result;
}

void add_to_map(std::map<uint64_t, uint64_t> &map, const uint64_t &number, const uint64_t &value) {
    if (map.find(number) == map.end()) {
        map[number] = value;
    } else {
        map[number] += value;
    }
}

std::map<uint64_t, uint64_t> blink(const std::map<uint64_t, uint64_t> &input) {
    std::map<uint64_t, uint64_t> result;
    for (auto value: input) {
        if (value.first == 0) {
            add_to_map(result, 1, value.second);
        } else if (get_number_of_digits(value.first) % 2 == 0) {
            auto split_values = split(value.first);
            add_to_map(result, split_values[0], value.second);
            add_to_map(result, split_values[1], value.second);
        } else {
            add_to_map(result, value.first * 2024, value.second);
        }
    }
    return result;
}

uint64_t day_11_1(const std::vector<uint64_t> &input);

uint64_t day_11_2(const std::vector<uint64_t> &input);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> values;
    while (std::getline(in_file, line)) {
        values = string_split(line);
    }
    std::vector<uint64_t> input;
    input.reserve(values.size());
    for (const auto &value: values) {
        input.push_back(std::stoull(value));
    }
    const auto result_1 = day_11_1(input);
    std::cout << "Result 1: " << result_1 << std::endl;
    const auto result_2 = day_11_2(input);
    std::cout << "Result 2: " << result_2 << std::endl;
}

uint64_t day_11_1(const std::vector<uint64_t> &input) {
    constexpr size_t blink_count = 25;
    std::vector result_stones(input.begin(), input.end());
    for (auto i = 0; i < blink_count; i++) {
        result_stones = blink(result_stones);
    }
    return result_stones.size();
}


uint64_t day_11_2(const std::vector<uint64_t> &input) {
    constexpr size_t blink_count = 75;
    std::vector<uint64_t> result_stones;
    uint64_t sum = 0;
    std::map<uint64_t, uint64_t> map;
    for (const auto &value: input) {
        add_to_map(map, value, 1);
    }

    for (int i = 0; i < blink_count; ++i) {
        map = blink(map);
    }

    for (const auto &value: map) {
        sum += value.second;
    }
    return sum;
}

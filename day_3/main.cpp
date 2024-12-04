/*
 * AoC Link: https://adventofcode.com/2024/day/3
 * To Execute ./day_3 "$(cat input.txt)"
 */
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

struct Operands {
  int a{0};
  int b{0};
};

struct ParseResult {
  bool success;
  int end_location;
  Operands operands;
};

constexpr int max_operand_length = 3;

typedef std::vector<Operands> token_list;

const std::vector<std::string> keywords_1 = {"mul"};
const std::vector<std::string> keywords_2 = {"mul", "do()", "don't()"};

int day_3_1(const std::string &input);

int day_3_2(const std::string &input);

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cerr << "Not enough arguments.\n";
    return 1;
  }

  std::cout << "======|\nInput |\n======V\n" << argv[1] << "\n\n";

  const auto result_1 = day_3_1(argv[1]);
  std::cout << "Result 1: " << result_1 << std::endl;

  const auto result_2 = day_3_2(argv[1]);
  std::cout << "Result 2: " << result_2 << std::endl;
}

bool starts_with(const std::string &target, const std::string &test) {
  return target.find(test, 0) == 0;
}

ParseResult get_mul_call_operands(const std::string &s) {
  if (s[0] != '(') {
    return {false};
  }

  auto end_paren_found = false;
  auto right_paren_index = 0;

  for (auto i = s.size() - 1; i >= 4; --i) {
    if (s[i] == ')' && std::isdigit(s[i - 1])) {
      end_paren_found = true;
      right_paren_index = i;
      break;
    }
  }

  if (!end_paren_found) {
    return {false};
  }

  std::string op_1;
  std::string op_2;

  auto current_num_count = 0;
  auto comma_index = 0;
  for (auto i = 1; i < right_paren_index; ++i) {
    auto c = s[i];
    if (std::isdigit(c)) {
      op_1 += c;
      current_num_count++;
      if (current_num_count > max_operand_length) {
        return {false};
      }
    } else if (c == ',') {
      if (current_num_count == 0) {
        return {false};
      } else {
        comma_index = i;
        break;
      }
    } else {
      return {false};
    }
  }

  current_num_count = 0;
  for (auto i = comma_index + 1; i < right_paren_index; ++i) {
    auto c = s[i];
    if (std::isdigit(c)) {
      op_2 += c;
      current_num_count++;
      if (current_num_count > max_operand_length) {
        return {false};
      }
    } else {
      return {false};
    }
  }

  return {
    true,
    right_paren_index,
    {std::strtol(op_1.c_str(),nullptr,10), std::strtol(op_2.c_str(),nullptr,10)}
  };
}

token_list parse_tokens(const std::string &s,
                        const std::vector<std::string> &keywords = keywords_1) {
  token_list list;
  auto should_do = true;

  for (auto i = 0; i < s.size(); ++i) {
    for (const std::string &keyword: keywords) {
      const auto kw_size = static_cast<int>(keyword.size());
      if (const auto c = s[i]; starts_with(keyword, std::string{c})) {
        if (i + kw_size < s.size() &&
            starts_with(keyword, s.substr(i, kw_size))) {
          if (keyword == "mul") {
            i += kw_size;
            auto sub_str = i + 9 > s.size() ? s.substr(i) : s.substr(i, 9);
            auto result = get_mul_call_operands(sub_str);
            if (result.success) {
              if (should_do) {
                list.push_back(result.operands);
              }
              i += result.end_location - 1;
              break;
            }
          } else if (keyword == "do()") {
            should_do = true;
            i += kw_size - 1;
            break;
          } else if (keyword == "don't()") {
            should_do = false;
            i += kw_size - 1;
            break;
          }
        }
      }
    }
  }

  return list;
}

int day_3_1(const std::string &input) {
  auto tokens = parse_tokens(input);
  auto sum = 0;
  for (auto t: tokens) {
    sum += t.a * t.b;
  }
  return sum;
}

int day_3_2(const std::string &input) {
  auto tokens = parse_tokens(input, keywords_2);
  auto sum = 0;
  for (auto t: tokens) {
    sum += t.a * t.b;
  }
  return sum;
}

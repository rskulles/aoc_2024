/*
 *AoC Link: https://adventofcode.com/2024/day/2
 *To Execute: cat input.txt | ./day_2
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

unsigned int day_2_1(const std::vector<std::vector<int> > &r);

unsigned int day_2_2(const std::vector<std::vector<int> > &r);

int direction(int a, int b);

bool is_safe_difference(int a, int b);

bool can_be_made_safe(const std::vector<int> &r);

std::vector<int> unsafe_reports(const std::vector<int> &r);

int main(const int argc, char **argv) {
  if (argc > 1) {
    std::cerr << "too many arguments\n";
  }

  std::vector<std::vector<int> > reports;
  auto delimiter = ' ';

  for (std::string line; std::getline(std::cin, line);) {
    if (line.size() == 0) {
      continue;
    }
    std::vector<int> report;

    auto start_pos = 0;
    auto pos = line.find(delimiter, start_pos);

    while (pos != std::string::npos) {
      auto level = line.substr(start_pos, pos - start_pos);
      report.push_back(atoi(level.c_str()));
      start_pos = pos + 1;
      pos = line.find(delimiter, start_pos);
    }
    auto level = line.substr(start_pos);

    report.push_back(atoi(level.c_str()));
    reports.push_back(report);
  }

  /*
    for (auto r : reports) {
      for (auto l : r) {
        std::cout << l << " ";
      }
      std::cout << std::endl;
    }
  */

  auto safe_result = day_2_1(reports);
  std::cout << "Safe Result: " << safe_result << std::endl;

  safe_result = day_2_2(reports);
  std::cout << "Safe Result (adj): " << safe_result << std::endl;
}

bool is_safe_difference(int a, int b) {
  auto val = std::abs(b - a);
  return val >= 1 && val <= 3;
}

int direction(int a, int b) {
  if (b > a) {
    return 1;
  }
  if (b == a) {
    return 0;
  }
  return -1;
}

unsigned int day_2_1(const std::vector<std::vector<int> > &reports) {
  unsigned int safe_count = 0;
  for (auto report: reports) {
    auto d = direction(report[0], report[1]);
    if (d == 0) {
      continue;
    }
    auto is_safe = true;
    for (auto j = 1; j < report.size(); j++) {
      auto i = j - 1;
      auto a = report[i];
      auto b = report[j];
      auto safe_direction = d == direction(a, b);
      auto safe_distance = is_safe_difference(a, b);
      is_safe = safe_direction && safe_distance;
      if (!is_safe) {
        break;
      }
    }
    if (is_safe) {
      safe_count++;
    }
  }
  return safe_count;
}

std::vector<int> unsafe_reports(const std::vector<int> &r) {
  auto d = 0;
  std::vector<int> bad_levels;

  for (auto j = 1; j < r.size(); j++) {
    auto i = j - 1;
    auto a = r[i];
    auto b = r[j];
    auto level_direction = direction(a, b);

    if (d == 0 && level_direction != 0) {
      d = level_direction;
    }
    auto safe_direction = (level_direction != 0 && d == level_direction);

    auto safe_distance = is_safe_difference(a, b);

    auto good_level = safe_direction && safe_distance;

    if (!good_level) {
      bad_levels.push_back(i);
      bad_levels.push_back(j);
      if (j + 1 < r.size()) {
        bad_levels.push_back(j + 1);
      }
    }
  }
  return bad_levels;
}

bool can_be_made_safe(const std::vector<int> &r) {
  for (auto i = 0; i < r.size(); ++i) {
    std::vector<int> new_report;
    for (auto j = 0; j < r.size(); j++) {
      if (j != i) {
        new_report.push_back(r[j]);
      }
    }
    auto bad_levels = unsafe_reports(new_report);
    if (bad_levels.empty()) {
      return true;
    }
  }

  return false;
}

unsigned int day_2_2(const std::vector<std::vector<int> > &reports) {
  unsigned int safe_count = 0;
  for (auto report: reports) {
    auto bad_levels = unsafe_reports(report);
    auto is_safe = bad_levels.empty();
    if (is_safe) {
      safe_count++;
    } else if (can_be_made_safe(report)) {
      safe_count++;
    }
  }
  return safe_count;
}

/*
 *AoC Link: https://adventofcode.com/2024/day/1
 *To execute: ./day_1 $(cat input.txt)
*/
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

unsigned int day_1_1(const std::vector<int> &l, const std::vector<int> &r);

unsigned int day_1_2(const std::vector<int> &l, const std::vector<int> &r);

int main(const int argc, char **argv) {

  if (argc == 1) {
    std::cerr << "Not enough arguments\n";
    return 1;
  }

  std::vector<int> left;
  std::vector<int> right;

  for (auto i = 1; i < argc; ++i) {
    if (i % 2 == 0) {
      right.push_back(std::strtol(argv[i], nullptr, 10));
    } else {
      left.push_back(std::strtol(argv[i], nullptr, 10));
    }
  }

  const auto dist_result = day_1_1(left, right);
  std::cout << "Distance Result: " << dist_result << std::endl;

  const auto sim_result = day_1_2(left, right);
  std::cout << "Similarity Result: " << sim_result << std::endl;
}

unsigned int day_1_1(const std::vector<int> &l, const std::vector<int> &r) {
  auto left_clone = std::vector(l);
  auto right_clone = std::vector(r);
  std::vector<int> diffs;

  std::sort(left_clone.begin(), left_clone.end());
  std::sort(right_clone.begin(), right_clone.end());

  diffs.reserve(left_clone.size());

  for (auto i = 0; i < left_clone.size(); ++i) {
    diffs.push_back(std::abs((right_clone[i] - left_clone[i])));
  }

  unsigned int sum = 0;

  for (const auto i: diffs) {
    sum += i;
  }

  return sum;
}

unsigned int day_1_2(const std::vector<int> &l, const std::vector<int> &r) {
  std::map<int, unsigned int> similarity_map;
  std::vector<unsigned int> similarity_scores;

  for (int left_id: l) {
    if (auto hashed_score = similarity_map.find(left_id); hashed_score != similarity_map.end()) {
      similarity_scores.push_back(hashed_score->second);
      continue;
    }

    auto count = 0;
    for (const auto right_id: r) {
      if (left_id == right_id) {
        count++;
      }
    }

    unsigned int score = left_id * count;
    similarity_scores.push_back(score);
    similarity_map[left_id] = score;
  }

  unsigned int sum = 0;

  for (const auto score: similarity_scores) {
    sum += score;
  }

  return sum;
}

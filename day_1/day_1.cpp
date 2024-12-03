#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

unsigned int day_1_1(const std::vector<int> &l, const std::vector<int> &r);
unsigned int day_1_2(const std::vector<int> &l, const std::vector<int> &r);

int main(int argc, char **argv) {

  if (argc == 1) {
    std::cerr << "Not enough arguments\n";
  }

  std::vector<int> left;
  std::vector<int> right;

  for (auto i = 1; i < argc; ++i) {
    if (i % 2 == 0) {
      right.push_back(atoi(argv[i]));
    } else {
      left.push_back(atoi(argv[i]));
    }
  }

  auto dist_result = day_1_1(left, right);
  std::cout << "Distance Result: " << dist_result << std::endl;
  auto sim_result = day_1_2(left, right);

  std::cout << "Similarity Result: " << sim_result << std::endl;
}

unsigned int day_1_1(const std::vector<int> &l, const std::vector<int> &r) {

  auto lclone = std::vector(l);
  auto rclone = std::vector(r);
  std::vector<int> diffs;

  std::sort(lclone.begin(), lclone.end());
  std::sort(rclone.begin(), rclone.end());

  for (auto i = 0; i < lclone.size(); ++i) {
    diffs.push_back(std::abs((rclone[i] - lclone[i])));
  }

  unsigned int sum = 0;
  for (auto i : diffs) {
    sum += i;
  }

  return sum;
}

unsigned int day_1_2(const std::vector<int> &l, const std::vector<int> &r) {

  std::map<int, unsigned int> similarity_map;
  std::vector<unsigned int> similarity_scores;

  for (auto i = 0; i < l.size(); ++i) {
    auto left_id = l[i];
    auto hashed_score = similarity_map.find(left_id);

    if (hashed_score != similarity_map.end()) {
      similarity_scores.push_back(hashed_score->second);
      continue;
    }

    auto count = 0;
    for (auto right_id : r) {
      if (left_id == right_id) {
        count++;
      }
    }

    unsigned int score = left_id * count;
    similarity_scores.push_back(score);
    similarity_map[left_id] = score;
  }

  unsigned int sum = 0;
  for (auto score : similarity_scores) {
    sum += score;
  }

  return sum;
}

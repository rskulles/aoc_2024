#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdint>
#include<cmath>
#include <map>
#include<sstream>
#define CHANGE_TO_REBUILD 133

class Puzzle: public std::vector<std::int64_t> {
public:
  explicit Puzzle(std::vector<std::int64_t> const& input): std::vector<std::int64_t>(input) {}
};

unsigned long long day_22_1(const Puzzle& puzzle);
unsigned long long day_22_2(const Puzzle& puzzle);
Puzzle parse_puzzle(const std::vector<std::string>& puzzle);

int64_t mix(const int64_t& secret, const int64_t& value){
  return secret^value;
}

int64_t prune(const int64_t& secret) {
  return secret%16777216;
}

int64_t evolve_a(const int64_t& secret) {
  auto value  = secret *64;
  value = mix(secret,value);
  return prune(value);
}

int64_t evolve_b(const int64_t &secret) {
  auto value =static_cast<int64_t>(std::floor(static_cast<double>(secret)/32.0));
  value = mix(secret,value);
  return prune(value);
}

int64_t evolve_c(const int64_t& secret) {
 auto value = secret *2048;
  value = mix(secret,value);
  return prune(value);
}

int64_t evolve(const int64_t& secret) {
  auto result = evolve_a(secret);
  result = evolve_b(result);
  return evolve_c(result);
}

int main(const int argc, const char *  argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
       lines.push_back(line);
    }
    in_file.close();

    const auto puzzle = parse_puzzle(lines);
    const auto result_1 = day_22_1(puzzle);
    std::cout<<"Result 1: " << result_1 << "\n"<<std::flush;
    const auto result_2 = day_22_2(puzzle);
    std::cout<<"Result 2: " << result_2 << "\n"<<std::flush;
}

Puzzle parse_puzzle(const std::vector<std::string>& puzzle) {
  std::vector<std::int64_t> input;
  input.reserve(puzzle.size());
  for (const auto& line : puzzle) {
    input.push_back(std::stoull(line));
  }
  return Puzzle(input);
}

unsigned long long day_22_1(const Puzzle& puzzle) {
  uint64_t result = 0;
  for (const auto& line : puzzle) {
    auto s = line;
    for (auto i=0;i<2000;++i) {
      s = evolve(s);
    }
    result += s;
  }
  return result;
}

struct EvolveResult {
  int64_t change;
  uint64_t price;
};

unsigned long long day_22_2(const Puzzle& puzzle) {
  const auto puzzle_size = puzzle.size();
  std::vector<std::vector<EvolveResult>> evolve_results;
  evolve_results.resize(puzzle_size);

  std::map<std::string,std::map<long long,uint64_t>> price_map;


  for (auto j=0;j<puzzle.size();++j) {
    auto s = puzzle[j];
    for (auto i=0;i<2000;++i) {
      const auto t = s;
      s =evolve(s);
      const int64_t diff = s%10-t%10;

      evolve_results[j].push_back(EvolveResult{diff, static_cast<uint64_t>(s)%10});
    }
  }

  for (auto j=0;j<puzzle.size();++j) {
    auto changes = evolve_results[j];
    for (auto i=3;i<2000;++i) {
      std::stringstream ss;
      ss <<changes[i-3].change<<'|'<<changes[i-2].change<<'|'<<changes[i-1].change<<'|'<< changes[i].change;
      const auto p = changes[i].price;
      const auto s = ss.str();
      if (price_map.find(s)==price_map.end()) {
        price_map[s][j]=p;
      }else {
        if (price_map[s].find(j)==price_map[s].end()) {
          price_map[s][j]=1;
          price_map[s].begin()->second+=p;
        }
      }
    }
  }
  uint64_t result =0;
  for (const auto&[fst, snd]: price_map) {
    if (snd.begin()->second>result) {
      result = snd.begin()->second;
    }
  }
  return result;
}

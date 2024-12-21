#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
#include <set>

#define CHANGE_TO_REBUILD 12

class Puzzle {
public:
    explicit Puzzle(const std::vector<std::string> &towel_patterns, const std::vector<std::string> &towels): _towels(
        towels) {
        for (const auto &towel_pattern: towel_patterns) {
            _patterns[towel_pattern[0]].push_back(towel_pattern);
        }
    };

    Puzzle() = delete;
    [[nodiscard]] unsigned long long possible_towels( const bool& should_return_good_count=true) const {
        size_t count = 0;
        std::map<std::string, size_t> bad_values{};
        std::map<std::string, size_t> good_values{};
        unsigned long long total_score=0;
        for (const auto &s: _towels) {
            const auto score = find_solution(s,get_patterns(s),bad_values, good_values);
            //std::cout<<score<<std::endl;
            total_score += score;
            if (score>0) {
                ++count;
            }
        }
        return should_return_good_count? count :total_score;
    }

private:
    [[nodiscard]]std::vector<std::string> get_patterns(const std::string& s)const {
        std::vector<std::string> patterns;
        const auto c = s[0];
        auto kvp = _patterns.find(c);
        if (kvp == _patterns.end()) {
            return patterns;
        }
        for (const auto &p: kvp->second) {
            if (p.size() <= s.size()) {
                patterns.push_back(p);
            }
        }
        return patterns;
    }

    [[nodiscard]]size_t find_solution(const std::string& towel, const std::vector<std::string>& patterns, std::map<std::string,size_t>& bad_patterns, std::map<std::string, size_t>& good_patterns) const {

        if (bad_patterns.find(towel) != bad_patterns.end()) {
           return 0;
        }

        if (good_patterns.find(towel) !=good_patterns.end()) {
            return good_patterns[towel];
        }

        if (towel.empty()) {
            return 1;
        }

        if (patterns.empty()) {
            return 0;
        }


        size_t sum=0;

        for (const auto& p:patterns) {

           if (towel.find(p)==0) {
               std::string str = towel.substr(p.size());
               const auto new_patterns = get_patterns (str);
               const auto val =find_solution(str,new_patterns,bad_patterns, good_patterns);
               if (val>0) {
                   sum += val;
                   good_patterns[str] = val;
               }else {
                   bad_patterns[str]= 1;
               }
           }
        }
       return sum;
    }

    std::map<char,std::vector<std::string>> _patterns;
    std::vector<std::string> _towels;
};

unsigned long long day_19_1(const Puzzle &puzzle);

unsigned long long day_19_2(const Puzzle &puzzle);

std::string trim_space(const std::string &s) {
    std::stringstream ss;
    for (const auto &c: s) {
        if (!std::isspace(c)) {
            ss << c;
        }
    }
    return ss.str();
}

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> patterns;
    std::vector<std::string> towels;
    bool getting_patterns = true;
    while (std::getline(in_file, line)) {
        if (line.empty()) {
            getting_patterns = false;
            continue;
        }
        size_t pos = 0;
        size_t pos2 = line.find(',', pos);
        if (getting_patterns) {
            while (pos2 != std::string::npos) {
                patterns.push_back(trim_space(line.substr(pos, pos2 - pos)));
                pos = pos2 + 1;
                pos2 = line.find(',', pos);
            }
            patterns.push_back(trim_space(line.substr(pos)));
        } else {
            towels.push_back(line);
        }
    }


    const Puzzle puzzle(patterns, towels);

    const auto result_1 = day_19_1(puzzle);
    const auto result_2 = day_19_2(puzzle);
    std::cout << "Result 1: " << result_1 << "\n";
    std::cout << "Result 2: " << result_2 << "\n";
}

unsigned long long day_19_1(const Puzzle &puzzle) {
    return puzzle.possible_towels();
}

unsigned long long day_19_2(const Puzzle &puzzle) {
    return puzzle.possible_towels(false);
}

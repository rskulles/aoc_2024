#include <iostream>
#include<fstream>
#include<string>
#include<vector>

typedef std::vector<int> update_t;
typedef std::vector<std::vector<int> > updates_t;

struct Rule {
    int number;
    int comes_before;
};

std::string rule_to_string(const Rule &rule) {
    return std::to_string(rule.number) + "|" + std::to_string(rule.comes_before);
}

void print_rules(const std::vector<Rule> &rules) {
    std::cout << "-----" << std::endl;
    std::cout << "RULES" << std::endl;
    std::cout << "-----\n\n";
    for (auto &rule: rules) {
        std::cout << rule_to_string(rule) << std::endl;
    }
}
void print_update(const update_t &update) {

    std::cout << "------" << std::endl;
    std::cout << "UPDATE" << std::endl;
    std::cout << "------\n\n";
      for (auto &item: update) {
            std::cout << item << " ";
        }
        std::cout << "\n\n";
}

int day_5_1(const updates_t &updates, const std::vector<Rule> &rules);
int day_5_2(const updates_t & updates,const std::vector<Rule> & rules) ;

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;

    std::vector<Rule> rules;
    updates_t updates;

    auto is_getting_rules = true;

    while (std::getline(in_file, line)) {
        if (is_getting_rules) {
            if (line.empty()) {
                is_getting_rules = false;
            } else {
                const auto sep_index = line.find('|');

                if (sep_index != std::string::npos) {
                    const auto left = std::strtol(line.substr(0, sep_index).c_str(), nullptr, 10);
                    const auto right = std::strtol(line.substr(sep_index + 1).c_str(), nullptr, 10);
                    rules.push_back(Rule{left, right});
                }
            }
        } else {
            if (!line.empty()) {
                update_t update;
                auto search_offset = 0;
                auto comma_index = line.find(',');

                while (comma_index != std::string::npos) {
                    auto number = std::strtol(line.substr(search_offset, comma_index - search_offset).c_str(), nullptr,
                                              10);
                    update.push_back(number);
                    search_offset = static_cast<int>(comma_index) + 1;
                    comma_index = line.find(',', comma_index + 1);
                    if (comma_index == std::string::npos) {
                        number = std::strtol(line.substr(search_offset).c_str(), nullptr, 10);
                        update.push_back(number);
                    }
                }
                updates.emplace_back(update);
            }
        }
    }

    /*

    for (auto &update: updates) {

    }
    */
    auto result_1 = day_5_1(updates, rules);
    std::cout << "RES 1:" << result_1 << std::endl;
    auto result_2 = day_5_2(updates, rules);
    std::cout << "RES 2:" << result_2 << std::endl;
}


// A number is in a rule if it is the base number
bool in_rule(const int &number, const Rule &rule) {
    return rule.number == number || rule.comes_before == number;
}

std::vector<Rule> get_rules_for_update(const update_t &update, const std::vector<Rule> &Rules) {
    std::vector<Rule> rules;
    for (const auto &item: update) {
        for (const auto &rule: Rules) {
            if (in_rule(item, rule)) {
                rules.push_back(rule);
            }
        }
    }
    return rules;
}

std::vector<Rule> get_rules_for_item(const int &item, std::vector<Rule> all_rules) {
    std::vector<Rule> rules;
    for (const auto &rule: all_rules) {
        if (in_rule(item, rule)) {
            rules.push_back(rule);
        }
    }
    return rules;
}

int day_5_1(const updates_t &updates, const std::vector<Rule> &all_rules) {
    auto sum = 0;
    auto count =0;
    for (auto &update: updates) {
        auto current_rules = get_rules_for_update(update, all_rules);
        auto update_is_valid = true;
        //print_rules(current_rules);
        for (int i = 0; update_is_valid && i < (int) update.size() - 1; ++i) {
            for (const auto &rule: get_rules_for_item(update[i], all_rules)) {
                auto future_updates = update_t(update.begin() + i + 1, update.end());
                for (auto item: future_updates) {
                    if (item == rule.number) {
                        update_is_valid = false;
                        break;
                    }
                }
            }
        }
        if (update_is_valid) {
            count ++;
            const auto middle = update[update.size() / 2];
            sum += middle;
        }
    }
    std::cout << count<<"/"<<updates.size() << std::endl;

    return sum;
}

int day_5_2(const updates_t &updates, const std::vector<Rule> &all_rules) {
    auto sum = 0;
    //auto count =0;
    updates_t updates_copy(updates.begin(), updates.end());

    for (auto & update : updates_copy) {
        auto current_rules = get_rules_for_update(update, all_rules);
        auto bad = false;
        for (int i = 0;  i < static_cast<int>(update.size()) - 1; ++i) {
            bool corrected = false;
            for (const auto &rule: get_rules_for_item(update.at(i), all_rules)) {
                for (auto j=i+1;j<update.size();++j) {
                    const auto check_number= update[j];
                    if (check_number== rule.number) {
                        corrected = true;
                        bad=true;
                        for (auto top_index =j; top_index>i;--top_index) {
                            const auto value = update[top_index];
                            update[top_index] = update[top_index - 1];
                            update[top_index - 1] = value;
                        }
                        break;
                    }
                }
                if (corrected) {break;}
            }
            
           if (corrected) {
               i--;
           }
        }

        if (bad) {
            const auto middle = update[update.size() / 2];
            sum += middle;
        }
    }
    return sum;
}

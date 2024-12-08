#include <iostream>
#include<fstream>
#include<string>
#include<vector>

struct CalibrationEquation {
    long long result = 0;
    std::vector<long long> inputs = {};
};

long long day_7_1(const std::vector<CalibrationEquation> &calibrationEquations);

long long day_7_2(const std::vector<CalibrationEquation> &calibrationEquations);

int main(const int argc, const char *argv[]) {
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<CalibrationEquation> equations;
    while (std::getline(in_file, line)) {
        CalibrationEquation equation;
        //parse result;
        auto colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            equation.result = std::strtoll(line.substr(0, colon_pos).c_str(), nullptr, 10);
            auto inputs = line.substr(colon_pos + 2);
            std::string current_input;
            for (char input: inputs) {
                if (input != ' ') {
                    current_input += input;
                } else {
                    equation.inputs.push_back(std::stoi(current_input));
                    current_input = "";
                }
            }
            equation.inputs.push_back(std::stoi(current_input));
        }
        equations.push_back(equation);
    }

    const auto result = day_7_1(equations);
    std::cout << "RESULT 1: " << result << std::endl;
    const auto result_2 = day_7_2(equations);
    std::cout << "RESULT 2: " << result_2 << std::endl;
}

long add(const long long a, const long long b) {
    return a + b;
}

long mul(const long long a, const long long b) {
    return a * b;
}

long cat(const long long a, const long long b) {
    auto str =std::to_string(a);
    str+=std::to_string(b);
    return std::strtoll(str.c_str(), nullptr, 10);
}

long long perform_equation(const std::vector<long long> &operands, const std::vector<int> &description,
                           const std::vector<std::string> &operators) {
    long long result = 0;
    for (int i = 0; i < description.size(); ++i) {
        const auto &op = operators[description[i]];
        if (op == "+") {
            if (i == 0) {
                result += add(operands[0], operands[1]);
            } else {
                result = add(result, operands[i + 1]);
            }
        } else if (op == "*") {
            if (i == 0) {
                result += mul(operands[0], operands[1]);
            } else {
                result = mul(result, operands[i + 1]);
            }
        } else if (op == "||") {

            if (i == 0) {
                result += cat(operands[0], operands[1]);
            } else {
                result = cat(result, operands[i + 1]);
            }
        } else {
            throw std::invalid_argument("Invalid operation");
        }
    }
    return result;
}

bool equation_is_solvable(const CalibrationEquation &equation, const std::vector<std::string> &operators) {
    const auto total_operands = equation.inputs.size() - 1;
    const auto total_operators = operators.size();
    const auto total_equations = static_cast<long long>(std::pow(total_operators, total_operands));
    std::vector<std::vector<int> > equation_description;
    equation_description.resize(total_equations);
    for (int i = 0; i < total_equations; ++i) {
        equation_description[i].resize(total_operands);
        if (i == 0) {
            for (int j = 0; j < total_operands; ++j) {
                equation_description[i][j] = 0;
            }
            continue;
        }

        for (int j = 0; j < total_operands; ++j) {
            equation_description[i][j] = equation_description[i - 1][j];
        }
        int j = 0;
        auto should_carry = false;
        do {
            equation_description[i][j] += 1;
            should_carry = equation_description[i][j] >= total_operators;
            if (should_carry) {
                equation_description[i][j] = 0;
            }
            ++j;
        } while (j < total_operands &&
                 should_carry);
    }

    for (int i = 0; i < total_equations; ++i) {
        auto result = perform_equation(equation.inputs, equation_description[i], operators);
        if (result == equation.result) {
            return true;
        }
    }

    return false;
}

long long day_7_1(const std::vector<CalibrationEquation> &calibrationEquations) {
    auto sum = 0ll;
    for (const auto &equation: calibrationEquations) {
        if (equation_is_solvable(equation, {"+", "*"})) {
            sum += equation.result;
        }
    }
    return sum;
}

long long day_7_2(const std::vector<CalibrationEquation> &calibrationEquations) {
    auto sum = 0ll;
    int line = 1;
    for (const auto &equation: calibrationEquations) {
        if (equation_is_solvable(equation, {"+", "*", "||"})) {
            sum += equation.result;
        }

    }
    return sum;
}

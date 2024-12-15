#include <iostream>
#include<fstream>
#include <ostream>
#include<string>
#include<vector>
#include<limits>
#define CHANGE_TO_REBUILD 1

struct Position {
    double x;
    double y;
};

typedef struct Position Vector2;

struct SolveResult {
    long button_a_presses{0};
    long button_b_presses{0};
};

class ClawMachine {
public:
    friend std::ostream &operator<<(std::ostream &os, const ClawMachine &obj) {
        return os
               << "Button A: " << obj._b_a.x << ", " << obj._b_a.y << "\n"
               << "Button B: " << obj._b_b.x << ", " << obj._b_b.y << "\n"
               << "Prize: " << obj._p_l.x << ", " << obj._p_l.y << "\n";
    }

    ClawMachine(const Vector2 &button_a, const Vector2 &button_b, const Position &prize_location): _b_a(button_a),
        _b_b(button_b), _p_l(prize_location) {
    }

    /* The equations to solve one of these claw machines is:
     * Button_A_X_Offset * A_Presses + Button_B_X_Offset * B_Presses = X_Offset
     * Button_A_Y_Offset * A_Presses + Button_B_Y_Offset * B_Presses = Y_Offset
     *
     * This solution solves for 'A_Presses' then plugs 'A_Presses' into equation 1 to solve for 'B_Presses'
     */
    SolveResult solve()const {
        SolveResult result;
        const double a = static_cast<double>(_b_b.x*_p_l.y-_p_l.x*_b_b.y)/static_cast<double>(_b_b.x*_b_a.y-_b_a.x*_b_b.y);
        const double b = static_cast<double>(_p_l.x-_b_a.x*a)/static_cast<double>(_b_b.x);
        result.button_a_presses = is_integer(a)? static_cast<long>(a): -1;
        result.button_b_presses = is_integer(b)? static_cast<long>(b): -1;
        return result;
    }
    void correct_position(const double& by) {
        _p_l.x += by;
        _p_l.y += by;
    }
private:
    static bool is_integer(const double& d){
        return std::floor(d) == d;
    }
    ClawMachine() = default;
    const Vector2 _b_a{0, 0};
    const Vector2 _b_b{0, 0};
    Position _p_l{0, 0};
};

class Puzzle : public std::vector<ClawMachine> {
public:
    Puzzle() = default;
    explicit Puzzle(const std::vector<ClawMachine>& other): std::vector<ClawMachine>(other) {}
};

long day_13_1(const Puzzle &puzzle);
long day_13_2(const Puzzle &puzzle);

int main(const int argc, const char *argv[]) {
    std::cout<<"Max Double"<<std::numeric_limits<double>::max()<<std::endl;
    std::ifstream in_file("input.txt");
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in_file, line)) {
        lines.push_back(line);
    }
    Vector2 button_a;
    Vector2 button_b;
    Position prize_location;

    Puzzle puzzle;
    const std::string &x_search = "X+";
    const std::string &y_search = "Y+";
    const std::string &x_search_alt = "X=";
    const std::string &y_search_alt = "Y=";
    for (const auto &line: lines) {
        if (const auto a_line = line.find("Button A:"); a_line != std::string::npos) {
            const auto x_pos = line.find(x_search, a_line) + x_search.size();
            const auto end_x_pos = line.find(",", x_pos);
            const auto x_string = line.substr(x_pos, end_x_pos - x_pos);
            const auto y_pos = line.find(y_search, end_x_pos) + y_search.size();
            const auto y_string = line.substr(y_pos);
            button_a.x = std::stoi(x_string);
            button_a.y = std::stoi(y_string);
        } else if (const auto b_line = line.find("Button B:"); b_line != std::string::npos) {
            const auto x_pos = line.find(x_search, b_line) + x_search.size();
            const auto end_x_pos = line.find(",", x_pos);
            const auto x_string = line.substr(x_pos, end_x_pos - x_pos);
            const auto y_pos = line.find(y_search, end_x_pos) + y_search.size();
            const auto y_string = line.substr(y_pos);
            button_b.x = std::stoi(x_string);
            button_b.y = std::stoi(y_string);
        } else if (const auto prize_line = line.find("Prize:"); prize_line != std::string::npos) {
            const auto x_pos = line.find(x_search_alt, prize_line) + x_search_alt.size();
            const auto end_x_pos = line.find(",", x_pos);
            const auto x_string = line.substr(x_pos, end_x_pos - x_pos);
            const auto y_pos = line.find(y_search_alt, end_x_pos) + y_search_alt.size();
            const auto y_string = line.substr(y_pos);
            prize_location.x = std::stoi(x_string);
            prize_location.y = std::stoi(y_string);
            puzzle.push_back(ClawMachine(button_a, button_b, prize_location));
        }
    }
    const long result_1 = day_13_1(puzzle);
    std::cout << "RESULT 1: "<<result_1 << std::endl;
    const long result_2 = day_13_2(puzzle);
    std::cout << "RESULT 2: "<<result_2 << std::endl;
}

long day_13_1(const Puzzle &puzzle) {
    long result = 0;
    for (auto &p: puzzle) {
        if (auto [button_a_presses, button_b_presses]= p.solve(); button_a_presses >= 0 && button_b_presses >= 0) {
            result += (3*button_a_presses) + button_b_presses;
        }
    }
    return result;
}

long day_13_2(const Puzzle &puzzle) {
    Puzzle corrected_puzzle(std::vector<ClawMachine>(puzzle.begin(), puzzle.end()));

    for (auto &p: corrected_puzzle) {
        p.correct_position(10000000000000);
    }

    long result = 0;
    for (auto &p: corrected_puzzle) {
        if (auto [button_a_presses, button_b_presses]= p.solve(); button_a_presses >= 0 && button_b_presses >= 0) {
            result += (3*button_a_presses) + button_b_presses;
        }
    }
    return result;
}

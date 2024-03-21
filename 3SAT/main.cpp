#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <cmath>
#include <array>

struct Literal {
public:
    char variable{};
    bool negated{false};
};

class Clause {
public:
    std::array<Literal, 3> literals;

    explicit Clause(std::array<Literal, 3> literals) : literals{literals} {}

    [[nodiscard]] bool simulate_clause(const std::map<char, bool> &test_case) const {
        for (const Literal &l : literals) {
            bool r = test_case.at(l.variable);
            if (l.negated)
                r = !r;
            if (r)
                return true;
        }
        return false;
    }
};

class Formula {
private:
    std::vector<Clause> clauses{};
    std::set<char> variables{};
public:
    Formula() = default;

private:
    std::vector<std::map<char, bool>> populate_permutation(int c) {
        std::vector<std::map<char, bool>> permutations{};
        for (int i = 0; i < c; ++i) {
            std::map<char, bool> chars{};
            for (auto v : this->variables) {
                chars[v] = false;
            }
            permutations.push_back(chars);
        }
        return permutations;
    }

    std::vector<std::map<char, bool>> generateVariablePermutations() {
        int n = static_cast<int>(this->variables.size());
        int c = pow(2, n);
        int x = c/2;
        int y = 0;
        bool set{true};

        auto permutations{populate_permutation(c)};

        for (char i : variables) {
            for(int j = 0; j < c; ++j) {
                permutations[j].at(i) = set;
                ++y;
                if (y == x) {
                    set = !set;
                    y = 0;
                }
            }
            x /= 2;
        }

        return permutations;
    }

public:
    void solve() {
        std::vector<std::map<char, bool>> permutations = generateVariablePermutations();
        for (const std::map<char, bool> &p : permutations) {
            std::vector<bool> result;
            result.reserve(clauses.size());
            for (const Clause &c : this->clauses) {
                result.push_back(c.simulate_clause(p));
            }
            bool accept{true};
            for (bool r : result) {
                if (!r) {
                    accept = false;
                    break;
                }
            }
            if (accept) {
                std::cout << "yes" << std::endl;
                return;
            }
        }
        std::cout << "no" << std::endl;
    }

    void read_input() {
        std::string n_in;
        std::getline(std::cin, n_in);
        int n{stoi(n_in)};
        std::vector<Clause> input_clauses{};
        input_clauses.reserve(n);
        for (int i = 0; i < n; ++i) {
            std::string clause_line;
            std::getline(std::cin, clause_line);

            std::array<Literal, 3> literals;
            int index{0};
            bool negated{false};
            for (char c: clause_line) {
                if (c == ' ') continue;

                if (c == '!') {
                    negated = true;
                } else {
                    this->variables.insert(c);
                    literals[index] = Literal{c, negated};
                    negated = false;
                    index++;
                }
            }
            input_clauses.emplace_back(literals);
        }
        this->clauses = input_clauses;
    }
};

int main() {
    Formula formula{};
    formula.read_input();
    formula.solve();
    return 0;
}

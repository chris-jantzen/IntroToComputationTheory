#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <cmath>

struct Literal {
public:
    char variable{};
    bool negated{false};
};

class Clause {
public:
    std::vector<Literal> literals{};

    explicit Clause(std::vector<Literal> literals) : literals{std::move(literals)} {}

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

    static void print_permutations(const std::vector<std::map<char, bool>> &permutations) {
        for (const std::map<char, bool> &x : permutations) {
            for (std::pair<char, bool> y : x) {
                std::cout << y.first << ": " << y.second << " ";
            }
            std::cout << '\n';
        }
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
        auto permutations = generateVariablePermutations();
        for (const auto &p : permutations) {
            std::vector<bool> result{};
            for (const Clause &c : this->clauses) {
                result.push_back(c.simulate_clause(p));
            }
            bool accept{true};
            for (auto r : result) {
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
        for (int i = 0; i < n; ++i) {
            std::string clause_line;
            std::getline(std::cin, clause_line);

            std::vector<Literal> literals{};
            bool negated{false};
            for (char c: clause_line) {
                if (c == ' ') continue;

                if (c == '!') {
                    negated = true;
                } else {
                    this->variables.insert(c);
                    literals.push_back(Literal{c, negated});
                    negated = false;
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

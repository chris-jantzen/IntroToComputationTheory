#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <cmath>
#include <array>

struct PermutationTracker {
public:
    int frequency{};
    int counter{};
    bool set{};
};

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

public:
    void solve() {
        int number_of_variables = static_cast<int>(this->variables.size());
        int number_of_permutations = static_cast<int>(pow(2, number_of_variables));
        int x = number_of_permutations / 2;

        // initialize permutation map
        std::map<char, bool> permutation_map{};
        for (char v : this->variables) {
            permutation_map[v] = false;
        }

        std::map<char, PermutationTracker> permutation_iteration_tracker{};
        for (char v : this->variables) {
            permutation_iteration_tracker[v] = PermutationTracker{x, 0, true};
            x /= 2;
        }

        for (int i = 0; i < number_of_permutations; ++i) {
            for (char v : this->variables) {
                PermutationTracker &current_permutation_state = permutation_iteration_tracker.at(v);
                if (current_permutation_state.counter == current_permutation_state.frequency) {
                    current_permutation_state.set = !current_permutation_state.set;
                    current_permutation_state.counter = 0;
                }
                ++current_permutation_state.counter;
                permutation_map.at(v) = current_permutation_state.set;
            }

            for (const auto &p : permutation_map) {
                std::cout << p.first << ": " << p.second << ", ";
            }
            std::cout << std::endl;

            // Perform check on all clauses with current permutation_map iteration
            std::vector<bool> result;
            result.reserve(clauses.size());
            for (const Clause &clause : this->clauses) {
                result.push_back(clause.simulate_clause(permutation_map));
            }
            bool accept{true};
            for (bool r : result) {
                if (!r) {
                    accept = false;
                    break;
                }
            }

            // check if all are true and immediately halt and output
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

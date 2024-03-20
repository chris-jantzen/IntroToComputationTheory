#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>

class Literal {
public:
    char variable{};
    bool negated{false};

    Literal(char variable, bool negated) : variable{variable}, negated{negated} {}
};

class Clause {
public:
    std::vector<Literal> literals{};

    explicit Clause(std::vector<Literal> literals) : literals{std::move(literals)} {}
};

class Formula {
public:
    std::vector<Clause> clauses{};

    explicit Formula(std::vector<Clause> clauses) : clauses{std::move(clauses)} {}
};

// Read in input
Formula read_input() {
    std::string n_in;
    std::getline(std::cin, n_in);
    int n {stoi(n_in)};
    std::vector<Clause> clauses{};
    for (int i = 0; i < n; ++i) {
        std::string clause_line;
        std::getline(std::cin, clause_line);

        std::vector<Literal> literals{};
        bool negated{false};
        for (char c : clause_line) {
            if (c == ' ') continue;

            if (c == '!') {
                negated = true;
            }
            else {
                literals.emplace_back(c, negated);
                negated = false;
            }
        }
        clauses.emplace_back(literals);
    }
    return Formula{clauses};
}

int main() {
    auto formula{read_input()};
    return 0;
}

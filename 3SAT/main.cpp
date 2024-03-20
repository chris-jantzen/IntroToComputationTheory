#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>
#include <set>

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
private:
    std::vector<Clause> clauses{};
    std::set<char> variables{};
public:

    Formula() = default;
//    explicit Formula(std::vector<Clause> clauses) : clauses{std::move(clauses)} {}

    void solve() {
        // Create my permutation set which gives values for each variable
        // Create mechanism to supply the values to each clause in my list of clauses and get the boolean result
        // Combine all results and accept immediately if there is a permutation that results in all clauses having accepted
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
                    literals.emplace_back(c, negated);
                    negated = false;
                }
            }
            input_clauses.emplace_back(literals);
        }
        this->clauses = input_clauses;
    }
};

// Read in input
//Formula read_input() {
//    std::string n_in;
//    std::getline(std::cin, n_in);
//    int n {stoi(n_in)};
//    std::vector<Clause> clauses{};
//    for (int i = 0; i < n; ++i) {
//        std::string clause_line;
//        std::getline(std::cin, clause_line);
//
//        std::vector<Literal> literals{};
//        bool negated{false};
//        for (char c : clause_line) {
//            if (c == ' ') continue;
//
//            if (c == '!') {
//                negated = true;
//            }
//            else {
//                literals.emplace_back(c, negated);
//                negated = false;
//            }
//        }
//        clauses.emplace_back(literals);
//    }
//    return Formula{clauses};
//}

int main() {
//    auto formula{read_input()};
    Formula formula{};
    formula.read_input();
    return 0;
}

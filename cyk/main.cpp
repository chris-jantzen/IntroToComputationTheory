#include <iostream>
#include <unordered_set>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <map>

class Rule {
public:
    std::string variable;
    std::string product;
    bool terminal{};

    Rule(std::string variable, const std::string& product) : variable(std::move(variable)), product(product) {
        terminal = product.size() == 1;
    }
};

std::pair<std::vector<Rule>, std::vector<std::string>> read_input() {
    std::string n_in;
    std::cin >> n_in;
    int n{stoi(n_in)};

    std::vector<Rule> rules{};
    for (int i = 0; i < n; ++i) {
        std::string var;
        std::string product;
        std::cin >> var >> product;
        rules.emplace_back(var, product);
    }

    std::string t_in;
    std::cin >> t_in;
    int t{stoi(t_in)};
    std::vector<std::string> test_strings{};
    for (int i = 0; i < t; ++i) {
        std::string test_str;
        std::cin >> test_str;
        test_strings.push_back(test_str);
    }

    return std::make_pair(rules, test_strings);
}

class TableCell {
public:
    std::unordered_set<std::string> contents{};

    TableCell() = default;

    void add_variable(const std::string &var) {
        contents.insert(var);
    }

    void add_variables_range(const std::unordered_set<std::string>& var_set) {
        if (var_set.empty()) return;

        for (const std::string& s : var_set) {
            contents.insert(s);
        }
    }
};

std::string as_string(char x) {
    std::string s{x};
    return s;
}

bool cyk(
        std::string test_string,
        const std::string &start_rule_variable,
        const std::unordered_set<std::string> variables,
        const std::vector<std::unique_ptr<Rule>> &variable_rules,
        const std::vector<std::unique_ptr<Rule>> &terminal_rules) {
    auto test_string_length = test_string.size();

    if (test_string_length < 1) return false;

    if (test_string_length == 1) {
        for (const auto &r: terminal_rules) {
            if (r->variable == start_rule_variable && r->product == as_string(test_string[0])) {
                return true;
            }
        }
        return false;
    }

    std::vector<std::vector<TableCell>> table{};

    for (int i = 0; i < test_string_length; ++i) {
        table.emplace_back();
        for (auto j = test_string_length; j > i; --j) {
            table[i].push_back(TableCell{});
        }
    }

    std::map<std::string, std::map<std::string, std::unordered_set<std::string>>> var_map{};
    for (const std::string& x : variables) {
        if (x == start_rule_variable) continue;
        var_map[x] = std::map<std::string, std::unordered_set<std::string>>{};
        for (const std::string& y : variables) {
            var_map.at(x)[y] = {};
        }
    }

    for (const auto& rule : variable_rules) {
        auto first = rule->product[0];
        auto second= rule->product[1];

        var_map.at(as_string(first)).at(as_string(second)).insert(rule->variable);
    }

    for (int i = 0; i < test_string_length; ++i) {
        for (const auto &t: terminal_rules) {
            if (test_string.substr(i, 1) == t->product) {
                table[0][i].contents.insert(t->variable);
            }
        }
    }

    for (int i = 1; i < test_string_length; ++i) {
        for (int j = 0; j < test_string_length - i; ++j) {
            for (int k = 0; k < i; ++k) {
                const std::unordered_set<std::string> &left_substring_portion_vars = table[k][j].contents;
                const std::unordered_set<std::string> &right_substring_portion_vars =
                        table[i - k - 1][j + k + 1].contents;

                if (!left_substring_portion_vars.empty() && !right_substring_portion_vars.empty()) {
                    for (const std::string &l: left_substring_portion_vars) {
                        for (const std::string &r: right_substring_portion_vars) {
                            if (l == start_rule_variable || r == start_rule_variable) continue;
                            table[i][j].add_variables_range(var_map.at(l).at(r));
                        }
                    }
                }
            }
        }
    }

    for (const std::string &v: table[test_string_length - 1][0].contents) {
        if (v == start_rule_variable) {
            return true;
        }
    }
    return false;
}

int main() {
    auto [rules, test_strings] = read_input();

    std::string start_rule_variable = rules[0].variable;

    std::vector<std::unique_ptr<Rule>> variable_rules{};
    std::vector<std::unique_ptr<Rule>> terminal_rules{};
    std::unordered_set<std::string> variables{};
    for (const Rule &rule: rules) {
        variables.insert(rule.variable);
        if (rule.terminal) {
            terminal_rules.push_back(std::make_unique<Rule>(rule));
        } else {
            variable_rules.push_back(std::make_unique<Rule>(rule));
        }
    }

    for (const auto &test_string: test_strings) {
        bool can_be_generated = cyk(test_string, start_rule_variable, variables, variable_rules, terminal_rules);
        if (can_be_generated) {
            std::cout << "yes" << std::endl;
        } else {
            std::cout << "no" << std::endl;
        }
    }

    return 0;
}

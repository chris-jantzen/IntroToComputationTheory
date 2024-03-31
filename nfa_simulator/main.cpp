#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <map>

#define EPSILON "eps"

bool append_unique(std::vector<int> &v, int val) {
    for (const auto &i: v) {
        if (i == val) {
            return false;
        }
    }

    v.push_back(val);
    return true;
}

class TransitionPair {
public:
    std::string symbol;
    std::set<int> to;

    explicit TransitionPair(std::string sym) : symbol{std::move(sym)} {}
};

class NFA {
public:
    struct Transition {
        int from;
        std::string symbol;
        int to;
    };

    std::set<std::string> alphabet;
    std::map<int, std::vector<TransitionPair>> transition_table;
    std::map<int, std::set<int>> epsilon_expansions;
    std::map<int, bool> acceptStates;

    NFA(std::set<std::string> alphabet,
        std::map<int, std::vector<TransitionPair>> transitionTable,
        std::map<int, std::set<int>> epsilon_expansions,
        std::map<int, bool> acceptStates)
            : alphabet{std::move(alphabet)},
              transition_table{std::move(transitionTable)},
              epsilon_expansions{std::move(epsilon_expansions)},
              acceptStates{std::move(acceptStates)} {}

    static NFA::Transition split_transition_input(std::string transitionString) {
        std::vector<std::string> pieces{};

        unsigned long position{0};
        std::string delimiter{" "};
        while (position < transitionString.size()) {
            position = transitionString.find(delimiter);
            pieces.push_back(transitionString.substr(0, position));
            transitionString.erase(0, position + delimiter.size());
        }
        if (!transitionString.empty())
            pieces.push_back(transitionString);

        return {stoi(pieces[0]), pieces[1], stoi(pieces[2])};
    }

    [[nodiscard]] bool is_accept_state(const int state) const {
        return acceptStates.at(state);
    }
};

std::map<int, std::set<int>>
populate_epsilon_transitions_map(const std::vector<NFA::Transition> &transitions, const int number_of_states) {
    std::map<int, std::set<int>> epsilon_expansions_map{};
    for (int i = 0; i < number_of_states; ++i) {
        epsilon_expansions_map[i] = {};
    }

    for (const NFA::Transition &t: transitions) {
        if (t.symbol == EPSILON) {
            epsilon_expansions_map.at(t.from).insert(t.to);
        }
    }

    return epsilon_expansions_map;
}

std::vector<int> get_epsilon_expansion_states(
        const NFA::Transition &transition,
        const std::map<int, std::set<int>> &epsilon_expansions_map) {
    std::vector<int> states_to_expand{transition.from, transition.to};
    int i{0};
    while (i < states_to_expand.size()) {
        int current_state_to_expand = states_to_expand[i];

        // Debugging
        if (current_state_to_expand > epsilon_expansions_map.size()) return states_to_expand;

        std::set<int> states = epsilon_expansions_map.at(current_state_to_expand);
        for (int s: states) {
            append_unique(states_to_expand, s);
        }
        i++;
    }

    return states_to_expand;
}

std::pair<std::map<int, std::vector<TransitionPair>>, std::map<int, std::set<int>>>
construct_transitions_table_for_alphabet(int number_of_states,
                                         const std::set<std::string> &alphabet,
                                         const std::vector<NFA::Transition> &transitions) {
    std::map<int, std::vector<TransitionPair>> transitions_table{};
    std::map<int, std::set<int>> epsilon_expansion_transitions_map{
            populate_epsilon_transitions_map(transitions, number_of_states)};

    // Map state => the set of states the machine would expand to be in upon
    // reaching this state.
    std::map<int, std::set<int>> epsilon_expansions{};
    for (int i = 0; i < number_of_states; ++i) {
        epsilon_expansions[i] = {};
    }

    // Initialize table cells for all states and symbols
    for (int i = 0; i < number_of_states; ++i) {
        for (const auto &symbol: alphabet) {
            TransitionPair p{symbol};
            transitions_table[i].push_back(p);
        }
    }

    // Populate the epsilon expansions first
    for (const NFA::Transition &transition: transitions) {
        if (transition.symbol != EPSILON) continue;

        std::set<int> &item = epsilon_expansions.at(transition.from);
        std::vector<int> states_to_expand{get_epsilon_expansion_states(
                transition,
                epsilon_expansion_transitions_map)};

        for (const int state: states_to_expand) {
            item.insert(state);
        }
    }

    for (int i = 0; i < number_of_states; ++i) {
        std::set<int> epsilon_expanded_states{i};
        std::set<int> expansions = epsilon_expansions.at(i);
        // Populate epsilon expanded states set
        for (const auto cell: expansions) {
            epsilon_expanded_states.insert(cell);
        }

        for (const std::string &symbol: alphabet) {
            for (auto state: epsilon_expanded_states) {
                for (const NFA::Transition &t: transitions) {
                    if (state == t.from && t.symbol == symbol) {
                        for (TransitionPair &p: transitions_table.at(i)) {
                            if (p.symbol == symbol) {
                                p.to.insert(t.to);
                            }
                        }
                    }
                }
            }
        }
    }

    return std::pair<std::map<int, std::vector<TransitionPair>>,
            std::map<int, std::set<int>>>{transitions_table, epsilon_expansions};
}

std::pair<std::map<int, std::set<int>>, std::map<int, std::map<std::string, std::set<int>>>> populate_transitions_table(
    int number_of_states,
    const std::set<std::string>& alphabet,
    const std::vector<NFA::Transition>& transitions) {

    std::map<int, std::map<std::string, std::set<int>>> transitions_table{};
    std::map<int, std::set<int>> epsilon_expansions{};

    for (int i = 0; i < number_of_states; ++i) {
        epsilon_expansions[i] = {};
        transitions_table[i] = {};
        for (const std::string& letter : alphabet) {
            transitions_table.at(i)[letter] = {};
        }
    }

    for (const auto& t : transitions) {
        if (t.symbol == EPSILON) {
            epsilon_expansions.at(t.from).insert(t.to);
        } else {
            transitions_table.at(t.from).at(t.symbol).insert(t.to);
        }
    }
};

NFA read_nfa_definition_input() {
    std::string n_in;
    getline(std::cin, n_in);
    int n{stoi(n_in)};

    std::string t_in;
    getline(std::cin, t_in);
    int t{stoi(t_in)};

    std::vector<NFA::Transition> transitions{};
    for (int i = 0; i < t; ++i) {
        std::string transition_string;
        std::getline(std::cin, transition_string);
        transitions.push_back(NFA::split_transition_input(transition_string));
    }

    std::set<std::string> alphabet{};
    for (const auto &transition: transitions) {
        alphabet.insert(transition.symbol);
    }
    alphabet.erase(EPSILON);

    auto transitions_data =
            construct_transitions_table_for_alphabet(n, alphabet, transitions);

    std::string f_in;
    getline(std::cin, f_in);
    int f{stoi(f_in)};

    std::map<int, bool> accept_states{};
    for (int i = 0; i < n; ++i) {
        accept_states[i] = false;
    }

    for (int i = 0; i < f; ++i) {
        std::string accept_state;
        getline(std::cin, accept_state);
        accept_states[stoi(accept_state)] = true;
    }

    return {alphabet, transitions_data.first, transitions_data.second,
            accept_states};
}

std::vector<std::string> read_test_strings() {
    std::string s_in;
    getline(std::cin, s_in);
    int s{stoi(s_in)};

    std::vector<std::string> test_strings{};

    for (int i = 0; i < s; ++i) {
        std::string test_string;
        getline(std::cin, test_string);
        test_strings.push_back(test_string);
    }

    return test_strings;
}

std::string asString(char x) {
    std::string s{x};
    return s;
}

void produceOutput(const std::set<int> &current_states, const NFA &n) {
    for (const int state: current_states) {
        if (n.is_accept_state(state)) {
            std::cout << "accept" << std::endl;
            return;
        }
    }
    std::cout << "reject" << std::endl;
}

bool check_if_test_string_has_char_not_in_alphabet(const std::string& test_string, const NFA& n) {
    bool contains_char_not_in_alphabet{false};
    for (const auto s: test_string) {
        if (asString(s).empty()) contains_char_not_in_alphabet = false;

        bool symbol_in_alphabet{false};
        for (const auto &symbol : n.alphabet) {
            if (symbol == asString(s)) {
                symbol_in_alphabet = true;
                break;
            }
        }
        if (!symbol_in_alphabet) {
            contains_char_not_in_alphabet = true;
            break;
        }
    }
    return contains_char_not_in_alphabet;
}

int main() {
    NFA n{read_nfa_definition_input()};
    std::vector<std::string> testStrings{read_test_strings()};

    std::set<int> start_states{0};
    for (const auto &s: n.epsilon_expansions.at(0)) {
        start_states.insert(s);
    }
    for (const std::string &test_string: testStrings) {
        std::set<int> current_states{start_states};
        std::set<int> next_states{};

        if (test_string.empty()) {
            produceOutput(current_states, n);
            continue;
        }

        bool contains_char_not_in_alphabet{check_if_test_string_has_char_not_in_alphabet(test_string, n)};
        if (contains_char_not_in_alphabet) {
            std::cout << "reject" << '\n';
            continue;
        }

        for (int j = 0; j < test_string.length(); ++j) {
            for (const int state : current_states) {
                for (const TransitionPair &transition_states: n.transition_table.at(state)) {
                    if (transition_states.symbol == asString(test_string[j])) {
                        for (const int s: transition_states.to) {
                            next_states.insert(s);
                            std::set<int> eps_transitions = n.epsilon_expansions.at(s);
                            for (const int eps_transition: eps_transitions) {
                                next_states.insert(eps_transition);
                            }
                        }
                    }
                }
            }

            current_states.clear();
            if (!next_states.empty()) {
                current_states = next_states;
                next_states.clear();
            }
        }

        produceOutput(current_states, n);
    }

    return 0;
}
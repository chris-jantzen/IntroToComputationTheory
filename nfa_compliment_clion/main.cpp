#include <iostream>
#include <string>
#include <set>
#include <unordered_set>
#include <map>
#include <utility>
#include <iterator>
#include <vector>
#include <cmath>

#define EPSILON "eps"

/**
 * Construct the NFA table fully epsilon expanded
 * Create the DFA table starting from the expanded start state of the NFA
 * After filling out the state transition sets for each symbol in the alphabet, add any new k-tuples to the table as keys and continue filling out the table until reaching the end
 * Give each state an identifier
 * Mark any states that contain any of the NFA final states as accept states
 * Invert the accept states
 * Convert the transitions between state identifiers into strings to log
 */

bool append_unique(std::vector<int>& v, int val)
{
    for (const auto& i : v)
    {
        if (i == val)
        {
            return false;
        }
    }

    v.push_back(val);
}

bool operator<(const std::set<int>& s1, const std::set<int>& s2)
{
    if (s1.size() > s2.size()) return false;
    else if (s1.size() < s2.size()) return true;

    for (int i = 0; i < s1.size(); ++i)
    {
        int s1_value = *std::next(s1.begin(), i);
        int s2_value = *std::next(s2.begin(), i);
        if (s1_value < s2_value)
        {
            return true;
        }
    }

    // The two sets are identical
    return false;
}

class DFA_State
{
private:
    inline static int next_id{0};
public:
    int id;
    std::set<int> state_set;
    std::map<std::string, std::set<int>> transitions{};
    bool is_accept_state{false};

    DFA_State(const std::set<int>& state_set, const std::set<std::string>& alphabet,
              const std::map<int, bool>& nfa_accept_states)
        : state_set{state_set} {
        for (const std::string& symbol : alphabet)
        {
            transitions[symbol] = {};
        }

        for (int i : state_set)
        {
            if (nfa_accept_states.at(i))
            {
                is_accept_state = true;
                break;
            }
        }

        id = next_id++;
    }

    bool operator== (const DFA_State& ds) const
    {
        if (this->state_set.size() != ds.state_set.size()) return false;

        if (this->state_set.empty() && ds.state_set.empty()) return true;

        for (int i = 0; i < this->state_set.size(); ++i)
        {
            int s1_value = *std::next(this->state_set.begin(), i);
            int s2_value = *std::next(ds.state_set.begin(), i);
            if (s1_value != s2_value)
            {
                return false;
            }
        }

        return true;
    }

    bool operator< (const DFA_State& ds) const
    {
        if (this->state_set.size() > ds.state_set.size()) return false;
        else if (this->state_set.size() < ds.state_set.size()) return true;

        // They will have to have the same size to have gotten here
        for (int i = 0; i < this->state_set.size(); ++i)
        {
            int s1_value = *std::next(this->state_set.begin(), i);
            int s2_value = *std::next(ds.state_set.begin(), i);
            if (s1_value < s2_value)
            {
                return true;
            }
        }

        // The two sets are identical
        return false;
    }
};

class TransitionPair
{
public:
    std::string symbol;
    std::set<int> to;

    explicit TransitionPair(std::string sym) : symbol{std::move(sym)} {}

    bool append_to_state_value(int to_value)
    {
        auto out = to.insert(to_value);
        return out.second;
    }
};

class NFA
{
public:
    struct Transition
    {
        int from;
        std::string symbol;
        int to;
    };

    int states{};
    std::set<std::string> alphabet;
    std::map<int, std::vector<TransitionPair>> transitionTable;
    std::map<int, bool> acceptStates;

    NFA(int states, std::set<std::string> alphabet, std::map<int, std::vector<TransitionPair>> transitionTable, std::map<int, bool> acceptStates)
            : states{states}, alphabet{std::move(alphabet)}, transitionTable{std::move(transitionTable)}, acceptStates{std::move(acceptStates)} {}

    inline bool isAcceptState(int state) const
    {
        return acceptStates.at(state);
    }
};

NFA::Transition split_transition_input(std::string transitionString)
{
    std::vector<std::string> pieces{};

    unsigned long position{0};
    std::string delimiter{" "};
    while (position < transitionString.size())
    {
        position = transitionString.find(delimiter);
        pieces.push_back(transitionString.substr(0, position));
        transitionString.erase(0, position + delimiter.size());
    }
    if (!transitionString.empty())
        pieces.push_back(transitionString);

    return {stoi(pieces[0]), pieces[1], stoi(pieces[2])};
}

std::map<int, std::set<int>> populate_epsilon_transitions_map(const std::vector<NFA::Transition>& transitions)
{
    std::map<int, std::set<int>> epsilon_expansions_map{};
    bool foundEpsilon{false};
    for (NFA::Transition t : transitions) {
        epsilon_expansions_map[t.from] = {};
        if (t.symbol == EPSILON) {
            foundEpsilon = true;
        }
    }
    if (!foundEpsilon)
        return epsilon_expansions_map;

    for (NFA::Transition t : transitions)
    {
        if (t.symbol == EPSILON)
        {
            epsilon_expansions_map.at(t.from).insert(t.to);
        }
    }
    return epsilon_expansions_map;
}

std::map<int, std::vector<TransitionPair>> construct_transitions_table_for_alphabet(
    int number_of_states,
    const std::set<std::string>& alphabet,
    const std::vector<NFA::Transition>& transitions)
{
    std::map<int, std::vector<TransitionPair>> transitions_table{};
    std::map<int, std::set<int>> epsilon_expansions_map{populate_epsilon_transitions_map(transitions)};

    // Initialize table cells for all states and symbols
    for (int i = 0; i < number_of_states; ++i)
    {
        for (const auto& symbol : alphabet)
        {
            TransitionPair p{symbol};
            transitions_table[i].push_back(p);
        }
    }

    // Populate the epsilon expansions first
    for (const NFA::Transition& transition : transitions)
    {
        if (transition.symbol != EPSILON)
        {
            continue;
        }
        for (auto &item : transitions_table.at(transition.from))
        {
            if (item.symbol == EPSILON)
            {
                std::vector<int> states_to_expand{transition.from, transition.to};
                int i{0};
                while (i < states_to_expand.size())
                {
                    int current_state_to_expand = states_to_expand[i];
                    std::set<int> states = epsilon_expansions_map.at(current_state_to_expand);
                    for (int s : states)
                    {
                        append_unique(states_to_expand, s);
                    }
                    i++;
                }

                for (int state : states_to_expand)
                {
                    item.append_to_state_value(state);
                }
            }
        }
    }

    // ALL TRANSITIONS
    for (const NFA::Transition& transition : transitions)
    {
        for (auto &item : transitions_table.at(transition.from))
        {
            if (item.symbol == transition.symbol && transition.symbol != EPSILON)
            {
                item.append_to_state_value(transition.to);

                // TODO: make this a shared_ptr or unique_ptr
                std::set<int> epsilon_expanded_states{};
                std::vector<TransitionPair> a = transitions_table.at(transition.from);
                // TODO: Is there a c++ find-where type function?
                for (const auto& s : a)
                {
                    if (s.symbol == EPSILON)
                    {
                        epsilon_expanded_states = s.to;
                    }
                }

                std::set<int> set_of_epsilon_states{};
                for (auto s : epsilon_expanded_states)
                {
                    // get transitions_table.at(s) where symbol == EPSILON, grab those states
                    // for each of those states, look through the input transitions using them as to states, getting
                    // the "from" states for the current symbol
                }
            }
        }
    }

//    std::map<int, bool> states_with_exiting_epsilon_transition{};
//    for (int i = 0; i < number_of_states; ++i)
//    {
//        states_with_exiting_epsilon_transition[i] = false;
//    }


//    for (const auto& t : transitions)
//    {
//        std::pair<std::string, std::set<int>> p{};
//        for (auto &item : transitions_table[t.from])
//        {
//            if (item.symbol == t.symbol)
//            {
//                item.append_to_state_value(t.to);
//                if (item.symbol == EPSILON)
//                {
//                    states_with_exiting_epsilon_transition[t.from] = true;
//                    for (auto &xitem : transitions_table[t.from])
//                    {
//                        for (auto epsState : item.to)
//                        {
//                            xitem.append_to_state_value(epsState);
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    bool shouldContinue;
//    int i{0};
//    do {
//        shouldContinue = false;
//
//        for (const auto& t : transitions_table)
//        {
//            for (auto &p : transitions_table[t.first])
//            {
//                for (int trans : p.to)
//                {
//                    if (states_with_exiting_epsilon_transition.at(trans))
//                    {
//                        for (const TransitionPair& x : transitions_table.at(trans))
//                        {
//                            if (x.symbol == EPSILON)
//                            {
//                                for (int q : x.to)
//                                {
//                                    auto inserted = p.append_to_state_value(q);
//                                    if (inserted)
//                                        shouldContinue = true;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//        ++i;
//        if (i > number_of_states)
//            break;
//    } while(shouldContinue);

    return transitions_table;
}

NFA read_nfa_definition_input()
{
    std::string n_in;
    getline(std::cin, n_in);
    int n{stoi(n_in)};

    std::string t_in;
    getline(std::cin, t_in);
    int t{stoi(t_in)};

    std::vector<NFA::Transition> transitions{};
    for (int i = 0; i < t; ++i)
    {
        std::string transitionString;
        std::getline(std::cin, transitionString);
        transitions.push_back(split_transition_input(transitionString));
    }

    std::set<std::string> alphabet{EPSILON};
    for (const auto& transition : transitions)
    {
        alphabet.insert(transition.symbol);
    }

    auto transitionsTable = construct_transitions_table_for_alphabet(n, alphabet, transitions);

    std::string f_in;
    getline(std::cin, f_in);
    int f{stoi(f_in)};

    std::map<int, bool> acceptStates{};
    for (int i = 0; i < n; ++i)
    {
        acceptStates[i] = false;
    }

    for (int i = 0; i < f; ++i)
    {
        std::string acceptState;
        getline(std::cin, acceptState);
        acceptStates[stoi(acceptState)] = true;
    }

    return {n, alphabet, transitionsTable, acceptStates};
}

//std::vector<std::string> read_test_strings()
//{
//    std::string s_in;
//    getline(std::cin, s_in);
//    int s{stoi(s_in)};
//
//    std::vector<std::string> test_strings{};
//
//    for (int i = 0; i < s; ++i)
//    {
//        std::string test_string;
//        getline(std::cin, test_string);
//        test_strings.push_back(test_string);
//    }
//
//    return test_strings;
//}

int main() {
    NFA n{read_nfa_definition_input()};
    //    std::vector<std::string> test_strings{read_test_strings()};

    int max_dfa_state_count{static_cast<int>(pow(2, n.states))};

    // Get the nfa epsilon expanded start states
    std::set<int> start_states{0};
    // TODO: Try a case with three epsilon transitions in a row off of the start state to make sure this works the way it should
    for (const auto& p : n.transitionTable.at(0))
    {
        if (p.symbol == EPSILON)
        {
            for (auto s : p.to)
            {
                start_states.insert(s);
            }
        }
    }

    std::set<std::string> dfa_alphabet{n.alphabet};
    dfa_alphabet.erase(EPSILON);

    // start state
    DFA_State dfa_start_state{start_states, dfa_alphabet, n.acceptStates};
    std::vector<DFA_State> dfa_states{dfa_start_state};

    // for (DFA_State dfa_state : dfa_states)
    int i{0};
    do
    {
        DFA_State &dfa_state = dfa_states[i];

        std::vector<DFA_State> new_dfa_states{};

        // Go through each state in the state set
        for (int nfa_state : dfa_state.state_set)
        {
            // Look in the NFA transition table for each state to get what set of states you
            // would be in after taking each one and take the union of all of them
            for (const TransitionPair& t : n.transitionTable.at(nfa_state))
            {
                // Don't need epsilon in a DFA; nfa table is already epsilon expanded
                if (t.symbol == EPSILON) continue;

                std::set<int> &set = dfa_state.transitions.at(t.symbol);

                for (int state : t.to)
                {
                    set.insert(state);
                }
            }
        }

        // For each "to" transition in the dfa state, add a new DFA state if it wasn't already in the table
        for (const std::string &symbol : dfa_alphabet)
        {
            std::set<int> set_states{};
            for (const int t : dfa_state.transitions.at(symbol))
            {
                set_states.insert(t);
            }
            DFA_State new_state{set_states, dfa_alphabet, n.acceptStates};
            bool should_add{true};
            for (const DFA_State& ds : dfa_states)
            {
                if (new_state == ds)
                {
                    should_add = false;
                    break;
                }
            }
            if (should_add)
            {
                new_dfa_states.push_back(new_state);
            }
        }

        for (const DFA_State& ns : new_dfa_states)
        {
            dfa_states.push_back(ns);
        }
        i++;
        if (i == dfa_states.size() || dfa_states.size() > max_dfa_state_count)
        {
            break;
        }
    } while (true);

    /**
     * I think this is solved:
     * Things to deal with for DFAs:
     * Sink states when the transitions for any of a DFA_State's symbols is an empty list
     * Either:
     *   - immediately create the sink state every time as dfa_states[0] and then the start
     *     state as dfa_states[1] and start doing the DFA table population at i=1
     *   - Add the sink state as soon as I encounter a symbol with a transition "to" set of length 0
     *     after having finished it's NFA table reading and append it to the new states.
     *   NOTE: It may already be handled just the way it is where an empty set_states list will be
     *     passed in the constructor of one of these new states. Just have to make sure nothing goes
     *     wrong when it goes through to have it's transitions populated in the table.
     *     (Use sample 3 input to test this).
     */

    return 0;
}
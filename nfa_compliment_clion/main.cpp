#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#define EPSILON "eps"

bool append_unique(std::vector<int> &v, int val) {
  for (const auto &i : v) {
    if (i == val) {
      return false;
    }
  }

  v.push_back(val);
  return true;
}

bool operator<(const std::set<int> &s1, const std::set<int> &s2) {
  if (s1.size() > s2.size())
    return false;
  else if (s1.size() < s2.size())
    return true;

  for (int i = 0; i < s1.size(); ++i) {
    int s1_value = *std::next(s1.begin(), i);
    int s2_value = *std::next(s2.begin(), i);
    if (s1_value < s2_value) {
      return true;
    }
  }

  // The two sets are identical
  return false;
}

class DFA_State {
private:
  int id{};

public:
  std::set<int> state_set;
  std::map<std::string, std::set<int>> transitions{};
  bool is_accept_state{false};
  bool is_sink_state{false};

  DFA_State(const std::set<int> &state_set,
            const std::set<std::string> &alphabet,
            const std::map<int, bool> &nfa_accept_states)
      : state_set{state_set} {
    for (const std::string &symbol : alphabet) {
      transitions[symbol] = {};
    }

    for (int i : state_set) {
      if (nfa_accept_states.at(i)) {
        is_accept_state = true;
        break;
      }
    }

    if (state_set.empty()) {
      is_sink_state = true;
    }
  }

  void convert_state_to_compliment(int _id) {
    this->id = _id;
    flip_accept_state_status();
  }

  inline void flip_accept_state_status() {
    this->is_accept_state = !is_accept_state;
  }

  std::vector<std::string>
  transitions_to_string(std::vector<DFA_State> dfa_states) const {
    std::vector<std::string> transitions_to_string{};
    for (const auto &t : transitions) {
      if (!is_sink_state || is_accept_state) {
        for (const auto &state : dfa_states) {
          if (state.state_set == t.second) {
            std::string transition_str = std::to_string(get_id()) + " " +
                                         t.first + " " +
                                         std::to_string(state.get_id());
            transitions_to_string.push_back(transition_str);
          }
        }
      }
    }

    return transitions_to_string;
  }

  inline void set_id(int _id) { this->id = _id; }

  [[nodiscard]] inline int get_id() const { return this->id; }

  bool operator==(const DFA_State &ds) const {
    if (this->state_set.size() != ds.state_set.size())
      return false;

    if (this->state_set.empty() && ds.state_set.empty())
      return true;

    for (int i = 0; i < this->state_set.size(); ++i) {
      int s1_value = *std::next(this->state_set.begin(), i);
      int s2_value = *std::next(ds.state_set.begin(), i);
      if (s1_value != s2_value) {
        return false;
      }
    }

    return true;
  }

  bool operator<(const DFA_State &ds) const {
    if (this->state_set.size() > ds.state_set.size())
      return false;
    else if (this->state_set.size() < ds.state_set.size())
      return true;

    // They will have to have the same size to have gotten here
    for (int i = 0; i < this->state_set.size(); ++i) {
      int s1_value = *std::next(this->state_set.begin(), i);
      int s2_value = *std::next(ds.state_set.begin(), i);
      if (s1_value < s2_value) {
        return true;
      }
    }

    // The two sets are identical
    return false;
  }
};

bool operator==(const std::set<int> &s1, const std::set<int> &s2) {
  if (s1.size() != s2.size())
    return false;

  if (s1.empty() && s2.empty())
    return true;

  for (int i = 0; i < s1.size(); ++i) {
    int s1_value = *std::next(s1.begin(), i);
    int s2_value = *std::next(s2.begin(), i);
    if (s1_value != s2_value) {
      return false;
    }
  }

  return true;
}

bool operator!=(const std::set<int> &s1, const std::set<int> &s2) {
  if (s1.size() != s2.size())
    return true;
  if (s1.empty() && s2.empty())
    return false;

  for (int i = 0; i < s1.size(); ++i) {
    int s1_value = *std::next(s1.begin(), i);
    int s2_value = *std::next(s2.begin(), i);
    if (s1_value != s2_value) {
      return true;
    }
  }

  return false;
}

std::ostream &operator<<(std::ostream &stream, std::vector<std::string> vec) {
  for (const std::string &s : vec) {
    stream << s << '\n';
  }
  return stream;
}
std::ostream &operator<<(std::ostream &stream, std::vector<int> vec) {
  for (const int n : vec) {
    stream << n << '\n';
  }
  return stream;
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

  int states{};
  std::set<std::string> alphabet;
  std::map<int, std::vector<TransitionPair>> transition_table;
  std::map<int, std::set<int>> epsilon_expansions;
  std::map<int, bool> acceptStates;

  NFA(int states, std::set<std::string> alphabet,
      std::map<int, std::vector<TransitionPair>> transitionTable,
      std::map<int, std::set<int>> epsilon_expansions,
      std::map<int, bool> acceptStates)
      : states{states}, alphabet{std::move(alphabet)},
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
};

std::map<int, std::set<int>> populate_epsilon_transitions_map(
    const std::vector<NFA::Transition> &transitions,
    const int number_of_states) {
  std::map<int, std::set<int>> epsilon_expansions_map{};
  bool foundEpsilon{false};
  for (int i = 0; i < number_of_states; ++i) {
    epsilon_expansions_map[i] = {};
  }
  for (const NFA::Transition &t : transitions) {
    if (t.symbol == EPSILON) {
      foundEpsilon = true;
      break;
    }
  }
  if (!foundEpsilon)
    return epsilon_expansions_map;

  for (const NFA::Transition &t : transitions) {
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
    std::set<int> states = epsilon_expansions_map.at(current_state_to_expand);
    for (int s : states) {
      append_unique(states_to_expand, s);
    }
    i++;
  }

  return states_to_expand;
}

std::pair<std::map<int, std::vector<TransitionPair>>,
          std::map<int, std::set<int>>>
construct_transitions_table_for_alphabet(
    int number_of_states, const std::set<std::string> &alphabet,
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
    for (const auto &symbol : alphabet) {
      TransitionPair p{symbol};
      transitions_table[i].push_back(p);
    }
  }

  // Populate the epsilon expansions first
  for (const NFA::Transition &transition : transitions) {
    if (transition.symbol != EPSILON) {
      continue;
    }
    std::set<int> &item = epsilon_expansions.at(transition.from);
    std::vector<int> states_to_expand{get_epsilon_expansion_states(
        transition, epsilon_expansion_transitions_map)};

    for (int state : states_to_expand) {
      item.insert(state);
    }
  }

  for (int i = 0; i < number_of_states; ++i) {
    std::set<int> epsilon_expanded_states{i};
    std::set<int> expansions = epsilon_expansions.at(i);
    // Populate epsilon expanded states set
    for (const auto cell : expansions) {
      epsilon_expanded_states.insert(cell);
    }

    for (const std::string &symbol : alphabet) {
      for (auto state : epsilon_expanded_states) {
        for (const NFA::Transition &t : transitions) {
          if (state == t.from && t.symbol == symbol) {
            for (TransitionPair &p : transitions_table.at(i)) {
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
                   std::map<int, std::set<int>>>{transitions_table,
                                                 epsilon_expansions};
}

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
  for (const auto &transition : transitions) {
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

  return {n, alphabet, transitions_data.first, transitions_data.second,
          accept_states};
}

int main() {
  NFA n{read_nfa_definition_input()};

  int max_dfa_state_count{static_cast<int>(pow(2, n.states))};

  // Get the nfa epsilon expanded start states
  std::set<int> start_states{0};
  for (const auto &s : n.epsilon_expansions.at(0)) {
    start_states.insert(s);
  }

  // start state
  DFA_State dfa_start_state{start_states, n.alphabet, n.acceptStates};
  std::vector<DFA_State> dfa_states{dfa_start_state};

  int i{0};
  do {
    DFA_State &dfa_state = dfa_states[i];

    std::vector<DFA_State> new_dfa_states{};

    // Using the current dfa state's state_set, get the set of states each one
    // of those states will take you to for each symbol. Keep the union of all
    // of them for each symbol
    for (int nfa_state : dfa_state.state_set) {
      for (const TransitionPair &t : n.transition_table.at(nfa_state)) {
        if (t.symbol == EPSILON)
          continue;
        std::set<int> &set = dfa_state.transitions.at(t.symbol);
        for (int state : t.to) {
          set.insert(state);

          for (const auto &s : n.epsilon_expansions.at(state)) {
            set.insert(s);
          }
        }
      }
    }

    // For each "to" transition in the dfa state, add a new DFA state if it
    // wasn't already in the table
    for (const std::string &symbol : n.alphabet) {
      std::set<int> set_states{dfa_state.transitions.at(symbol)};
      DFA_State new_state{set_states, n.alphabet, n.acceptStates};
      bool should_add{true};
      // Confirm that the state isn't already in the dfa table
      for (const DFA_State &ds : dfa_states) {
        if (new_state == ds) {
          should_add = false;
          break;
        }
      }
      if (should_add) {
        // Confirm that the state isn't added multiple times from symbols in the
        // current table row
        for (const DFA_State &ds : new_dfa_states) {
          if (new_state == ds) {
            should_add = false;
            break;
          }
        }
      }
      if (should_add) {
        new_dfa_states.push_back(new_state);
      }
    }

    for (const DFA_State &ns : new_dfa_states) {
      dfa_states.push_back(ns);
    }
    i++;
    if (i == dfa_states.size() || dfa_states.size() > max_dfa_state_count) {
      break;
    }
  } while (true);

  // Convert to compliment
  for (int d = 0; d < dfa_states.size(); ++d) {
    dfa_states[d].convert_state_to_compliment(d);
  }

  std::vector<int> accept_states{};
  std::vector<std::string> transitions_to_string{};
  for (const auto &dfa_state : dfa_states) {
    if (dfa_state.is_accept_state) {
      accept_states.push_back(dfa_state.get_id());
    }
    for (auto str_transition : dfa_state.transitions_to_string(dfa_states)) {
      transitions_to_string.push_back(str_transition);
    }
  }

  std::cout << dfa_states.size() << '\n';
  std::cout << transitions_to_string.size() << '\n';
  std::cout << transitions_to_string;

  std::cout << accept_states.size() << '\n';
  std::cout << accept_states;

  return 0;
}

// input description:
// n indicating the number of states in n, 0 to n-1 where 0 is the start state.
// integer indicating the number of transitions that follow given as tokens a b
// c
// -> a is the source state, c is the destination state, b is either 'eps' or
// the symbol for the transition.
// Following is an integer f for the number of final states
// The next {f} lines indicate which of the states are final accept states
// Then an integer {s} is given indicating the number of lines of strings to
// test in the machine to follow. Alphabet is [a-z]

#include <iostream>
#include <memory>
#include <string>
#include <vector>

bool test_run = true;

class Transition {
public:
  // (from, symbol) -> to
  int from;
  int to;
  std::string symbol;

  Transition(int f, std::string sym, int t) : from{f}, symbol{sym}, to{t} {}
};

class NFA {
public:
  int states{};
  std::vector<Transition> transitions;
  std::vector<int> acceptStates;
  NFA(int states, std::vector<Transition> transitions,
      std::vector<int> acceptStates)
      : states{states}, transitions{transitions}, acceptStates{acceptStates} {}

  void print_definition() {
    std::cout << "Machine definition" << '\n';
    std::cout << "Number of states: " << states << '\n';
    std::cout << "Alphabet: [a-z]" << '\n';
    std::cout << "Accept states: " << '\n';
    print_acceptStates();

    std::cout << "transitions: " << '\n';
    print_transitions();
    std::cout << "End Machine Definition" << std::endl;
  }

  void print_transitions() {
    for (int i = 0; i < transitions.size(); ++i) {
      std::cout << transitions[i].from << " " << transitions[i].symbol << " "
                << transitions[i].to << '\n';
    }
  }

  void print_acceptStates() {
    for (int i = 0; i < acceptStates.size(); ++i) {
      std::cout << acceptStates[i] << ' ';
    }
    std::cout << '\n';
  }
};

Transition split_transition_input(std::string transitionString) {
  std::vector<std::string> pieces{};

  int position{0};
  std::string delimiter{" "};
  while (position < transitionString.size()) {
    position = transitionString.find(delimiter);
    pieces.push_back(transitionString.substr(0, position));
    transitionString.erase(0, position + delimiter.size());
  }
  if (transitionString != "")
    pieces.push_back(transitionString);

  Transition t{stoi(pieces[0]), pieces[1], stoi(pieces[2])};
  return t;
}

NFA construct_machine_definition() {
  std::string n_in;
  if (test_run) {
    n_in = "4";
  } else {
    getline(std::cin, n_in);
  }

  // number of states (0 is the start state up to n-1)
  int n{stoi(n_in)};

  std::string t_in;
  if (test_run) {
    t_in = "5";
  } else {
    getline(std::cin, t_in);
  }
  int t{stoi(t_in)};

  std::vector<Transition> transitions{};
  if (test_run) {
    transitions.push_back(split_transition_input("0 a 0"));
    transitions.push_back(split_transition_input("0 b 0"));
    transitions.push_back(split_transition_input("0 a 1"));
    transitions.push_back(split_transition_input("1 b 2"));
    transitions.push_back(split_transition_input("2 a 3"));
  } else {
    for (int i = 0; i < t; ++i) {
      std::string transitionString;
      std::getline(std::cin, transitionString);
      transitions.push_back(split_transition_input(transitionString));
    }
  }

  std::string f_in;
  if (test_run) {
    f_in = "1";
  } else {
    getline(std::cin, f_in);
  }
  int f{stoi(f_in)};

  std::vector<int> acceptStates{};

  if (test_run) {
    std::string acceptState{"3"};
    acceptStates.push_back(stoi(acceptState));
  } else {
    for (int i = 0; i < f; ++i) {
      std::string acceptState;
      getline(std::cin, acceptState);
      acceptStates.push_back(stoi(acceptState));
    }
  }

  NFA machine_n{n, transitions, acceptStates};

  // machine_n.print_definition();

  return machine_n;
}

std::vector<std::string> read_test_strings() {
  std::string s_in;
  if (test_run) {
    s_in = "7";
  } else {
    getline(std::cin, s_in);
  }
  int s{stoi(s_in)};

  std::vector<std::string> test_strings{};

  if (test_run) {
    test_strings.push_back("aba");
    test_strings.push_back("bab");
    test_strings.push_back("baba");
    test_strings.push_back("aaabaaaaa");
    test_strings.push_back("bbbbbbbbbb");
    test_strings.push_back("aaaaaaaaa");
    test_strings.push_back("aaaaaabbbbaba");
  } else {
    for (int i = 0; i < s; ++i) {
      std::string test_string;
      getline(std::cin, test_string);
      test_strings.push_back(test_string);
    }
  }

  // for (int i = 0; i < test_strings.size(); ++i)
  // {
  //   std::cout << test_strings[i] << '\n';
  // }

  return test_strings;
}

// ----------------------------------------------------------------
// ->(q0) -> (q1) -> (q2) -> ((q3)) f=3
// ----------------------------------------------------------------
class State {
public:
  struct TransitionFunction {
    std::string symbol{};
    std::shared_ptr<State> to_state;
  };

  int state;
  std::vector<TransitionFunction> transition_states;

  State(int s) : state{s} {}
};

class Graph {
public:
  std::shared_ptr<State> start;

  Graph(std::shared_ptr<State> s) : start{s} {};
};

int main() {
  NFA n{construct_machine_definition()};
  std::vector<std::string> test_strings = read_test_strings();

  std::vector<std::shared_ptr<State>> states{};
  for (int i = 0; i < n.states; ++i) {
    std::shared_ptr<State> state = std::make_shared<State>(i);
    states.push_back(state);
  }

  for (int i = 0; i < states.size(); ++i) {
    std::vector<State::TransitionFunction> state_transitions{};
    for (int j = 0; j < n.transitions.size(); ++j) {
      if (n.transitions[j].from == i) {
        state_transitions.push_back(State::TransitionFunction{
            n.transitions[j].symbol, states[n.transitions[j].to]});
      }
    }
    states[i]->transition_states = state_transitions;
  }

  Graph g{states[0]};

  return 0;
}

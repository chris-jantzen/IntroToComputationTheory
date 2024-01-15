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
#include <string>
#include <vector>

class Transition {
public:
  // (from, symbol) -> to
  std::string from;
  std::string to;
  std::string symbol;

  Transition(std::string f, std::string sym, std::string t)
      : from{f}, symbol{sym}, to{t} {}
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

  // TODO: see if there's just a native way to do this.
  int position{0};
  std::string delimiter{" "};
  while (position < transitionString.size()) {
    position = transitionString.find(delimiter);
    pieces.push_back(transitionString.substr(0, position));
    transitionString.erase(0, position + delimiter.size());
  }
  pieces.push_back(transitionString);

  Transition t{pieces[0], pieces[1], pieces[2]};
  return t;
}

NFA construct_machine_definition() {
  std::string n_in;
  getline(std::cin, n_in);

  // number of states (0 is the start state up to n-1)
  int n{stoi(n_in)};

  std::string t_in;
  getline(std::cin, t_in);
  int t{stoi(t_in)};

  std::vector<Transition> transitions{};
  for (int i = 0; i < t; ++i) {
    std::string transitionString;
    std::getline(std::cin, transitionString);
    transitions.push_back(split_transition_input(transitionString));
  }

  std::string f_in;
  getline(std::cin, f_in);
  int f{stoi(f_in)};

  /* std::vector<int> acceptStates(f); */
  std::vector<int> acceptStates{};

  for (int i = 0; i < f; ++i) {
    std::string acceptState;
    getline(std::cin, acceptState);
    acceptStates.push_back(stoi(acceptState));
  }

  NFA machine_n{n, transitions, acceptStates};

  machine_n.print_definition();

  return machine_n;
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

  for (int i = 0; i < test_strings.size(); ++i) {
    std::cout << test_strings[i] << '\n';
  }

  return test_strings;
}

int main() {
  NFA n{construct_machine_definition()};
  std::vector<std::string> test_strings = read_test_strings();
  return 0;
}

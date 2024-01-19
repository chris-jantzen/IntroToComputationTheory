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

bool test_run = false;

class Transition
{
public:
  // (from, symbol) -> to
  int from;
  int to;
  std::string symbol;

  Transition(int f, std::string sym, int t) : from{f}, symbol{sym}, to{t} {}
};

class NFA
{
public:
  int states{};
  std::vector<Transition> transitions;
  std::vector<int> acceptStates;
  NFA(int states, std::vector<Transition> transitions,
      std::vector<int> acceptStates)
      : states{states}, transitions{transitions}, acceptStates{acceptStates} {}

  void print_definition()
  {
    std::cout << "Machine definition" << '\n';
    std::cout << "Number of states: " << states << '\n';
    std::cout << "Alphabet: [a-z]" << '\n';
    std::cout << "Accept states: " << '\n';
    print_acceptStates();

    std::cout << "transitions: " << '\n';
    print_transitions();
    std::cout << "End Machine Definition" << std::endl;
  }

  void print_transitions()
  {
    for (int i = 0; i < transitions.size(); ++i)
    {
      std::cout << transitions[i].from << " " << transitions[i].symbol << " "
                << transitions[i].to << '\n';
    }
  }

  void print_acceptStates()
  {
    for (int i = 0; i < acceptStates.size(); ++i)
    {
      std::cout << acceptStates[i] << ' ';
    }
    std::cout << '\n';
  }

  bool isAcceptState(int state)
  {
    for (int i : acceptStates)
    {
      if (state == i)
        return true;
    }
    return false;
  }
};

Transition split_transition_input(std::string transitionString)
{
  std::vector<std::string> pieces{};

  int position{0};
  std::string delimiter{" "};
  while (position < transitionString.size())
  {
    position = transitionString.find(delimiter);
    pieces.push_back(transitionString.substr(0, position));
    transitionString.erase(0, position + delimiter.size());
  }
  if (transitionString != "")
    pieces.push_back(transitionString);

  Transition t{stoi(pieces[0]), pieces[1], stoi(pieces[2])};
  return t;
}

NFA construct_machine_definition()
{
  std::string n_in;
  if (test_run)
  {
    n_in = "4";
  }
  else
  {
    getline(std::cin, n_in);
  }

  // number of states (0 is the start state up to n-1)
  int n{stoi(n_in)};

  std::string t_in;
  if (test_run)
  {
    t_in = "4";
  }
  else
  {
    getline(std::cin, t_in);
  }
  int t{stoi(t_in)};

  std::vector<Transition> transitions{};
  if (test_run)
  {
    transitions.push_back(split_transition_input("0 eps 1"));
    transitions.push_back(split_transition_input("1 a 2"));
    transitions.push_back(split_transition_input("2 b 3"));
    transitions.push_back(split_transition_input("3 a 3"));
  }
  else
  {
    for (int i = 0; i < t; ++i)
    {
      std::string transitionString;
      std::getline(std::cin, transitionString);
      transitions.push_back(split_transition_input(transitionString));
    }
  }

  std::string f_in;
  if (test_run)
  {
    f_in = "2";
  }
  else
  {
    getline(std::cin, f_in);
  }
  int f{stoi(f_in)};

  std::vector<int> acceptStates{};

  if (test_run)
  {
    std::vector<std::string> inputAcceptStates{"0", "3"};
    for (std::string as : inputAcceptStates)
    {
      acceptStates.push_back(stoi(as));
    }
  }
  else
  {
    for (int i = 0; i < f; ++i)
    {
      std::string acceptState;
      getline(std::cin, acceptState);
      acceptStates.push_back(stoi(acceptState));
    }
  }

  return {n, transitions, acceptStates};
}

std::vector<std::string> read_test_strings()
{
  std::string s_in;
  if (test_run)
  {
    s_in = "6";
  }
  else
  {
    getline(std::cin, s_in);
  }
  int s{stoi(s_in)};

  std::vector<std::string> test_strings{};

  if (test_run)
  {
    test_strings.push_back("");
    test_strings.push_back("aba");
    test_strings.push_back("abaaaaa");
    test_strings.push_back("abab");
  }
  else
  {
    for (int i = 0; i < s; ++i)
    {
      std::string test_string;
      getline(std::cin, test_string);
      test_strings.push_back(test_string);
    }
  }

  return test_strings;
}

class State
{
public:
  struct TransitionFunction
  {
    std::string symbol{};
    std::shared_ptr<State> to_state;
  };

  bool accept{};
  int state;
  std::vector<TransitionFunction> transition_states;

  State(int s, bool a) : state{s}, accept{a} {}

  void print()
  {
    std::string acceptState{(accept ? "" : "not an")};
    std::cout << "I am at state: " << state << " and I am " << acceptState << " accept state." << std::endl;
  }
};

std::string asString(char x)
{
  std::string s{x};
  return s;
}

void log_valid_states(std::vector<std::shared_ptr<State>> &validStates, std::vector<std::shared_ptr<State>> &nextValidStates)
{
  std::cout << "ValidStates :" << '\n';
  for (std::shared_ptr<State> vs : validStates)
  {
    std::cout << vs->state << '\n';
  }
  std::cout << "Next round valid states :" << '\n';
  for (std::shared_ptr<State> vs : nextValidStates)
  {
    std::cout << vs->state << '\n';
  }
}

std::vector<std::shared_ptr<State>> buildStateObjects(NFA &n)
{
  std::vector<std::shared_ptr<State>> states{};
  for (int i = 0; i < n.states; ++i)
  {
    std::shared_ptr<State> state = std::make_shared<State>(i, n.isAcceptState(i));
    states.push_back(state);
  }
  return states;
}

void populateStateTransitions(NFA &n, std::vector<std::shared_ptr<State>> &states)
{
  for (int i = 0; i < states.size(); ++i)
  {
    std::vector<State::TransitionFunction> state_transitions{};
    for (int j = 0; j < n.transitions.size(); ++j)
    {
      if (n.transitions[j].from == i)
      {
        state_transitions.push_back(State::TransitionFunction{
            n.transitions[j].symbol, states[n.transitions[j].to]});
      }
    }
    states[i]->transition_states = state_transitions;
  }
}

int main()
{
  NFA n{construct_machine_definition()};
  std::vector<std::string> testStrings = read_test_strings();

  std::vector<std::shared_ptr<State>> states = buildStateObjects(n);

  populateStateTransitions(n, states);

  // std::string problemString = testStrings[testStrings.size() - 1];
  // testStrings.clear();
  // testStrings.push_back(problemString);

  // Iterate through the test strings
  for (int i = 0; i < testStrings.size(); ++i)
  {
    std::vector<std::shared_ptr<State>> validStates{states[0]};
    std::vector<std::shared_ptr<State>> nextValidStates{};
    std::vector<std::shared_ptr<State>> epsilonClosureNextValidStates{};

    if (testStrings[i] == "")
    {
      // Perform Epsilon Closure for the start state
      for (int k = 0; k < validStates[0]->transition_states.size(); ++k)
      {
        if (validStates[0]->transition_states[k].symbol == "eps")
        {
          validStates.push_back(states[0]->transition_states[k].to_state);
        }
      }
    }
    else
    {
      // Iterate through the characters of each string
      for (int j = 0; j < testStrings[i].length(); ++j)
      {
        // Iterate through the valid states
        for (int x = 0; x < validStates.size(); ++x)
        {
          // Perform Epsilon Closure for the start state
          if (j == 0)
          {
            for (int k = 0; k < validStates[x]->transition_states.size(); ++k)
            {
              if (validStates[x]->transition_states[k].symbol == "eps")
              {
                validStates.push_back(states[x]->transition_states[k].to_state);
              }
            }
          }

          // Iterate through the state's transition functions
          for (int y = 0; y < validStates[x]->transition_states.size(); ++y)
          {
            if (validStates[x]->transition_states[y].symbol == asString(testStrings[i][j]))
            {
              // Only add the state to the list of valid states if it isn't already there
              if (nextValidStates.size() > 0)
              {
                bool stateAlreadyInNextValidStates = false;
                for (int z = 0; z < nextValidStates.size(); ++z)
                {
                  if (nextValidStates[z]->state == validStates[x]->transition_states[y].to_state->state)
                  {
                    stateAlreadyInNextValidStates = true;
                  }
                }
                if (!stateAlreadyInNextValidStates)
                {
                  nextValidStates.push_back(validStates[x]->transition_states[y].to_state);
                }
              }
              else
              {
                nextValidStates.push_back(validStates[x]->transition_states[y].to_state);
              }
            }
          }
        }
        for (auto x : nextValidStates)
        {
          epsilonClosureNextValidStates.push_back(x);
        }
        for (int x = 0; x < nextValidStates.size(); ++x)
        {
          for (int k = 0; k < nextValidStates[x]->transition_states.size(); ++k)
          {
            if (nextValidStates[x]->transition_states[k].symbol == "eps")
            {
              bool stateAlreadyInNextValidStates = false;
              for (int eps = 0; eps < epsilonClosureNextValidStates.size(); ++eps)
              {
                if (epsilonClosureNextValidStates[eps]->state == nextValidStates[x]->transition_states[k].to_state->state)
                {
                  stateAlreadyInNextValidStates = true;
                  break;
                }
              }
              if (!stateAlreadyInNextValidStates)
              {
                epsilonClosureNextValidStates.push_back(nextValidStates[x]->transition_states[k].to_state);
              }
            }
          }
        }

        // Prepare valid states set for the next symbol read
        validStates.clear();
        if (epsilonClosureNextValidStates.size())
        {
          for (std::shared_ptr<State> s : epsilonClosureNextValidStates)
          {
            validStates.push_back(s);
          }
          nextValidStates.clear();
          epsilonClosureNextValidStates.clear();
        }
      }
    }

    if (validStates.size())
    {
      bool shouldAccept = false;
      for (auto state : validStates)
      {
        if (state->accept == true)
        {
          shouldAccept = true;
          std::cout << "accept" << '\n';
          break;
        }
      }
      if (!shouldAccept)
      {
        std::cout << "reject" << '\n';
      }
    }
    else
    {
      std::cout << "reject" << '\n';
    }
  }

  /**
   * Read in machine construction information
   * Check for epsilon transitions from the start state
   *   add any epsilon transition "to states" to the set of current states
   * Read the first symbol of the string
   *   for each current state, look for any transition functions for that symbol
   *   add the "to state" to the set of current states
   * Continue until all symbols of the string have been read
   * Accept if any of the current states are final states
   */

  /* std::cout << states[0]->state << std::endl; */
  /* std::shared_ptr<State> traverser = states[0]; */
  // Graph g{states[0]};

  // std::shared_ptr<State> traverser = g.start;

  // // aba
  // std::string ts1 = test_strings[0];

  // for (int i = 0; i < ts1.length(); ++i)
  // {
  //   for (int j = 0; j < traverser->transition_states.size(); ++j)
  //   {
  //     if (traverser->transition_states[j].symbol.compare(asString(ts1[i])))
  //     {
  //       traverser = traverser->transition_states[j].to_state;
  //     }
  //     else if (traverser->transition_states[j].symbol.compare("eps"))
  //     {
  //       traverser = traverser->transition_states[j].to_state;
  //     }
  //   }
  // }

  // for (int i = 0; i < traverser->transition_states.size(); ++i)
  // {
  //   std::cout << "I am at state: " << traverser->state
  //             << " and about to read symbol "
  //             << traverser->transition_states[i].symbol << " to go to state: "
  //             << traverser->transition_states[i].to_state->state << std::endl;
  //   std::shared_ptr<State> t = traverser->transition_states[i].to_state;
  // }

  /* std::cout << traverser->state << std::endl; */
  /* std::cout << traverser->transition_states[2].symbol << std::endl; */
  /* traverser = traverser->transition_states[2].to_state; */
  /* std::cout << traverser->state << std::endl; */

  return 0;
}

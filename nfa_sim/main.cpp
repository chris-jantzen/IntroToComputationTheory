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
    n_in = "5";
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
    transitions.push_back(split_transition_input("0 a 1"));
    transitions.push_back(split_transition_input("1 eps 2"));
    transitions.push_back(split_transition_input("2 eps 3"));
    transitions.push_back(split_transition_input("3 eps 4"));
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
    f_in = "1";
  }
  else
  {
    getline(std::cin, f_in);
  }
  int f{stoi(f_in)};

  std::vector<int> acceptStates{};

  if (test_run)
  {
    std::vector<std::string> inputAcceptStates{"4"};
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
    s_in = "3";
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
    test_strings.push_back("a");
    test_strings.push_back("aa");
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

void performEpsilonClosure(std::vector<std::shared_ptr<State>> &validStates, std::vector<std::shared_ptr<State>> &states)
{
  bool shouldContinue{false};
  int k = 0;
  do
  {
    shouldContinue = false;
    for (int x = 0; x < validStates[k]->transition_states.size(); ++x)
    {
      if (validStates[k]->transition_states[x].symbol == "eps")
      {
        bool alreadyThere{false};
        for (auto vs : validStates)
        {
          if (vs->state == states[k]->transition_states[x].to_state->state)
          {
            alreadyThere = true;
            break;
          }
        }
        if (!alreadyThere)
        {
          validStates.push_back(states[k]->transition_states[x].to_state);
          shouldContinue = true;
        }
      }
    }
    ++k;
  } while (shouldContinue);
}

std::vector<std::shared_ptr<State>> performStateEpsilonExpansion(const std::shared_ptr<State> state, const std::vector<std::shared_ptr<State>> &states)
{
  std::vector<std::shared_ptr<State>> result{state};
  int i{0};
  bool shouldContinue{false};
  do
  {
    shouldContinue = false;
    for (State::TransitionFunction t : result[i]->transition_states)
    {
      if (t.symbol == "eps")
      {
        bool alreadyThere{false};
        for (std::shared_ptr<State> r : result)
        {
          if (r->state == t.to_state->state)
          {
            alreadyThere = true;
            break;
          }
        }
        if (!alreadyThere)
        {
          result.push_back(t.to_state);
          shouldContinue = true;
        }
      }
    }
    ++i;
  } while (shouldContinue);
  return result;
}

std::vector<std::shared_ptr<State>> appendUniqueStates(const std::vector<std::shared_ptr<State>> &nextValidStates, const std::vector<std::shared_ptr<State>> &epsilonExpandedNextState)
{
  std::vector<std::shared_ptr<State>> result{};
  for (auto nv : nextValidStates)
    result.push_back(nv);

  for (auto e : epsilonExpandedNextState)
  {
    bool alreadyThere{false};
    for (auto nv : nextValidStates)
    {
      if (nv->state == e->state)
      {
        alreadyThere = true;
        break;
      }
    }
    if (!alreadyThere)
      result.push_back(e);
  }
  return result;
}

int main()
{
  NFA n{construct_machine_definition()};
  std::vector<std::string> testStrings = read_test_strings();

  std::vector<std::shared_ptr<State>> states = buildStateObjects(n);

  populateStateTransitions(n, states);

  // Iterate through the test strings
  for (int i = 0; i < testStrings.size(); ++i)
  {
    std::vector<std::shared_ptr<State>> validStates{states[0]};
    std::vector<std::shared_ptr<State>> nextValidStates{};
    // std::vector<std::shared_ptr<State>> epsilonClosureNextValidStates{};

    if (testStrings[i] == "")
    {
      // Perform Epsilon Closure for the start state
      performEpsilonClosure(validStates, states);
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
            performStateEpsilonExpansion(states[x], states);
          }

          // Iterate through the state's transition functions
          for (int y = 0; y < validStates[x]->transition_states.size(); ++y)
          {
            if (validStates[x]->transition_states[y].symbol == asString(testStrings[i][j]))
            {
              // Only add the state to the list of valid states if it isn't already there
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
                auto epsilonExpandedNextState{performStateEpsilonExpansion(validStates[x]->transition_states[y].to_state, states)};
                // nextValidStates.push_back(validStates[x]->transition_states[y].to_state);
                nextValidStates = appendUniqueStates(nextValidStates, epsilonExpandedNextState);
              }
            }
          }
        }

        // Prepare valid states set for the next symbol read
        validStates.clear();
        if (nextValidStates.size() > 0)
        {
          for (std::shared_ptr<State> s : nextValidStates)
          {
            validStates.push_back(s);
          }
          nextValidStates.clear();
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

  return 0;
}

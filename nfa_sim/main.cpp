#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>

#define EPSILON "eps"

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
  std::vector<Transition> transitions;
  std::vector<int> acceptStates;
  NFA(int states, std::vector<Transition> transitions, std::vector<int> acceptStates)
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
  std::vector<TransitionFunction> transition_functions{};
  std::set<int> epsilonTransitions{};

  State(int s, bool a) : state{s}, accept{a} {}

  friend bool operator<(const std::shared_ptr<State> &left, const std::shared_ptr<State> &right);
};

bool operator<(const std::shared_ptr<State> &left, const std::shared_ptr<State> &right)
{
  return left->state < right->state;
}

NFA::Transition split_transition_input(std::string transitionString)
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

  return {stoi(pieces[0]), pieces[1], stoi(pieces[2])};
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

  std::string f_in;
  getline(std::cin, f_in);
  int f{stoi(f_in)};

  std::vector<int> acceptStates{};

  for (int i = 0; i < f; ++i)
  {
    std::string acceptState;
    getline(std::cin, acceptState);
    acceptStates.push_back(stoi(acceptState));
  }

  return {n, transitions, acceptStates};
}

std::vector<std::string> read_test_strings()
{
  std::string s_in;
  getline(std::cin, s_in);
  int s{stoi(s_in)};

  std::vector<std::string> test_strings{};

  for (int i = 0; i < s; ++i)
  {
    std::string test_string;
    getline(std::cin, test_string);
    test_strings.push_back(test_string);
  }

  return test_strings;
}

std::vector<std::shared_ptr<State>> buildStateObjects(NFA &n)
{
  std::vector<std::shared_ptr<State>> states{};
  for (int i = 0; i < n.states; ++i)
  {
    std::shared_ptr<State> state = std::make_shared<State>(i, n.isAcceptState(i));
    states.push_back(state);
  }

  for (auto t : n.transitions)
  {
    states[t.from]->transition_functions.push_back(State::TransitionFunction{t.symbol, states[t.to]});
    if (t.symbol == EPSILON)
    {
      states[t.from]->epsilonTransitions.insert(t.to);
    }
  }
  return states;
}

std::string asString(char x)
{
  std::string s{x};
  return s;
}

std::set<std::shared_ptr<State>> performStateEpsilonExpansion(
    const std::shared_ptr<State> &state,
    std::set<std::shared_ptr<State>> validNextStates,
    const std::vector<std::shared_ptr<State>> &states)
{
  std::vector<std::shared_ptr<State>> result{state};
  validNextStates.insert(state);
  bool shouldContinue{false};
  int i{0};
  do
  {
    shouldContinue = false;
    for (auto t : result[i]->epsilonTransitions)
    {
      auto out = validNextStates.insert(states[t]);
      if (out.second)
      {
        result.push_back(states[t]);
        shouldContinue = true;
      }
    }
    ++i;
    if (i > states.size())
      break;
  } while (shouldContinue);
  return validNextStates;
}

void produceOutput(const std::set<std::shared_ptr<State>> &validStates)
{
  for (std::shared_ptr<State> state : validStates)
  {
    if (state->accept == true)
    {
      std::cout << "accept" << std::endl;
      return;
    }
  }
  std::cout << "reject" << std::endl;
}

NFA test_run()
{
  int n{7};
  int t{7};
  std::vector<NFA::Transition> transitions{};
  std::vector<std::string> transitionStrings{"0 eps 1", "1 eps 2", "2 eps 3", "3 eps 4", "4 eps 5", "5 eps 0", "5 a 6"};
  for (auto transition : transitionStrings)
  {
    transitions.push_back(split_transition_input(transition));
  }

  int f{1};
  std::vector<int> acceptStates{6};

  return {n, transitions, acceptStates};
}

std::vector<std::string> test_run_strings() { return {"", "a", "aa"}; }

int main()
{
  NFA n{read_nfa_definition_input()};
  std::vector<std::string> testStrings{read_test_strings()};
  // NFA n{test_run()};
  // std::vector<std::string> testStrings = test_run_strings();

  std::vector<std::shared_ptr<State>> states = buildStateObjects(n);

  for (std::string testString : testStrings)
  {
    std::set<std::shared_ptr<State>> validStates{states[0]};
    std::set<std::shared_ptr<State>> nextValidStates{};

    if (testString == "")
    {
      validStates = performStateEpsilonExpansion(states[0], nextValidStates, states);
    }
    else
    {
      for (int j = 0; j < testString.length(); ++j)
      {
        if (j == 0)
        {
          validStates = performStateEpsilonExpansion(states[0], nextValidStates, states);
        }
        for (std::shared_ptr<State> state : validStates)
        {
          for (State::TransitionFunction transition : state->transition_functions)
          {
            if (transition.symbol == asString(testString[j]))
            {
              nextValidStates = performStateEpsilonExpansion(transition.to_state, nextValidStates, states);
            }
          }
        }

        validStates.clear();
        if (nextValidStates.size() > 0)
        {
          validStates = nextValidStates;
          nextValidStates.clear();
        }
      }
    }

    produceOutput(validStates);
  }

  return 0;
}
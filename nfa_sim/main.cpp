#include <iostream>
#include <memory>
#include <string>
#include <vector>

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
  std::vector<TransitionFunction> transition_states;

  State(int s, bool a) : state{s}, accept{a} {}
};

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

NFA test_run()
{
  int n{5};
  int t{4};
  std::vector<NFA::Transition> transitions{};
  std::vector<std::string> transitionStrings{"0 eps 1", "1 eps 2", "2 eps 3", "3 eps 4"};
  for (auto transition : transitionStrings)
  {
    transitions.push_back(split_transition_input(transition));
  }

  int f{1};
  std::vector<int> acceptStates{4};

  return {n, transitions, acceptStates};
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
    std::shared_ptr<State> state =
        std::make_shared<State>(i, n.isAcceptState(i));
    states.push_back(state);
  }
  return states;
}

void populateStateTransitions(const NFA &n, std::vector<std::shared_ptr<State>> &states)
{
  for (int i = 0; i < states.size(); ++i)
  {
    std::vector<State::TransitionFunction> state_transitions{};
    for (int j = 0; j < n.transitions.size(); ++j)
    {
      if (n.transitions[j].from == i)
      {
        state_transitions.push_back(State::TransitionFunction{n.transitions[j].symbol, states[n.transitions[j].to]});
      }
    }
    states[i]->transition_states = state_transitions;
  }
}

std::vector<std::shared_ptr<State>> performStateEpsilonExpansion(
    const std::shared_ptr<State> state,
    const std::vector<std::shared_ptr<State>> &states)
{
  std::vector<std::shared_ptr<State>> result{state};
  int i{0};
  bool shouldContinue{false};
  do
  {
    shouldContinue = false;
    for (State::TransitionFunction t : result[i]->transition_states)
    {
      if (t.symbol == EPSILON)
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

std::vector<std::shared_ptr<State>> appendUniqueStates(
    const std::vector<std::shared_ptr<State>> &nextValidStates,
    const std::vector<std::shared_ptr<State>> &epsilonExpandedNextState)
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

std::vector<std::string> test_run_strings()
{
  return {"", "a", "aa", "abbccc", "xyz"};
}

int main()
{
  NFA n{read_nfa_definition_input()};
  std::vector<std::string> testStrings{read_test_strings()};
  // NFA n{test_run()};
  // std::vector<std::string> testStrings = test_run_strings();

  std::vector<std::shared_ptr<State>> states = buildStateObjects(n);

  populateStateTransitions(n, states);

  for (int i = 0; i < testStrings.size(); ++i)
  {
    std::vector<std::shared_ptr<State>> validStates{states[0]};
    std::vector<std::shared_ptr<State>> nextValidStates{};

    if (testStrings[i] == "")
    {
      validStates = performStateEpsilonExpansion(states[0], states);
    }
    else
    {
      for (int j = 0; j < testStrings[i].length(); ++j)
      {
        if (j == 0)
        {
          validStates = performStateEpsilonExpansion(states[0], states);
        }
        for (int x = 0; x < validStates.size(); ++x)
        {
          for (auto transition : validStates[x]->transition_states)
          {
            if (transition.symbol == asString(testStrings[i][j]))
            {
              bool stateAlreadyInNextValidStates = false;
              for (std::shared_ptr<State> nvs : nextValidStates)
              {
                if (nvs->state == transition.to_state->state)
                {
                  stateAlreadyInNextValidStates = true;
                }
              }
              if (!stateAlreadyInNextValidStates)
              {
                std::vector<std::shared_ptr<State>> epsilonExpandedNextState{performStateEpsilonExpansion(transition.to_state, states)};
                nextValidStates = appendUniqueStates(nextValidStates, epsilonExpandedNextState);
              }
            }
          }
        }

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
      for (std::shared_ptr<State> state : validStates)
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

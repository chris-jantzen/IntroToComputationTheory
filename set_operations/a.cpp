#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

std::vector<std::string> readInSetItems()
{
  std::string count;
  std::string str;
  std::cin >> count;

  const int c = std::stoi(count);
  std::vector<std::string> set(c);

  for (int i = 0; i < c; ++i)
  {
    std::cin >> str;
    set[i] = str;
  }
  return set;
}

void construct_cartesian_product(std::vector<std::string> &set1, std::vector<std::string> &set2)
{
  std::vector<std::array<std::string, 2>> cartesian_product_set{};

  for (int i = 0; i < set1.size(); ++i)
  {
    for (int j = 0; j < set2.size(); ++j)
    {
      const std::array<std::string, 2> product{{set1[i], set2[j]}};
      cartesian_product_set.push_back(product);
    }
  }

  for (int i = 0; i < cartesian_product_set.size(); ++i)
  {
    std::cout << cartesian_product_set[i][0] << " " << cartesian_product_set[i][1] << '\n';
  }
  std::cout << '\n';
}

void construct_intersection(std::vector<std::string> &set1, std::vector<std::string> &set2)
{
  std::vector<std::string> intersection_set{};

  for (int i = 0; i < set1.size(); i++)
  {
    for (int j = 0; j < set2.size(); j++)
    {
      if (set1[i] == set2[j] && std::find(intersection_set.begin(), intersection_set.end(), set1[i]) == intersection_set.end())
      {
        intersection_set.push_back(set1[i]);
      }
    }
  }

  std::sort(intersection_set.begin(), intersection_set.end());

  for (int i = 0; i < intersection_set.size(); ++i)
  {
    std::cout << intersection_set[i] << '\n';
  }

  std::cout << '\n';
}

void construct_union(std::vector<std::string> &set1, std::vector<std::string> &set2)
{
  std::vector<std::string> union_set{};
  for (int i = 0; i < set1.size(); ++i)
  {
    if (std::find(union_set.begin(), union_set.end(), set1[i]) == union_set.end())
    {
      union_set.push_back(set1[i]);
    }
  }
  for (int j = 0; j < set2.size(); ++j)
  {
    if (std::find(union_set.begin(), union_set.end(), set2[j]) == union_set.end())
    {
      union_set.push_back(set2[j]);
    }
  }

  std::sort(union_set.begin(), union_set.end());

  for (int i = 0; i < union_set.size(); ++i)
  {
    std::cout << union_set[i] << '\n';
  }

  std::cout << '\n';
}

int main()
{
  std::vector<std::string> set1 = readInSetItems();
  std::vector<std::string> set2 = readInSetItems();

  std::sort(set1.begin(), set1.end());
  std::sort(set2.begin(), set2.end());

  construct_union(set1, set2);
  construct_intersection(set1, set2);
  construct_cartesian_product(set1, set2);

  return 0;
}

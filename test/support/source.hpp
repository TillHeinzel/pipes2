#pragma once

#include <string>
#include <vector>
#include <memory>

template<class T = int>
struct source : public std::vector<T>
{
  using std::vector<T>::vector;

  template<class... Ts>
  source(Ts&&... ts) : std::vector<T>{PIPES_FWD(ts)...}
  {
  }
};

template<class... Ts>
source(int, Ts...) -> source<int>;

template<class... Ts>
source(char, Ts...) -> source<char>;

template<class... Ts>
source(const char*, Ts...) -> source<std::string>;

template<class... Ts>
source(double, Ts...) -> source<double>;

template<class... T1s, class... Ts>
source(std::tuple<T1s...>, Ts...) -> source<std::tuple<T1s...>>;

template<class T1, class T2, class... Ts>
source(std::pair<T1, T2>, Ts...) -> source<std::pair<T1, T2>>;

//template<class T = int>
//using vals = source<T>;

#define vals source

auto unique_source(auto... ts)
{
  auto retval = std::vector<std::unique_ptr<int>>{};

  (retval.push_back(std::make_unique<int>(ts)), ...);

  return retval;
}

inline bool operator==(std::vector<std::unique_ptr<int>> const& lhs,
                       std::vector<int> const& rhs)
{
  if(lhs.size() != rhs.size()) return false;

  for(auto i = 0; i < lhs.size(); ++i)
  {
    if((*lhs.at(i)) != rhs.at(i)) return false;
  }
  return true;
}

auto unique(auto f) 
{
  return [f](std::unique_ptr<int> const& ptr) { return f(*ptr); };
}

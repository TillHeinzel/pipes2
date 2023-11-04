#pragma once

#include <string>
#include <vector>

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
source(const char*, Ts...) -> source<std::string>;

template<class... Ts>
source(double, Ts...) -> source<double>;

template<class... T1s, class... Ts>
source(std::tuple<T1s...>, Ts...) -> source<std::tuple<T1s...>>;

template<class T1, class T2, class... Ts>
source(std::pair<T1, T2>, Ts...) -> source<std::pair<T1, T2>>;

template<class T = int>
using vals = source<T>;

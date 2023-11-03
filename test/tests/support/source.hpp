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

template<class T = int>
using vals = source<T>;
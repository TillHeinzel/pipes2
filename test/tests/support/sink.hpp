#pragma once

#include <string>
#include <variant>
#include <vector>

using sinkVal_t = std::variant<int, std::string>;
using sink = std::vector<sinkVal_t>;

template<class T>
bool operator==(sink const& lhs, std::vector<T> const& rhs)
{
  auto vv = std::vector<T>{};

  for(auto const& x : lhs)
  {
    vv.push_back(std::get<T>(x));
  }

  return vv == rhs;
}

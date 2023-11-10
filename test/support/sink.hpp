#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

using sinkVal_t =
  std::variant<char,
               int,
               double,
               std::string,
               std::tuple<int>,
               std::tuple<int, int>,
               std::tuple<int, int, int>,
               std::tuple<int, std::string>,
               std::tuple<std::string, std::string>,
               std::tuple<int, std::string, int>,
               std::tuple<std::pair<int, std::string>, int>,
               std::tuple<std::tuple<int, int>, std::tuple<int, int>>,
               std::pair<int, int>>;

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


auto unique_sink(auto... ts)
{
  auto retval = std::vector<std::unique_ptr<int>>{};

  (retval.push_back(std::make_unique<int>(ts)), ...);

  return retval;
}

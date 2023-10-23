#pragma once

#include <vector>

#include "impl.hpp"

namespace pipes
{
  struct ForEachSource
  {
    static constexpr bool isRootSource = true;

    std::vector<int> const& v;

    void push(Sink<int> auto sink)
    {
      for(const int i : v) { sink.push(i); }
    }
  };

  auto forEach(const std::vector<int>& v)
  {
    return Source{ForEachSource{v}, {}};
  }
} // namespace pipes
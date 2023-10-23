#pragma once

#include <vector>

#include "impl.hpp"

namespace pipes
{
  template<class T>
  struct ForEachSource
  {
    static constexpr bool isRootSource = true;

    std::vector<T> const& v;

    void push(Sink<T> auto sink)
    {
      for(T const& t : v) { sink.push(t); }
    }
  };

  template<class T>
  auto forEach(const std::vector<T>& v)
  {
    return Source{ForEachSource{v}, {}};
  }
} // namespace pipes
#pragma once

#include <vector>

#include "impl.hpp"

namespace pipes::detail
{
  template<class T>
  struct ForEachSource
  {
    using OutputType = T;

    std::vector<T> const& v;

    void push(SinkFor<T> auto sink)
    {
      for(T const& t : v) { sink.push(t); }
    }
  };

  namespace api
  {
    template<class T>
    auto forEach(const std::vector<T>& v)
    {
      return Source<ForEachSource<T>>{ForEachSource<T>{v}, {}};
    }
  } // namespace api
} // namespace pipes::detail
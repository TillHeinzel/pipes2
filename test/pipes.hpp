#pragma once

#include <concepts>
#include <vector>

#include "PushBack.hpp"
#include "Filter.hpp"
#include "Transform.hpp"

namespace pipes
{
  template<class F>
  TransformSink<F> operator>>=(TransformNode<F> node, std::vector<int>& v)
  {
    return {node, v};
  }


  template<class F>
  FilterSink<F> operator>>=(FilterNode<F> node, std::vector<int>& v)
  {
    return {node, v};
  }

  template<typename T>
  concept Sink = requires(T t, int i) { t.push(i); };

  void operator>>=(std::vector<int> const& source, Sink auto sink)
  {
    for(const int i : source) { sink.push(i); }
  }
} // namespace pipes

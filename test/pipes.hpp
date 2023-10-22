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
  void operator>>=(std::vector<int> const& source, TransformSink<F> sink)
  {
    for(const int i : source) { sink.push(i); }
  }



  template<class F>
  FilterSink<F> operator>>=(FilterNode<F> node, std::vector<int>& v)
  {
    return {node, v};
  }

  template<class F>
  void operator>>=(const std::vector<int>& source, FilterSink<F> sink)
  {
    for(const int i : source) { sink.push(i); }
  }

} // namespace pipes

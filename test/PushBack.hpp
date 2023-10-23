#pragma once

#include <vector>

#include "impl.hpp"

namespace pipes
{
  template<class T>
  struct PushBackSink
  {
    static constexpr bool isOpenSink = true;

    std::vector<T>& v;

    void push(T t) { v.push_back(t); }
  };

  template<class T>
  PushBackSink<T> push_back(std::vector<T>& v) { return {v}; }
} // namespace pipes

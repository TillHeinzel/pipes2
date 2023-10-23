#pragma once

#include <vector>

#include "impl.hpp"

namespace pipes
{
  template<class T>
  struct PushBackSink
  {
    std::vector<T>& v;

    void push(T t) { v.push_back(t); }
  };

  template<class T>
  auto push_back(std::vector<T>& v) { 
    return PSink{PushBackSink<T>{v}, {}}; 
  }
} // namespace pipes

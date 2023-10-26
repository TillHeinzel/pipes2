#pragma once
#include <vector>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class T>
  struct PushBackSink
  {
    std::vector<T>& v;

    void push(T t) { v.push_back(t); }
  };
} // namespace pipes::detail

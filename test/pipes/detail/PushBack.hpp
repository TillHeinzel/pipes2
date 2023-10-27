#pragma once
#include <vector>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  //template<class S, class T>
  //concept PushBackAble = requires(S s, T t) { s.push_back(t); };

  template<class T>
  struct PushBackSink
  {
    std::vector<T>& v;

    void push(T t) { v.push_back(t); }
  };
} // namespace pipes::detail

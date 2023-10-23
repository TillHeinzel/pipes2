#pragma once

#include <vector>

#include "impl.hpp"

namespace pipes::detail
{
  template<class T>
  struct PushBackSink
  {
    std::vector<T>& v;

    void push(T t) { v.push_back(t); }
  };

  namespace api
  {
    template<class T>
    auto push_back(std::vector<T>& v)
    {
      return Sink{PushBackSink<T>{v}, {}};
    }
  } // namespace api
} // namespace pipes::detail

#pragma once

#include <vector>

#include "../GenericImplementation/impl.hpp"

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
    auto push_back(std::vector<T>& v) PIPES_RETURN(Sink{PushBackSink{v}});
  } // namespace api
} // namespace pipes::detail

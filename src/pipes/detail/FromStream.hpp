#pragma once

#include <iostream>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class T>
  struct FromStream
  {
    std::istream& stream;

    void push(auto& sink)
    {
      T x;
      while(stream >> x)
      {
        sink.push(x);
      }
    }
  };
} // namespace pipes::detail

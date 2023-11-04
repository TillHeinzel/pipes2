#pragma once

#include <iostream>

#include "Utility/FWD.hpp"
#include "Utility/functional.hpp"
#include "Utility/tuples.hpp"

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

namespace pipes::detail
{
  struct ToStream
  {
    std::ostream& stream;

    auto push(auto&& x) PIPES_RETURN(stream << x);

    auto& value() { return stream; }
  };
} // namespace pipes::detail

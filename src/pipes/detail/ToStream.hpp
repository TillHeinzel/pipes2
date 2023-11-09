#pragma once

#include <iostream>

#include "Utility/HOF.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  struct ToStream
  {
    std::ostream& stream;

    auto push(auto&& x) PIPES_RETURN(stream << x);

    auto& value() { return stream; }
  };
} // namespace pipes::detail

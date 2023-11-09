#pragma once

#include <ranges>
#include <concepts>
#include <iterator>

#include "Utility/HOF.hpp"


#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class It>
  struct IteratorSink
  {
    It it;

    template<class T>
      requires(std::output_iterator<It, T>)
    auto push(T&& t) PIPES_RETURN(*(it++) = PIPES_FWD(t));

    It value() { return it; }
  };
} // namespace pipes::detail

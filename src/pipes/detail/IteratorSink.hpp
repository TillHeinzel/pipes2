#pragma once

#include <concepts>
#include <iterator>
#include <ranges>

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
    void push(T&& t)
    {
      *(it++) = PIPES_FWD(t);
    }

    It value() { return it; }
  };
} // namespace pipes::detail

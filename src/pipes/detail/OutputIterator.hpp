#pragma once

#include <ranges>
#include <tuple>

#include "Utility/FWD.hpp"
#include "Utility/functional.hpp"
#include "Utility/tuples.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class... Ts>
  struct OutputIteratorSection
  {
    Section<Ts...> pipe;
  };

  template<class Sink>
  struct OutputIterator
  {
    using iterator_category = std::output_iterator_tag;

    Sink sink;

    OutputIterator& operator=(int t)
    {
      sink.push(t);
      return *this;
    }

    OutputIterator& operator*() { return *this; }

    OutputIterator& operator++() { return *this; }
  };
} // namespace pipes::detail

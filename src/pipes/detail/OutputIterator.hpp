#pragma once

#include <iterator>

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

    OutputIterator& operator=(auto t)
    {
      sink.push(std::move(t));
      return *this;
    }

    OutputIterator& operator*() { return *this; }

    OutputIterator& operator++() { return *this; }
  };
} // namespace pipes::detail

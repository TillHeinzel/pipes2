#pragma once

#include <tuple>
#include <vector>

#include "../GenericImplementation/impl.hpp"
#include "../Utility/reverse.hpp"

#include "helpers.hpp"

namespace pipes::detail
{
  template<class... Nexts>
  struct Fork
  {
    std::tuple<Nexts...> nexts;

    template<class T>
      requires(SinkFor<Nexts, T>&&...)
    void push(T const& t)
    {
      for_each([&t](auto& n) { n.push(t); }, nexts);
    }
  };

  namespace api
  {
    template<class... S>
      requires(SinkSectionable<S>&&...)
    auto fork(S&&... s)
    {
      return sink(Fork{std::tuple{connect(asSinkSection(PIPES_FWD(s)))...}});
    }

  } // namespace api
} // namespace pipes::detail

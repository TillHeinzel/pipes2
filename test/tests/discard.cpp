#include "doctest.h"

#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/test_streaming.hpp"

TEST_CASE("sinks")
{
  SUBCASE("discard")
  {
    SUBCASE("generic")
    {
      auto const source = std::vector<int>{1, 2, 3, 4};

      static_assert(
        pipes::CanLink<decltype(source), decltype(pipes::discard())>);
    }

    SUBCASE("typed")
    {
      using SourceSection = std::vector<int>;

      static_assert(
        pipes::CanLink<SourceSection, decltype(pipes::discard<int>())>);

      static_assert(!pipes::CanLink<SourceSection,
                                    decltype(pipes::discard<std::string>())>);
    }

    SUBCASE("typed with tuple")
    {
      using SourceSection = std::vector<std::tuple<int, int>>;

      using Sink1 = decltype(pipes::discard<std::tuple<int, int>>());
      static_assert(pipes::CanLink<SourceSection, Sink1>);

      using Sink2 = decltype(pipes::discard<int, int>());
      static_assert(pipes::CanLink<SourceSection, Sink2>);
    }

    SUBCASE("typed with tuple of tuple (only unpack one level at a time!)")
    {
      using SourceSection = std::vector<std::tuple<std::tuple<int>>>;

      using Sink1 = decltype(pipes::discard<std::tuple<std::tuple<int>>>());
      static_assert(pipes::CanLink<SourceSection, Sink1>);

      using Sink2 = decltype(pipes::discard<std::tuple<int>>());
      static_assert(pipes::CanLink<SourceSection, Sink2>);

      using Sink3 = decltype(pipes::discard<int>());
      static_assert(!pipes::CanLink<SourceSection, Sink3>);
    }

    // todo: typed with types that can be converted to?
  }
}

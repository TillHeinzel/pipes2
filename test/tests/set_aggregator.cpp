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

#include "test_streaming.hpp"

TEST_CASE("sinks")
{
  SUBCASE("set_aggregator")
  {
    SUBCASE("")
    {
      auto const source = std::vector<int>{1, 2};
      auto sink = std::set<int>{};

      pipes::for_each(source) >>= pipes::set_aggregator(sink, std::plus{});

      CHECK(sink == std::set<int>{1, 2});
    }

    SUBCASE("")
    {
      auto const source = std::vector<int>{1, 3};
      auto sink = std::set<int>{1};

      pipes::for_each(source) >>= pipes::set_aggregator(sink, std::plus{});

      CHECK(sink == std::set<int>{1 + 1, 3});
    }

    SUBCASE("")
    {
      auto const source = std::vector<int>{1};
      auto sink = std::set<int>{1, 2};

      pipes::for_each(source) >>= pipes::set_aggregator(sink, std::plus{});

      CHECK(sink == std::set<int>{1 + 1 + 2});
    }
  }
}

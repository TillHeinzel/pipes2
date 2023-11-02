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
  SUBCASE("map_aggregator")
  {
    SUBCASE("")
    {
      auto const source = std::vector<std::pair<int, int>>{
        {1, 1},
        {2, 2}
      };
      auto sink = std::map<int, int>{};

      pipes::for_each(source) >>= pipes::map_aggregator(sink, std::plus{});

      CHECK(sink
            == std::map<int, int>{
              {1, 1},
              {2, 2}
      });
    }

    SUBCASE("")
    {
      auto const source = std::vector<std::pair<int, int>>{
        {1, 1},
        {2, 5},
        {3, 3},
        {3, 4}
      };
      auto sink = std::map<int, int>{
        {2, 2}
      };

      pipes::for_each(source) >>= pipes::map_aggregator(sink, std::plus{});

      CHECK(sink
            == std::map<int, int>{
              {1,     1},
              {2, 2 + 5},
              {3, 3 + 4}
      });
    }
  }
}

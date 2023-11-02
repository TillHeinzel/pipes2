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
  SUBCASE("sink defaults")
  {
    SUBCASE("vector default is push_back")
    {
      auto const source = std::vector{1, 2, 3};
      auto sink = std::vector<int>{};

      pipes::for_each(source) >>= sink;
      CHECK(sink == source);
    }

    SUBCASE("map default is insert_or_assign")
    {
      auto source = std::vector<std::pair<int, std::string>>{
        {1, "1"},
        {2, "2"},
        {3, "3"},
        {3, "4"}
      };
      auto sink = std::map<int, std::string>{};

      pipes::for_each(source) >>= sink;

      // uses insert_or_assign, so the last element pushed in for a
      // particular key is pushed
      CHECK(sink
            == std::map<int, std::string>{
              {1, "1"},
              {2, "2"},
              {3, "4"}
      });
    }
  }
}

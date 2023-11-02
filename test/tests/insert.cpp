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
  SUBCASE("insert")
  {
    SUBCASE("set")
    {
      auto source = std::vector{1, 2, 3, 3};
      auto sink = std::set<int>{};

      pipes::for_each(source) >>= pipes::insert(sink);

      CHECK(sink == std::set<int>{1, 2, 3});
    }

    SUBCASE("temporary")
    {
      auto source = std::vector{1, 2, 3, 3};
      auto sink = pipes::for_each(source) >>= pipes::insert(std::set<int>{});

      CHECK(sink == std::set<int>{1, 2, 3});
    }

    SUBCASE("map")
    {
      auto source = std::vector<std::pair<int, std::string>>{
        {1, "1"},
        {2, "2"},
        {3, "3"},
        {3, "4"}
      };
      auto sink = std::map<int, std::string>{};

      pipes::for_each(source) >>= pipes::insert(sink);

      // uses insert, so only the first element for a particular key is
      // actually inserted.
      CHECK(sink
            == std::map<int, std::string>{
              {1, "1"},
              {2, "2"},
              {3, "3"}
      });
    }
  }
}

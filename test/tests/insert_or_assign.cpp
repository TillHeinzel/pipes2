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
  SUBCASE("insert_or_assign")
  {
    SUBCASE("map")
    {
      auto source = std::vector<std::pair<int, std::string>>{
        {1, "1"},
        {2, "2"},
        {3, "3"},
        {3, "4"}
      };
      auto sink = std::map<int, std::string>{};

      pipes::for_each(source) >>= pipes::insert_or_assign(sink);

      // uses insert_or_assign, so the last element pushed in for a
      // particular key is pushed
      CHECK(sink
            == std::map<int, std::string>{
              {1, "1"},
              {2, "2"},
              {3, "4"}
      });
    }

    SUBCASE("temporary")
    {
      auto source = std::vector<std::pair<int, std::string>>{
        {1, "1"},
        {2, "2"},
        {3, "3"},
        {3, "4"}
      };
      auto sink = pipes::for_each(source) >>=
        pipes::insert_or_assign(std::map<int, std::string>{});

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

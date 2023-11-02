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

TEST_CASE("sources")
{
  SUBCASE("Adjacent")
  {
    SUBCASE("")
    {
      auto target = std::vector<std::tuple<int, int>>{};

      pipes::adjacent(std::vector{1}) >>= pipes::push_back(target);

      CHECK(target == std::vector<std::tuple<int, int>>{});
    }

    SUBCASE("")
    {
      auto target = std::vector<std::tuple<int, int>>{};

      pipes::adjacent(std::vector{1, 2}) >>= pipes::push_back(target);

      CHECK(target
            == std::vector<std::tuple<int, int>>{
              {1, 2}
      });
    }
    SUBCASE("")
    {
      auto target = std::vector<std::tuple<int, int>>{};

      pipes::adjacent(std::vector{1, 2, 3}) >>= pipes::push_back(target);

      CHECK(target
            == std::vector<std::tuple<int, int>>{
              {1, 2},
              {2, 3}
      });
    }

    SUBCASE("from function")
    {
      auto target = std::vector<std::tuple<int, int>>{};

      auto f = []() { return pipes::adjacent(std::vector{1, 2, 3}); };

      f() >>= pipes::push_back(target);

      CHECK(target
            == std::vector<std::tuple<int, int>>{
              {1, 2},
              {2, 3}
      });
    }

    SUBCASE("does not copy when it doesn't have to")
    {
      auto src = std::vector<int>{};

      auto pipe = pipes::adjacent(src);

      CHECK(&pipe.source.r.r == &src);
    }

    SUBCASE("")
    {
      auto source = std::map<int, int>{
        {1, 2},
        {2, 4},
        {3, 6}
      };

      auto target =
        std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int>>>{};

      pipes::adjacent(source) >>= pipes::push_back(target);

      CHECK(
        target
        == std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int>>>{
          {{1, 2}, {2, 4}},
          {{2, 4}, {3, 6}}
      });
    }
  }
}

#include <doctest/doctest.h>

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

TEST_CASE("pipes")
{
  SUBCASE("flatten")
  {
    SUBCASE("simple")
    {
      auto source = std::vector<std::vector<int>>{
        {1, 2}
      };

      auto target = std::vector<int>{};
      source >>= pipes::flatten() >>= target;

      CHECK(target == std::vector{1, 2});
    }

    SUBCASE("other types")
    {
      auto source = std::vector<std::map<int, int>>{
        {{1, 2}, {3, 4}},
        {{5, 6}},
        {{7, 8}, {9, 10}}
      };

      auto target = std::vector<std::pair<int, int>>{};
      source >>= pipes::flatten() >>= target;

      CHECK(target
            == std::vector<std::pair<int, int>>{
              {1,  2},
              {3,  4},
              {5,  6},
              {7,  8},
              {9, 10}
      });
    }

    SUBCASE("multiple layers")
    {
      auto source = std::vector<std::vector<std::vector<int>>>{
        {{1, 2}},
        {{3, 4}, {5, 6}}
      };

      auto target = std::vector<int>{};
      source >>= pipes::flatten() >>= pipes::flatten() >>= target;

      CHECK(target == std::vector{1, 2, 3, 4, 5, 6});
    }
  }
}

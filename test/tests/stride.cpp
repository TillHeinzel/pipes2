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

TEST_CASE("pipes")
{
  SUBCASE("stride")
  {
    SUBCASE("simple int")
    {
      auto const source = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

      auto target = std::vector<int>{};

      source >>= pipes::stride(3) >>= target;

      CHECK(target == std::vector{0, 3, 6, 9});
    }

    SUBCASE("with offset")
    {
      auto const source = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

      auto target = std::vector<int>{};

      SUBCASE("small")
      {
        source >>= pipes::stride(3, 1) >>= target;

        CHECK(target == std::vector{1, 4, 7, 10});
      }

      SUBCASE("still small")
      {
        source >>= pipes::stride(3, 2) >>= target;

        CHECK(target == std::vector{2, 5, 8});
      }

      SUBCASE("larger")
      {
        source >>= pipes::stride(3, 7) >>= target;

        CHECK(target == std::vector{7, 10});
      }

      SUBCASE("I SAID LARGER!!!1")
      {
        source >>= pipes::stride(3, 7381875) >>= target;

        CHECK(target == std::vector<int>{});
      }
    }

    SUBCASE("simple string")
    {
      auto const source =
        std::vector<std::string>{"0", "1", "2", "3", "4", "5"};

      auto target = std::vector<std::string>{};

      source >>= pipes::stride(2) >>= target;

      CHECK(target == std::vector<std::string>{"0", "2", "4"});
    }

    SUBCASE("multiple")
    {
      auto const source = std::vector<std::tuple<int, int>>{
        {0, 0},
        {1, 1},
        {2, 2},
        {3, 3},
        {4, 4},
        {5, 5},
      };

      auto target = std::vector<std::tuple<int, int>>{};

      source >>= pipes::stride(2) >>= target;

      CHECK(target
            == std::vector<std::tuple<int, int>>{
              {0, 0},
              {2, 2},
              {4, 4}
      });
    }

    SUBCASE("wrong type cannot link")
    {
      auto target = std::vector<int>{};

      auto sink = pipes::stride(2) >>= target;

      using GoodSource = std::vector<int>;
      using BadSource = std::vector<std::string>;

      static_assert(pipes::CanLink<GoodSource, decltype(sink)>);
      static_assert(!pipes::CanLink<BadSource, decltype(sink)>);
    }
  }
}

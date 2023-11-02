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
  SUBCASE("partition")
  {
    SUBCASE("to vectors")
    {
      auto input = std::vector<int>{1, 2, 3, 4};
      auto evens = std::vector<int>{};
      auto odds = std::vector<int>{};

      auto isEven = [](int n) { return n % 2 == 0; };

      input >>= pipes::partition(isEven, evens, odds);

      CHECK(evens == std::vector{2, 4});
      CHECK(odds == std::vector{1, 3});
    }

    SUBCASE("to pipes")
    {
      auto input = std::vector<int>{1, 2, 3, 4};
      auto evens = std::vector<int>{};
      auto odds = std::vector<int>{};

      auto isEven = [](int n) { return n % 2 == 0; };
      auto doubleIt = pipes::transform([](int n) { return 2 * n; });

      input >>= pipes::partition(isEven, doubleIt >>= evens, doubleIt >>= odds);

      CHECK(evens == std::vector{4, 8});
      CHECK(odds == std::vector{2, 6});
    }

    SUBCASE("multiple parameters")
    {
      auto input = std::vector<std::tuple<int, int>>{
        {1, 1},
        {1, 2},
        {3, 3},
        {4, 5}
      };
      auto same = std::vector<std::tuple<int, int>>{};
      auto diff = std::vector<std::tuple<int, int>>{};

      auto isSame = [](int i, int j) { return i == j; };

      input >>= pipes::partition(isSame, same, diff);

      CHECK(same
            == std::vector<std::tuple<int, int>>{
              {1, 1},
              {3, 3}
      });
      CHECK(diff
            == std::vector<std::tuple<int, int>>{
              {1, 2},
              {4, 5}
      });
    }
  }
}

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
  SUBCASE("take")
  {
    SUBCASE("count")
    {
      auto source = std::vector<int>{1, 2, 3, 4, 5};

      auto sink = std::vector<int>{};
      source >>= pipes::take(3) >>= sink;

      CHECK(sink == std::vector{1, 2, 3});
    }

    SUBCASE("take until")
    {
      auto source = std::vector<int>{1, 2, 3, 4, 5};

      auto sink = std::vector<int>{};
      source >>= pipes::take_until([](int i) { return i == 4; }) >>= sink;

      CHECK(sink == std::vector{1, 2, 3});
    }

    SUBCASE("take while")
    {
      auto source = std::vector<int>{1, 2, 3, 4, 5};

      auto sink = std::vector<int>{};
      source >>= pipes::take_while([](int i) { return i != 4; }) >>= sink;

      CHECK(sink == std::vector{1, 2, 3});
    }
  }
}

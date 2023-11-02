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

TEST_CASE("drop")
{
  SUBCASE("count")
  {
    auto source = std::vector<int>{1, 2, 3, 4, 5};

    auto sink = std::vector<int>{};
    source >>= pipes::drop(3) >>= sink;

    CHECK(sink == std::vector{4, 5});
  }
  SUBCASE("drop until")
  {
    auto source = std::vector<int>{1, 2, 3, 4, 5};

    auto sink = std::vector<int>{};
    source >>= pipes::drop_until([](int i) { return i == 4; }) >>= sink;

    CHECK(sink == std::vector{4, 5});
  }
  SUBCASE("drop while")
  {
    auto source = std::vector<int>{1, 2, 3, 4, 5};

    auto sink = std::vector<int>{};
    source >>= pipes::drop_while([](int i) { return i != 4; }) >>= sink;

    CHECK(sink == std::vector{4, 5});
  }
  SUBCASE("multiple parameters")
  {
    auto source = std::vector<std::tuple<int, int>>{
      {1, 2},
      {2, 2},
      {3, 5}
    };

    auto sink = std::vector<std::tuple<int, int>>{};
    source >>= pipes::drop_until([](int i, int j) { return i == j; }) >>= sink;

    CHECK(sink
          == std::vector<std::tuple<int, int>>{
            {2, 2},
            {3, 5}
    });
  }
}

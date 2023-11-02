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

TEST_CASE("filter")
{
  SUBCASE("empty")
  {
    auto const source = std::vector<int>{};

    auto target = std::vector<int>{};

    source >>= pipes::filter([](int i) { return false; }) >>= target;

    CHECK(target.empty());
  }

  SUBCASE("nonempty all good")
  {
    auto const source = std::vector<int>{1, 2, 3, 4, 5};

    auto target = std::vector<int>{};

    source >>= pipes::filter([](int i) { return true; }) >>= target;

    CHECK(target == source);
  }

  SUBCASE("only odd")
  {
    auto const source = std::vector<int>{1, 2, 3, 4, 5};

    auto target = std::vector<int>{};

    source >>= pipes::filter([](int i) { return i % 2 == 1; }) >>= target;

    CHECK(target == std::vector{1, 3, 5});
  }

  SUBCASE("multiple input parameters")
  {
    auto const source1 = std::vector<int>{1, 2, 3, 6};
    auto const source2 = std::vector<int>{3, 2, 7, 6};

    auto target = std::vector<std::tuple<int, int>>{};

    pipes::zip(source1, source2) >>=
      pipes::filter([](int i, int j) { return i == j; }) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {2, 2},
            {6, 6}
    });
  }
}

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

TEST_CASE("transform")
{
  SUBCASE("Identity")
  {
    SUBCASE("Empty")
    {
      auto const source = std::vector<int>{};

      auto target = std::vector<int>{};

      source >>= pipes::transform([](int i) { return i; }) >>= target;

      CHECK(target.empty());
    }

    SUBCASE("one int")
    {
      auto const source = std::vector<int>{1};

      auto target = std::vector<int>{};

      source >>= pipes::transform([](int i) { return i; }) >>= target;

      CHECK(target == source);
    }

    SUBCASE("multiple ints")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::transform([](int i) { return i; }) >>= target;

      CHECK(target == source);
    }
  }

  SUBCASE("times two")
  {
    auto target = std::vector<int>{};

    std::vector<int>{1, 2, 3, 4, 5} >>=
      pipes::transform([](int i) { return 2 * i; }) >>= target;

    CHECK(target == std::vector{2, 4, 6, 8, 10});
  }

  SUBCASE("chained transforms")
  {
    auto const source = std::vector<int>{1, 2, 3, 4, 5};

    auto target = std::vector<int>{};

    source >>= pipes::transform([](int i) { return 2 * i; }) >>=
      pipes::transform([](int i) { return i + 1; }) >>= target;

    CHECK(target == std::vector{3, 5, 7, 9, 11});
  }

  SUBCASE("string")
  {
    auto const source = std::vector<std::string>{"1", "2", "3"};

    auto target = std::vector<std::string>{};

    auto intermediate =
      pipes::transform([](std::string s) { return s + ";"; }) >>= target;

    source >>= intermediate;

    CHECK(target == std::vector<std::string>{"1;", "2;", "3;"});
  }

  SUBCASE("multiple input parameters")
  {
    auto const source1 = std::vector<int>{1, 2, 3};
    auto const source2 = std::vector<int>{3, 5, 7};

    auto target = std::vector<int>{};

    pipes::zip(source1, source2) >>=
      pipes::transform([](int i, int j) { return i + j; }) >>= target;

    CHECK(target == std::vector<int>{4, 7, 10});
  }
}

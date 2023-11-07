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

#include "support/test_streaming.hpp"

TEST_CASE("chained operations")
{
  SUBCASE("filter transform")
  {
    auto const source = std::vector<int>{1, 2, 3, 4, 5};

    auto target = std::vector<int>{};

    source >>= pipes::filter([](int i) { return i % 2 == 1; }) >>=
      pipes::transform([](int i) { return 2 * i; }) >>= target;

    CHECK(target == std::vector{2, 6, 10});
  }

  SUBCASE("prepare open sink")
  {
    auto const source = std::vector<int>{1, 2, 3, 4, 5};

    auto target = std::vector<int>{};

    auto sink = pipes::filter([](int i) { return i % 2 == 1; }) >>=
      pipes::transform([](int i) { return 2 * i; }) >>= target;

    source >>= sink;

    CHECK(target == std::vector{2, 6, 10});
  }

  SUBCASE("prepare operations-chain with 2 ops")
  {
    auto const source = std::vector<int>{1, 2, 3, 4, 5};

    auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
      pipes::transform([](int i) { return 2 * i; });

    auto target = std::vector<int>{};

    source >>= chain >>= target;

    CHECK(target == std::vector{2, 6, 10});
  }

  SUBCASE("prepare operations-chain with 3 ops")
  {
    auto const source = std::vector<int>{1, 2, 3, 4, 5};

    auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
      pipes::transform([](int i) { return 2 * i; }) >>=
      pipes::transform([](int i) { return i + 1; });

    auto target = std::vector<int>{};

    source >>= chain >>= target;

    CHECK(target == std::vector{3, 7, 11});
  }

  SUBCASE("prepare source without sink")
  {
    SUBCASE("int")
    {
      auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; }) >>=
        pipes::transform([](int i) { return i + 1; });

      auto target = std::vector<int>{};

      auto const orig = std::vector<int>{1, 2, 3, 4, 5};

      auto const source = orig >>= chain;
      source >>= target;

      CHECK(target == std::vector{3, 7, 11});
    }
    SUBCASE("string")
    {
      auto chain = pipes::filter([](std::string s) { return !s.empty(); }) >>=
        pipes::transform([](std::string s) { return s + ";"; });

      auto const orig = std::vector<std::string>{"1", "", "3"};
      auto target = std::vector<std::string>{};

      auto const source = orig >>= chain;
      source >>= target;

      CHECK(target == std::vector<std::string>{"1;", "3;"});
    }
  }

  SUBCASE("prepare source without sink in multiple steps")
  {
    auto target = std::vector<int>{};

    auto const orig = std::vector<int>{1, 2, 3, 4, 5};

    auto const source1 = orig >>=
      pipes::filter([](int i) { return i % 2 == 1; }) >>=
      pipes::transform([](int i) { return 2 * i; });
    auto const source = source1 >>=
      pipes::transform([](int i) { return i + 1; });

    source >>= target;

    CHECK(target == std::vector{3, 7, 11});
  }

  SUBCASE("prepare multi-parameter source without sink")
  {
    auto const source1 = std::vector<int>{1, 2, 3, 6};
    auto const source2 = std::vector<int>{3, 2, 7, 6};

    auto target = std::vector<std::tuple<int, int>>{};

    auto source = pipes::zip(source1, source2) >>=
      pipes::filter([](int i, int j) { return i == j; });

    source >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {2, 2},
            {6, 6}
    });
  }

  SUBCASE("prepare multi-parameter sink without source")
  {
    auto const source1 = std::vector<int>{1, 2, 3, 6};
    auto const source2 = std::vector<int>{3, 2, 7, 6};

    auto target = std::vector<std::tuple<int, int>>{};

    auto sink = pipes::filter([](int i, int j) { return i == j; }) >>= target;

    pipes::zip(source1, source2) >>= sink;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {2, 2},
            {6, 6}
    });
  }

  SUBCASE("zip filter transform")
  {
    auto const source1 = std::vector<int>{1, 2, 3, 6};
    auto const source2 = std::vector<int>{3, 2, 7, 6};

    auto target = std::vector<int>{};

    pipes::zip(source1, source2) >>=
      pipes::filter([](int i, int j) { return i != j; }) >>=
      pipes::transform([](int i, int j) { return i + j; }) >>= target;

    CHECK(target == std::vector<int>{4, 10});
  }
}

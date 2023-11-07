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

TEST_CASE("add_all")
{
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{};
    auto const source2 = std::vector<int>{};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_all(source2) >>= target;

    CHECK(target == std::vector<std::tuple<int, int>>{});
  }

  static_assert(std::ranges::range<std::initializer_list<int>>);
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1};
    auto const source2 = std::vector<int>{};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_all(source2) >>= target;

    CHECK(target == std::vector<std::tuple<int, int>>{});
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{};
    auto const source2 = std::vector<int>{1};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_all(source2) >>= target;

    CHECK(target == std::vector<std::tuple<int, int>>{});
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1};
    auto const source2 = std::vector<int>{4};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_all(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 4}
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1};
    auto target = std::vector<std::tuple<int, int>>{};

    auto pipe = []() { return pipes::add_all(std::vector<int>{4}); };

    source1 >>= pipe() >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 4}
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1};
    auto const source2 = std::vector<int>{4, 5};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_all(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 4},
            {1, 5}
    });
  }

  SUBCASE("")
  {
    auto const source1 = std::vector<std::string>{"1"};
    auto const source2 = std::vector<int>{4, 5};
    auto target = std::vector<std::tuple<std::string, int>>{};

    source1 >>= pipes::add_all(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<std::string, int>>{
            {"1", 4},
            {"1", 5}
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<std::string>{"1"};
    auto const source2 = std::list<std::string>{"4", "5"};
    auto target = std::vector<std::tuple<std::string, std::string>>{};

    source1 >>= pipes::add_all(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<std::string, std::string>>{
            {"1", "4"},
            {"1", "5"}
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1, 2};
    auto const source2 = std::vector<int>{3, 4};
    auto const source3 = std::vector<int>{5, 6};
    auto target = std::vector<std::tuple<int, int, int>>{};

    source1 >>= pipes::add_all(source2) >>= pipes::add_all(source3) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int, int>>{
            {1, 3, 5},
            {1, 3, 6},
            {1, 4, 5},
            {1, 4, 6},
            {2, 3, 5},
            {2, 3, 6},
            {2, 4, 5},
            {2, 4, 6},
    });
  }
}

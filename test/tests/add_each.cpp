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

TEST_CASE("add_each")
{
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{};
    auto const source2 = std::vector<int>{};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target == std::vector<std::tuple<int, int>>{});
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1};
    auto const source2 = std::vector<int>{};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target == std::vector<std::tuple<int, int>>{});
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{};
    auto const source2 = std::vector<int>{1};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target == std::vector<std::tuple<int, int>>{});
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1};
    auto const source2 = std::vector<int>{3};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 3}
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1};
    auto const source2 = []() { return pipes::add_each(std::vector<int>{3}); };
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= source2() >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 3}
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1, 2};
    auto const source2 = std::vector<int>{3};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 3}
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1};
    auto const source2 = std::vector<int>{3, 4};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 3}
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1, 2};
    auto const source2 = std::vector<int>{3, 4};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 3},
            {2, 4},
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1, 2};
    auto const source2 = std::list<int>{3, 4};
    auto target = std::vector<std::tuple<int, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<int, int>>{
            {1, 3},
            {2, 4},
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<std::string>{"1", "2"};
    auto const source2 = std::vector<int>{3, 4};
    auto target = std::vector<std::tuple<std::string, int>>{};

    source1 >>= pipes::add_each(source2) >>= target;

    CHECK(target
          == std::vector<std::tuple<std::string, int>>{
            {"1", 3},
            {"2", 4},
    });
  }
  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1, 2};
    auto const source2 = std::list<int>{3, 4};
    auto const source3 = std::list<int>{5, 6};
    auto target = std::vector<std::tuple<int, int, int>>{};

    source1 >>= pipes::add_each(source2) >>= pipes::add_each(source3) >>=
      target;

    CHECK(target
          == std::vector<std::tuple<int, int, int>>{
            {1, 3, 5},
            {2, 4, 6},
    });
  }

  SUBCASE("")
  {
    auto const source1 = std::vector<int>{1, 2, 2, 2, 2};
    auto const source2 = std::list<int>{3, 4};
    auto const source3 = std::list<int>{5, 6, 6, 6};
    auto target = std::vector<std::tuple<int, int, int>>{};

    source1 >>= pipes::add_each(source2) >>= pipes::add_each(source3) >>=
      target;

    CHECK(target
          == std::vector<std::tuple<int, int, int>>{
            {1, 3, 5},
            {2, 4, 6},
    });
  }
}

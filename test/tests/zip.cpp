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

TEST_CASE("sources")
{
  SUBCASE("zip")
  {
    SUBCASE("single source")
    {
      SUBCASE("int")
      {
        auto const source = std::vector<int>{1, 2, 3};

        auto target = std::vector<std::tuple<int>>{};

        pipes::zip(source) >>= target;

        CHECK(target == std::vector<std::tuple<int>>{{1}, {2}, {3}});
      }

      SUBCASE("temporary from function")
      {
        auto const source = []() {
          return pipes::zip(std::vector<int>{1, 2, 3});
        };

        auto target = std::vector<std::tuple<int>>{};

        source() >>= target;

        CHECK(target == std::vector<std::tuple<int>>{{1}, {2}, {3}});
      }

      SUBCASE("string")
      {
        auto const source = std::vector<std::string>{"1", "2", "3"};

        auto target = std::vector<std::tuple<std::string>>{};

        pipes::zip(source) >>= target;

        CHECK(target
              == std::vector<std::tuple<std::string>>{{"1"}, {"2"}, {"3"}});
      }
    }

    SUBCASE("two sources")
    {
      auto const source1 = std::vector<int>{1, 2, 3};
      auto const source2 = std::vector<std::string>{"a", "b", "c"};

      auto target = std::vector<std::tuple<int, std::string>>{};

      pipes::zip(source1, source2) >>= target;

      CHECK(target
            == std::vector<std::tuple<int, std::string>>{
              {1, "a"},
              {2, "b"},
              {3, "c"}
      });
    }

    SUBCASE("two sources, different lengths")
    {
      auto const source1 = std::vector<int>{1, 2, 3};
      auto const source2 = std::vector<std::string>{"a", "b"};

      auto target = std::vector<std::tuple<int, std::string>>{};

      pipes::zip(source1, source2) >>= target;

      CHECK(target
            == std::vector<std::tuple<int, std::string>>{
              {1, "a"},
              {2, "b"}
      });
    }

    SUBCASE("three sources")
    {
      auto const source1 = std::vector<int>{1, 2, 3};
      auto const source2 = std::vector<std::string>{"a", "b", "c"};
      auto const source3 = std::vector<double>{1.1, 2.2, 3.3};

      auto target = std::vector<std::tuple<int, std::string, double>>{};

      pipes::zip(source1, source2, source3) >>= target;

      CHECK(target
            == std::vector<std::tuple<int, std::string, double>>{
              {1, "a", 1.1},
              {2, "b", 2.2},
              {3, "c", 3.3}
      });
    }

    SUBCASE("unpack tuple into transform")
    {
      auto const source1 = std::vector<int>{1, 2, 3};

      auto target = std::vector<int>{};

      pipes::zip(source1) >>= pipes::transform([](int i) { return 2 * i; }) >>=
        target;

      CHECK(target == std::vector<int>{2, 4, 6});
    }

    SUBCASE("zero sources") {}
    SUBCASE("source with reference types") {}

    SUBCASE("mixed source types")
    {
      auto const source1 = std::vector<int>{1, 2, 3};
      auto source2 = std::map<int, int>{
        {4, 44},
        {5, 55},
        {6, 66}
      };

      auto target = std::vector<std::tuple<int, int>>{};

      pipes::zip(source1, source2) >>= pipes::transform(
        [](int i, auto p) {
          return std::tuple{i, p.second};
        }) >>= pipes::push_back(target);

      CHECK(target
            == std::vector<std::tuple<int, int>>{
              {1, 44},
              {2, 55},
              {3, 66}
      });
    }
  }
}

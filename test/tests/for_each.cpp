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

TEST_CASE("for each")
{
  SUBCASE("vector")
  {
    auto source = std::vector<int>{1, 2, 3};

    auto target = std::vector<int>{};

    SUBCASE("explicit")
    {
      pipes::for_each(source) >>= pipes::push_back(target);

      CHECK(target == source);
    }

    SUBCASE("implicit")
    {
      source >>= pipes::push_back(target);

      CHECK(target == source);
    }
  }

  SUBCASE("initializer_list")
  {
    auto source = {1, 2};
    auto target = std::vector<int>{};

    SUBCASE("explicit")
    {
      pipes::for_each(source) >>= pipes::push_back(target);

      CHECK(target == std::vector{1, 2});
    }

    SUBCASE("implicit")
    {
      source >>= pipes::push_back(target);
      CHECK(target == std::vector{1, 2});
    }
  }

  SUBCASE("map")
  {
    auto source = std::map<int, int>{
      {1, 11},
      {2, 22},
      {3, 33}
    };

    SUBCASE("explicit")
    {
      auto target = std::vector<std::pair<int, int>>{};
      pipes::for_each(source) >>= pipes::push_back(target);

      CHECK(target
            == std::vector<std::pair<int, int>>(source.begin(), source.end()));
    }

    SUBCASE("implicit")
    {
      auto target = std::vector<std::pair<int, int>>{};
      source >>= pipes::push_back(target);

      CHECK(target
            == std::vector<std::pair<int, int>>(source.begin(), source.end()));
    }

    SUBCASE("zip")
    {
      auto source2 = std::map<int, int>{
        {4, 44},
        {5, 55},
        {6, 66}
      };

      auto target = std::vector<std::tuple<int, int>>{};

      pipes::zip(source, source2) >>= pipes::transform(
        [](auto p1, auto p2) {
          return std::tuple{p1.first, p2.second};
        }) >>= pipes::push_back(target);

      CHECK(target
            == std::vector<std::tuple<int, int>>{
              {1, 44},
              {2, 55},
              {3, 66}
      });
    }
  }

  SUBCASE("range views")
  {
    SUBCASE("map keys view") {}
    SUBCASE("map values view") {}
  }

  SUBCASE("temporaries")
  {
    auto target = std::vector<int>{};

    SUBCASE("explicit")
    {
      pipes::for_each(std::vector<int>{1, 2, 3}) >>= pipes::push_back(target);

      CHECK(target == std::vector<int>{1, 2, 3});
    }

    SUBCASE("implicit")
    {
      std::vector<int>{1, 2, 3} >>= pipes::push_back(target);

      CHECK(target == std::vector<int>{1, 2, 3});
    }

    SUBCASE("from function explicit")
    {
      auto f = []() { return pipes::for_each(std::vector{1, 2, 3}); };

      f() >>= pipes::push_back(target);

      CHECK(target == std::vector<int>{1, 2, 3});
    }

    SUBCASE("from function implicit")
    {
      auto f = []() {
        return std::vector{1, 2, 3} >>=
               pipes::transform([](int i) { return i; });
      };

      f() >>= pipes::push_back(target);

      CHECK(target == std::vector<int>{1, 2, 3});
    }

    SUBCASE("for_each does not copy when it doesn't have to")
    {
      auto src = std::vector<int>{};

      auto pipe = pipes::for_each(src);

      CHECK(&pipe.source.r.r == &src);
    }
  }
}

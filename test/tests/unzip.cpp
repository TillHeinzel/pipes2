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
  SUBCASE("unzip")
  {
    SUBCASE("")
    {
      auto s = std::vector<int>{1, 2, 3};

      auto t = std::vector<int>{};

      pipes::zip(s) >>= pipes::unzip(t);

      CHECK(t == s);
    }

    SUBCASE("")
    {
      auto s = std::vector<int>{1, 2, 3};

      auto t = std::vector<int>{};

      pipes::zip(s) >>= pipes::unzip(pipes::push_back(t));

      CHECK(t == s);
    }

    SUBCASE("")
    {
      auto s = std::vector<std::string>{"1", "2", "3"};

      auto t = std::vector<std::string>{};

      pipes::zip(s) >>= pipes::unzip(t);

      CHECK(t == s);
    }

    SUBCASE("")
    {
      auto target = std::vector<int>{};

      auto sink = pipes::unzip(target);

      using GoodSource = std::vector<std::tuple<int>>;
      using BadSource = std::vector<std::tuple<std::string>>;

      static_assert(pipes::CanLink<GoodSource, decltype(sink)>);
      static_assert(!pipes::CanLink<BadSource, decltype(sink)>);
    }

    SUBCASE("")
    {
      auto s1 = std::vector<int>{1, 2, 3};
      auto s2 = std::vector<std::string>{"a", "b", "c"};

      auto t1 = std::vector<int>{};
      auto t2 = std::vector<std::string>{};

      pipes::zip(s1, s2) >>= pipes::unzip(t1, t2);

      CHECK(t1 == s1);
      CHECK(t2 == s2);
    }

    SUBCASE("")
    {
      auto s1 = std::vector<int>{1, 2, 3};
      auto s2 = std::vector<std::tuple<int, std::string>>{
        {4, "a"},
        {5, "b"},
        {6, "c"}
      };

      auto t1 = std::vector<int>{};
      auto t2 = std::vector<int>{};
      auto t3 = std::vector<std::string>{};

      pipes::zip(s1, s2) >>= pipes::unzip(t1, pipes::unzip(t2, t3));

      CHECK(t1 == s1);
      CHECK(t2 == std::vector{4, 5, 6});
      CHECK(t3 == std::vector<std::string>{"a", "b", "c"});
    }
  }

}

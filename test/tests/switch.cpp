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
  SUBCASE("switch")
  {
    SUBCASE("simple default")
    {
      auto numbers = std::vector<int>{1, 2, 3, 4};
      auto rest = std::vector<int>{};

      numbers >>= pipes::switch_(pipes::default_ >>= rest);

      CHECK(rest == numbers);
    }

    SUBCASE("simple case")
    {
      auto numbers = std::vector<int>{1, 2, 3, 4};
      auto target = std::vector<int>{};

      auto isEven = [](int i) { return i % 2 == 0; };

      numbers >>= pipes::switch_(pipes::case_(isEven) >>= target);

      CHECK(target == std::vector{2, 4});
    }

    SUBCASE("simple case with chained ops")
    {
      auto numbers = std::vector<int>{1, 2, 3, 4};
      auto target = std::vector<int>{};

      auto isEven = [](int i) { return i % 2 == 0; };
      auto doubleIt = pipes::transform([](int i) { return 2 * i; });

      numbers >>= pipes::switch_(pipes::case_(isEven) >>= doubleIt >>= target);

      CHECK(target == std::vector{4, 8});
    }

    SUBCASE("2 cases")
    {
      auto numbers = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
      auto m3 = std::vector<int>{};
      auto m4 = std::vector<int>{};

      numbers >>=
        pipes::switch_(pipes::case_([](int i) { return i % 3 == 0; }) >>= m3,
                       pipes::case_([](int i) { return i % 4 == 0; }) >>= m4);

      CHECK(m3 == std::vector{3, 6});
      CHECK(m4 == std::vector{4, 8});
    }

    SUBCASE("2 cases with default")
    {
      auto numbers = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
      auto m3 = std::vector<int>{};
      auto m4 = std::vector<int>{};
      auto rest = std::vector<int>{};

      numbers >>=
        pipes::switch_(pipes::case_([](int i) { return i % 3 == 0; }) >>= m3,
                       pipes::case_([](int i) { return i % 4 == 0; }) >>= m4,
                       pipes::default_ >>= rest);

      CHECK(m3 == std::vector{3, 6});
      CHECK(m4 == std::vector{4, 8});
      CHECK(rest == std::vector{1, 2, 5, 7});
    }

    SUBCASE("wrong type cannot link")
    {
      auto target = std::vector<int>{};
      auto isEven = [](int i) { return i % 2 == 0; };
      auto sw = pipes::switch_(pipes::case_(isEven) >>= target);

      using GoodSource = std::vector<int>;
      using BadSource = std::vector<std::string>;

      static_assert(pipes::CanLink<GoodSource, decltype(sw)>);
      static_assert(!pipes::CanLink<BadSource, decltype(sw)>);
    }

    SUBCASE("other type")
    {
      auto numbers =
        std::vector<std::string>{"1", "___", "22", "a", "aa", "_", "xxxx"};
      auto l1 = std::vector<std::string>{};
      auto l2 = std::vector<std::string>{};
      auto rest = std::vector<std::string>{};

      numbers >>= pipes::switch_(
        pipes::case_([](std::string s) { return s.size() == 1; }) >>= l1,
        pipes::case_([](std::string s) { return s.size() == 2; }) >>= l2,
        pipes::default_ >>= rest);

      CHECK(l1 == std::vector<std::string>{"1", "a", "_"});
      CHECK(l2 == std::vector<std::string>{"22", "aa"});
      CHECK(rest == std::vector<std::string>{"___", "xxxx"});
    }
    SUBCASE("multiple types")
    {
      auto numbers = std::vector<std::tuple<int, int>>{
        {1, 2},
        {3, 4}
      };

      auto target = std::vector<int>{};

      numbers >>= pipes::switch_(
        pipes::case_([](int i, int j) { return i + j > 4; }) >>=
        pipes::transform([](int i, int j) { return i + j; }) >>= target);

      CHECK(target == std::vector<int>{{7}});
    }
  }
}

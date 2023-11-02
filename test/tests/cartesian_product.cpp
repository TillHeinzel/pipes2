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
  SUBCASE("cartesian product")
  {
    SUBCASE("")
    {
      auto const inputs1 = std::vector<int>{1, 2};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1) >>=
        pipes::transform([](int i) { return std::to_string(i); }) >>=
        pipes::push_back(results);

      CHECK(results == std::vector<std::string>{"1", "2"});
    }

    SUBCASE("temporary from function")
    {
      auto const source = []() {
        return pipes::cartesian_product(std::vector<int>{1, 2});
      };

      auto results = std::vector<int>{};

      source() >>= pipes::push_back(results);

      CHECK(results == std::vector{1, 2});
    }

    SUBCASE("")
    {
      auto const inputs1 = std::vector<int>{};
      auto const inputs2 = std::vector<std::string>{};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1, inputs2) >>=
        pipes::transform([](int i, std::string const& s)
                         { return std::to_string(i) + '-' + s; }) >>=
        pipes::push_back(results);

      CHECK(results == std::vector<std::string>{});
    }

    SUBCASE("")
    {
      auto const inputs1 = std::vector<int>{1};
      auto const inputs2 = std::vector<std::string>{};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1, inputs2) >>=
        pipes::transform([](int i, std::string const& s)
                         { return std::to_string(i) + '-' + s; }) >>=
        pipes::push_back(results);

      CHECK(results == std::vector<std::string>{});
    }

    SUBCASE("")
    {
      auto const inputs1 = std::vector<int>{};
      auto const inputs2 = std::vector<std::string>{"up"};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1, inputs2) >>=
        pipes::transform([](int i, std::string const& s)
                         { return std::to_string(i) + '-' + s; }) >>=
        pipes::push_back(results);

      CHECK(results == std::vector<std::string>{});
    }

    SUBCASE("")
    {
      auto const inputs1 = std::vector<int>{1};
      auto const inputs2 = std::vector<std::string>{"up"};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1, inputs2) >>=
        pipes::transform([](int i, std::string const& s)
                         { return std::to_string(i) + '-' + s; }) >>=
        pipes::push_back(results);

      CHECK(results == std::vector<std::string>{"1-up"});
    }

    SUBCASE("")
    {
      auto const inputs1 = std::vector<int>{1, 2};
      auto const inputs2 = std::vector<std::string>{"up"};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1, inputs2) >>=
        pipes::transform([](int i, std::string const& s)
                         { return std::to_string(i) + '-' + s; }) >>=
        pipes::push_back(results);

      CHECK(results == std::vector<std::string>{"1-up", "2-up"});
    }

    SUBCASE("")
    {
      auto const inputs1 = std::vector<int>{1, 2};
      auto const inputs2 = std::vector<std::string>{"up", "down"};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1, inputs2) >>=
        pipes::transform([](int i, std::string const& s)
                         { return std::to_string(i) + '-' + s; }) >>=
        pipes::push_back(results);

      CHECK(results
            == std::vector<std::string>{"1-up", "1-down", "2-up", "2-down"});
    }

    SUBCASE("")
    {
      auto const inputs1 = std::map<int, std::string>{
        {5,   "up"},
        {7, "down"}
      };
      auto const inputs2 = std::vector<int>{2, 3};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1, inputs2) >>= pipes::transform(
        [](auto p, int i) {
          return std::to_string(i * p.first) + '-' + p.second;
        }) >>= pipes::push_back(results);

      CHECK(
        results
        == std::vector<std::string>{"10-up", "15-up", "14-down", "21-down"});
    }

    SUBCASE("")
    {
      auto const inputs1 = std::vector<int>{2, 3};
      auto const inputs2 = std::vector<std::string>{"up", "down"};
      auto const inputs3 = std::vector<int>{5, 7};

      auto results = std::vector<std::string>{};

      pipes::cartesian_product(inputs1, inputs2, inputs3) >>=
        pipes::transform([](int i, std::string const& s, int j)
                         { return std::to_string(i * j) + '-' + s; }) >>=
        pipes::push_back(results);

      CHECK(results
            == std::vector<std::string>{"10-up",
                                        "14-up",
                                        "10-down",
                                        "14-down",
                                        "15-up",
                                        "21-up",
                                        "15-down",
                                        "21-down"});
    }
  }
}

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

TEST_CASE("sinks")
{
  SUBCASE("to_iterator")
  {
    SUBCASE("")
    {
      auto const source = std::vector{1, 2, 3};
      auto sink = std::vector<int>{4, 5, 6};

      pipes::for_each(source) >>= pipes::to_iterator(sink.begin());

      CHECK(sink == source);
    }

    SUBCASE("")
    {
      auto const source = std::vector{1, 2, 3};
      auto sink = std::vector<int>{};

      pipes::for_each(source) >>= pipes::to_iterator(std::back_inserter(sink));

      CHECK(sink == source);
    }

    SUBCASE("")
    {
      auto const source = std::vector<std::string>{"1", "2", "3"};
      auto sink = std::vector<std::string>{};

      pipes::for_each(source) >>= pipes::to_iterator(std::back_inserter(sink));

      CHECK(sink == source);
    }
    SUBCASE("")
    {
      auto const source = std::vector{1, 2, 3};
      auto sink = std::vector<int>{4, 5, 6};

      auto it = pipes::for_each(source) >>= pipes::to_iterator(sink.begin());

      CHECK(it == sink.end());
    }
  }
}

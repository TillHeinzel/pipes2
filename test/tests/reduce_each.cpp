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
  SUBCASE("reduce each")
  {
    SUBCASE("with intial value")
    {
      auto source = std::vector<std::vector<int>>{
        {},
        {1},
        {2, 3},
        {5, 6, 7}
      };

      auto sink = std::vector<int>{};

      source >>= pipes::reduce_each(std::plus{}, 0) >>= sink;

      CHECK(sink == std::vector{0, 1, 5, 18});
    }

    SUBCASE("with inferred intial value")
    {
      auto source = std::vector<std::vector<int>>{
        {},
        {1},
        {2, 3},
        {5, 6, 7}
      };

      auto sink = std::vector<int>{};

      source >>= pipes::reduce_each(std::plus{}) >>= sink;

      CHECK(sink == std::vector{0, 1, 5, 18});
    }
  }
}

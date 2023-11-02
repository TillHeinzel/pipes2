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
  SUBCASE("combinations with self")
  {
    SUBCASE("")
    {
      auto results = //
        pipes::combinations(std::vector<int>{}) >>=
        pipes::push_back(std::vector<std::tuple<int, int>>{});

      CHECK(results == std::vector<std::tuple<int, int>>{});
    }
  }
}

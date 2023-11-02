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
  SUBCASE("tee")
  {
    SUBCASE("")
    {
      auto source = std::vector<int>{1, 2, 3, 4, 5};

      auto sink = std::vector<int>{};
      auto intermediate = std::vector<int>{};

      source >>= pipes::tee(intermediate) >>= sink;

      CHECK(sink == source);
      CHECK(intermediate == source);
    }

    SUBCASE("")
    {
      auto source = std::vector<int>{1, 2, 3, 4, 5};

      auto sink = std::vector<int>{};
      auto intermediate = std::vector<int>{};

      source >>= pipes::tee(pipes::push_back(intermediate)) >>= sink;

      CHECK(sink == source);
      CHECK(intermediate == source);
    }
  }
}

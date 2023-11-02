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
  SUBCASE("generic sink ")
  {
    const auto source = std::vector{1, 2, 3};
    auto sink = std::vector<int>{};

    pipes::for_each(source) >>=
      pipes::generic_sink([&sink](int i) { sink.push_back(i); });

    CHECK(sink == source);
  }
}

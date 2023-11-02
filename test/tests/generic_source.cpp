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
  SUBCASE("generic source")
  {
    auto source = std::vector<int>{1, 2, 3};

    auto target = std::vector<int>{};

    pipes::generic_source([&source](auto& sink)
                          { sink.push(source.front()); }) >>=
      pipes::push_back(target);

    CHECK(target == std::vector{1});
  }
}

#include <doctest/doctest.h>

#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("push_back")
{
  auto source = pipes::for_each(std::vector<int>{1, 2, 3});

  SUBCASE("")
  {
    CHECK((source >> pipes::push_back(std::vector<int>{})) //
          == std::vector<int>{1, 2, 3});

    CHECK((source >> pipes::push_back(std::deque<int>{})) //
          == std::deque<int>{1, 2, 3});

    CHECK((source >> pipes::push_back(std::list<int>{})) //
          == std::list<int>{1, 2, 3});

    auto sink = std::vector<int>{};
    auto& retval = source >> pipes::push_back(sink);

    CHECK(sink == std::vector<int>{1, 2, 3});
    CHECK(&retval == &sink);

    auto makePushBack = []() {
      return pipes::push_back(std::vector<int>{4, 5});
    };

    CHECK((source >> makePushBack()) == std::vector{4, 5, 1, 2, 3});
  }

  SUBCASE("")
  {
    auto v1 = std::vector<int>{};
    auto v2 = std::vector<int>{};

    auto pb = pipes::push_back(v1);
    pb = pipes::push_back(v2);

    source >> pb;

    CHECK(v1 == vals{});
    CHECK(v2 == vals{1, 2, 3});
  }
}

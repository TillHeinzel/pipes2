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

#include "support/sink.hpp"
#include "support/source.hpp"
#include "test_streaming.hpp"

TEST_CASE("push_back")
{
  SUBCASE("vector")
  {
    auto source = std::vector<int>{1, 2, 3};
    auto sink = std::vector<int>{};

    pipes::for_each(source) >>= pipes::push_back(sink);
    CHECK(sink == source);
  }

  SUBCASE("return value from pipeline")
  {
    auto source = std::vector<int>{1, 2, 3};
    auto sink = std::vector<int>{};

    auto& retval = pipes::for_each(source) >>= pipes::push_back(sink);
    CHECK(&sink == &retval);
  }

  SUBCASE("to temporary")
  {
    auto source = std::vector<int>{1, 2, 3};

    auto sink = pipes::for_each(source) >>=
      pipes::push_back(std::vector<int>{});

    CHECK(sink == std::vector{1, 2, 3});
  }

  SUBCASE("to temporary with precreated values from function")
  {
    auto source = std::vector<int>{1, 2, 3};

    auto sink = pipes::for_each(source) >>= []() {
      return pipes::push_back(std::vector<int>{4, 5});
    }();

    CHECK(sink == std::vector{4, 5, 1, 2, 3});
  }

  SUBCASE("deque")
  {
    auto source = std::vector<int>{1, 2, 3};
    auto sink = std::deque<int>{};

    pipes::for_each(source) >>= pipes::push_back(sink);
    CHECK(sink == std::deque<int>(source.begin(), source.end()));
  }
  SUBCASE("list")
  {
    auto source = std::vector<int>{1, 2, 3};
    auto sink = std::list<int>{};

    pipes::for_each(source) >>= pipes::push_back(sink);
    CHECK(sink == std::list<int>(source.begin(), source.end()));
  }
}

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
  SUBCASE("push_front")
  {
    SUBCASE("deque")
    {
      auto source = std::vector<int>{1, 2, 3};
      auto sink = std::deque<int>{};

      pipes::for_each(source) >>= pipes::push_front(sink);
      CHECK(sink == std::deque<int>{3, 2, 1});
    }

    SUBCASE("return value from pipeline")
    {
      auto source = std::vector<int>{1, 2, 3};
      auto sink = std::deque<int>{};

      auto& retval = pipes::for_each(source) >>= pipes::push_front(sink);
      CHECK(&sink == &retval);
    }

    SUBCASE("to temporary")
    {
      auto source = std::vector<int>{1, 2, 3};
      auto sink = pipes::for_each(source) >>=
        pipes::push_front(std::deque<int>{});
      CHECK(sink == std::deque<int>{3, 2, 1});
    }

    SUBCASE("list")
    {
      auto source = std::vector<int>{1, 2, 3};
      auto sink = std::list<int>{};

      pipes::for_each(source) >>= pipes::push_front(sink);
      CHECK(sink == std::list<int>{3, 2, 1});
    }
    SUBCASE("forward_list")
    {
      auto source = std::vector<int>{1, 2, 3};
      auto sink = std::forward_list<int>{};

      pipes::for_each(source) >>= pipes::push_front(sink);
      CHECK(sink == std::forward_list<int>{3, 2, 1});
    }
  }
}

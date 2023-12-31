#include <doctest/doctest.h>

#include <deque>
#include <forward_list>
#include <list>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("push_front")
{
  auto src = pipes::for_each(std::vector<int>{1, 2, 3});

  SUBCASE("")
  {
    CHECK((src >> pipes::push_front(std::deque<int>{})) //
          == std::deque<int>{3, 2, 1});

    CHECK((src >> pipes::push_front(std::list<int>{})) //
          == std::list<int>{3, 2, 1});

    CHECK((src >> pipes::push_front(std::forward_list<int>{})) //
          == std::forward_list<int>{3, 2, 1});

    auto sink = std::deque<int>{};
    auto& retval = src >> pipes::push_front(sink);

    CHECK(sink == std::deque<int>{3, 2, 1});
    CHECK(&retval == &sink);
  }

  SUBCASE("")
  {
    auto v1 = std::deque<int>{};
    auto v2 = std::deque<int>{};

    auto pf = pipes::push_front(v1);
    pf = pipes::push_front(v2);

    src >> pf;

    CHECK(v1 == std::deque<int>{});
    CHECK(v2 == std::deque<int>{3, 2, 1});
  }
}

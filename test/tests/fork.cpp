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
  SUBCASE("fork")
  {
    SUBCASE("to single vector")
    {
      auto source = std::vector{1, 2, 3, 4};
      auto sink = std::vector<int>{};

      source >>= pipes::fork(sink);

      CHECK(sink == source);
    }

    SUBCASE("to single pipeline")
    {
      auto source = std::vector{1, 2, 3, 4};
      auto sink = std::vector<int>{};

      auto target = pipes::drop(0) >>= sink;

      source >>= pipes::fork(target);

      CHECK(sink == source);
    }

    SUBCASE("to multiple vectors")
    {
      auto source = std::vector{1, 2, 3, 4};
      auto sink1 = std::vector<int>{};
      auto sink2 = std::vector<int>{};

      source >>= pipes::fork(sink1, sink2);

      CHECK(sink1 == source);
      CHECK(sink2 == source);
    }

    SUBCASE("to multiple pipelines")
    {
      auto source = std::vector{1, 2, 3, 4};
      auto sink1 = std::vector<int>{};
      auto sink2 = std::vector<int>{};

      auto target1 = pipes::drop(1) >>= sink1;
      auto target2 = pipes::drop(2) >>= sink2;

      source >>= pipes::fork(target1, target2);

      CHECK(sink1 == std::vector{2, 3, 4});
      CHECK(sink2 == std::vector{3, 4});
    }

    SUBCASE("to mixture of pipelines and vectors")
    {
      auto source = std::vector{1, 2, 3, 4};
      auto sink1 = std::vector<int>{};
      auto sink2 = std::vector<int>{};

      auto target2 = pipes::drop(2) >>= sink2;

      source >>= pipes::fork(sink1, target2);

      CHECK(sink1 == std::vector{1, 2, 3, 4});
      CHECK(sink2 == std::vector{3, 4});
    }

    SUBCASE("with unpacking")
    {
      auto source = std::vector<std::tuple<int, int>>{
        {1, 2},
        {3, 4}
      };
      auto sink = std::vector<int>{};

      auto target = pipes::transform([](int i, int j) { return i + j; }) >>=
        sink;

      source >>= pipes::fork(target);

      CHECK(sink == std::vector{3, 7});
    }
    // todo: fail with move-only types
  }
}

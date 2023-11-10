#include <doctest/doctest.h>

#include <string>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("to_iterator")
{
  SUBCASE("")
  {
    auto sink = std::vector<int>{4, 5, 6};

    auto it = source{1, 2, 3} >> pipes::to_iterator(sink.begin());

    CHECK(sink == vals{1, 2, 3});
    CHECK(it == sink.end());
  }

  SUBCASE("")
  {
    auto sink = std::vector<int>{4, 5, 6, 7};

    source{1, 2, 3} >> pipes::to_iterator(sink.begin());

    CHECK(sink == vals{1, 2, 3, 7});
  }

  SUBCASE("")
  {
    auto sink = std::vector<int>{};

    source{1, 2, 3} >> pipes::to_iterator(std::back_inserter(sink));

    CHECK(sink == vals{1, 2, 3});
  }

  SUBCASE("")
  {
    auto sink = std::vector<std::string>{};

    source{"1", "2", "3"} >> pipes::to_iterator(std::back_inserter(sink));

    CHECK(sink == vals{"1", "2", "3"});
  }

  SUBCASE("")
  {
    auto sink = unique_sink();

    auto back = pipes::to_iterator(std::back_inserter(sink));

    pipes::for_each(unique_source(1, 2, 3)) >> back;

    //CHECK(sink == vals{1, 2, 3});
  }
}

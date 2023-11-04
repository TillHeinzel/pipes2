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

TEST_CASE("set_aggregator")
{
  SUBCASE("")
  {
    using set = std::set<int>;

    CHECK((source{1, 2} >> pipes::set_aggregator(set{}, std::plus{})) //
          == set{1, 2});

    CHECK((source{1, 3} >> pipes::set_aggregator(set{1}, std::plus{})) //
          == set{1 + 1, 3});

    CHECK((source{1} >> pipes::set_aggregator(set{1, 2}, std::plus{})) //
          == set{1 + 1 + 2});

    auto sink = set{};

    CHECK((source{1, 2} >> pipes::set_aggregator(sink, std::plus{})) //
          == set{1, 2});
  }

  SUBCASE("")
  {
    using set = std::set<std::string>;

    CHECK((source{"1", "2"} >> pipes::set_aggregator(set{"1"}, std::plus{})) //
          == set{"11", "2"});

    auto sink = set{};

    CHECK((source{"1", "2"} >> pipes::set_aggregator(sink, std::plus{})) //
          == set{"1", "2"});

  }
}

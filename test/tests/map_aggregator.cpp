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

TEST_CASE("map_aggregator")
{
  using t = std::pair<int, int>;
  using map = std::map<int, int>;

  CHECK(
    (source{t{1, 1}, t{2, 2}} >> pipes::map_aggregator(map{}, std::plus{})) //
    == map{{1, 1}, {2, 2}});

  CHECK((source{t{1, 1}, t{2, 5}, t{3, 3}, t{3, 4}}
         >> pipes::map_aggregator(map{{2, 2}}, std::plus{})) //
        == map{{1, 1}, {2, 2 + 5}, {3, 3 + 4}});

  auto sink = map{};

  CHECK(
    (source{t{1, 1}, t{2, 2}} >> pipes::map_aggregator(sink, std::plus{})) //
    == map{{1, 1}, {2, 2}});
}

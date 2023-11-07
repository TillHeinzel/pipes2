#include "doctest.h"

#include <map>
#include <set>
#include <string>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("insert")
{
  SUBCASE("set")
  {
    CHECK((source{1, 2, 3, 3} >> pipes::insert(std::set<int>{}))
          == std::set<int>{1, 2, 3});

    auto sink = std::set<int>{};

    source{1, 2, 3, 3} >> pipes::insert(sink);

    CHECK(sink == std::set<int>{1, 2, 3});
  }

  SUBCASE("map")
  {
    using t = std::pair<int, std::string>;
    using map = std::map<int, std::string>;

    // uses insert, so only the first element for a particular key is
    // actually inserted.
    CHECK((source{t{1, "1"}, t{2, "2"}, t{3, "3"}, t{3, "4"}}
           >> pipes::insert(map{}))
          == map{{1, "1"}, {2, "2"}, {3, "3"}});
  }
}

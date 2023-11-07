#include <doctest/doctest.h>

#include <map>
#include <string>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("insert_or_assign")
{
  // uses insert_or_assign, so the last element pushed in for a
  // particular key is pushed

  using t = std::pair<int, std::string>;
  using map = std::map<int, std::string>;

  auto src =
    pipes::for_each(source{t{1, "1"}, t{2, "2"}, t{3, "3"}, t{3, "4"}});

  CHECK((src >> pipes::insert_or_assign(map{})) //
        == map{{1, "1"}, {2, "2"}, {3, "4"}});

  auto sink = map{};

  CHECK((src >> pipes::insert_or_assign(sink)) //
        == map{{1, "1"}, {2, "2"}, {3, "4"}});
}

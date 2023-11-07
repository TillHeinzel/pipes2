#include "doctest.h"

#include <map>
#include <string>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("default sinks")
{
  SUBCASE("vector default is push_back")
  {
    auto src = pipes::for_each(std::vector{1, 2, 3});

    SUBCASE("")
    {
      CHECK((src >> std::vector<int>{}) //
            == vals{1, 2, 3});
    }

    SUBCASE("")
    {
      auto sink = std::vector<int>{};

      CHECK((src >> sink) //
            == vals{1, 2, 3});
    }
  }

  SUBCASE("map default is insert_or_assign")
  {
    using t = std::pair<int, std::string>;
    using m = std::map<int, std::string>;

    auto src =
      pipes::for_each(source{t{1, "1"}, t{2, "2"}, t{3, "4"}, t{3, "3"}});

    SUBCASE("")
    {
      // uses insert_or_assign, so the last element pushed in for a
      // particular key is pushed
      CHECK((src >> m{t{2, "5"}}) //
            == m{{1, "1"}, {2, "2"}, {3, "3"}});
    }

    SUBCASE("")
    {
      auto sink = m{t{2, "5"}};

      // uses insert_or_assign, so the last element pushed in for a
      // particular key is pushed
      CHECK((src >> sink) //
            == m{{1, "1"}, {2, "2"}, {3, "3"}});
    }
  }
}

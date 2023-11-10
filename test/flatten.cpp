#include <doctest/doctest.h>

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
#include "support/test_streaming.hpp"

TEST_CASE("flatten")
{
  SUBCASE("")
  {
    CHECK((source<std::vector<int>>{{1, 2}} >> pipes::flatten() >> sink{}) //
          == vals{1, 2});
  }

  SUBCASE("")
  {
    CHECK((source<std::vector<std::vector<int>>>{{{1, 2}, {3}}, {{4}, {5, 6}}}
           >> pipes::flatten() >> pipes::flatten() >> sink{}) //
          == vals{1, 2, 3, 4, 5, 6});
  }

  SUBCASE("")
  {
    using t = std::pair<int, int>;

    CHECK((source<std::map<int, int>>{{t{1, 2}, t{3, 4}},
                                      {t{5, 6}},
                                      {t{7, 8}, t{9, 10}}}
           >> pipes::flatten() >> std::vector<std::pair<int, int>>{}) //
          == vals{t{1, 2}, t{3, 4}, t{5, 6}, t{7, 8}, t{9, 10}});
  }

  SUBCASE("")
  {
    auto vv_unique = []()
    {
      auto retval = std::vector<std::vector<std::unique_ptr<int>>>{};
      retval.push_back(unique_source(1, 2));
      return retval;
    };

    CHECK((vv_unique() >> pipes::flatten() >> unique_sink()) //
          == vals{1, 2});
  }
}

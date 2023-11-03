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

using sink_t = sink;

TEST_CASE("for_each")
{
  auto sink = pipes::push_back(sink_t{});

  SUBCASE("vector")
  {
    auto source = std::vector<int>{1, 2, 3};

    CHECK((pipes::for_each(source) >> sink) //
          == vals{1, 2, 3});

    CHECK((source >> sink) //
          == vals{1, 2, 3});

    CHECK((pipes::for_each(std::vector<int>{1, 2, 3}) >> sink) //
          == vals{1, 2, 3});

    CHECK((std::vector<int>{1, 2, 3} >> sink) //
          == vals{1, 2, 3});

    auto makeExplicitForEach = [](auto... is)
    { return pipes::for_each(std::vector<int>{is...}); };

    CHECK((makeExplicitForEach(1, 2, 3) >> sink) //
          == vals{1, 2, 3});

    auto makeImplicitForEach = [](auto... is)
    {
      return std::vector<int>{is...}
             >> pipes::transform([](int i) { return i; });
    };

    CHECK((makeImplicitForEach(1, 2, 3) >> sink) //
          == vals{1, 2, 3});

    // no unneccessary copies
    CHECK(&pipes::for_each(source).source.r.r == &source);
  }

  SUBCASE("initializer_list")
  {
    auto source = {1, 2};

    CHECK((pipes::for_each(source) >> sink) //
          == std::vector{1, 2});

    CHECK((source >> sink) //
          == std::vector{1, 2});

    // does not work, sadly
    // CHECK((pipes::for_each({1, 2}) >> sink) //
    //      == std::vector{1, 2});
  }

  SUBCASE("map")
  {
    auto source = std::map<int, int>{{1, 11}, {2, 22}, {3, 33}};

    using vals = std::vector<std::tuple<int, int>>;

    CHECK((pipes::for_each(source) >> sink) //
          == vals{{1, 11}, {2, 22}, {3, 33}});

    CHECK((source >> sink) //
          == vals{{1, 11}, {2, 22}, {3, 33}});
  }

  SUBCASE("range views")
  {
    auto m = std::map<int, int>{{1, 11}, {2, 22}, {3, 33}};

    CHECK((pipes::for_each(std::views::keys(m)) >> sink) //
          == vals{1, 2, 3});

    CHECK((std::views::keys(m) >> sink) //
          == vals{1, 2, 3});

    auto makeKeyView = [&m]() { return pipes::for_each(std::views::keys(m)); };

    CHECK((makeKeyView() >> sink) //
          == vals{1, 2, 3});
  }
}

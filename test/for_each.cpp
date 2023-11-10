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

using sink_t = sink;

TEST_CASE("for_each")
{
  auto sink = pipes::push_back(sink_t{});

  SUBCASE("vector")
  {
    auto src = std::vector<int>{1, 2, 3};

    CHECK((pipes::for_each(src) >> sink) //
          == vals{1, 2, 3});

    CHECK((src >> sink) //
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
    CHECK(&pipes::for_each(src).source.r.r == &src);
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

    using vals_ = std::vector<std::tuple<int, int>>;

    auto sink = pipes::push_back(vals_{});

    CHECK((pipes::for_each(source) >> sink) //
          == vals_{{1, 11}, {2, 22}, {3, 33}});

    CHECK((source >> sink) //
          == vals_{{1, 11}, {2, 22}, {3, 33}});
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

  SUBCASE("move-only types")
  {
    auto sink = std::vector<std::unique_ptr<int>>{};

    SUBCASE("")
    {
      auto src = unique_source(1, 2, 3);

      auto from = pipes::for_each(std::move(src));
      CHECK(src.size() == 0);

      auto to = pipes::push_back(sink);

      auto res = std::move(from) + to;

      auto connected = connectPipeline(std::move(res));

      std::move(connected).run();
    }

    SUBCASE("")
    {
      pipes::for_each(unique_source(1, 2, 3)) >> pipes::push_back(sink);

      CHECK(sink == vals{1, 2, 3});
    }

    SUBCASE("")
    {
      CHECK(
        (pipes::for_each(unique_source(1, 2, 3)) >> pipes::push_back(sink)) //
        == vals{1, 2, 3});
    }

    SUBCASE("")
    {
      CHECK((unique_source(1, 2, 3) >> pipes::push_back(sink)) //
            == vals{1, 2, 3});
    }

    SUBCASE("")
    {
      CHECK((pipes::detail::defaultSource(unique_source(1, 2, 3))
             >> pipes::push_back(unique_sink()))
            == vals{1, 2, 3});
    }

    //SUBCASE("")
    //{
    //  auto res = link(unique_source(1, 2, 3), pipes::push_back(unique_sink()));

    //  CHECK(res == vals{1, 2, 3});
    //}

    //SUBCASE("")
    //{
    //  auto res = (unique_source(1, 2, 3) >> pipes::push_back(unique_sink()));

    //  CHECK(res == vals{1, 2, 3});
    //}
  }
}

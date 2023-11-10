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

TEST_CASE("push_back")
{
  auto src = pipes::for_each(std::vector<int>{1, 2, 3});

  SUBCASE("")
  {
    CHECK((src >> pipes::push_back(std::vector<int>{})) //
          == std::vector<int>{1, 2, 3});

    CHECK((src >> pipes::push_back(std::deque<int>{})) //
          == std::deque<int>{1, 2, 3});

    CHECK((src >> pipes::push_back(std::list<int>{})) //
          == std::list<int>{1, 2, 3});

    auto sink = std::vector<int>{};
    auto& retval = src >> pipes::push_back(sink);

    CHECK(sink == std::vector<int>{1, 2, 3});
    CHECK(&retval == &sink);

    auto makePushBack = []() {
      return pipes::push_back(std::vector<int>{4, 5});
    };

    CHECK((src >> makePushBack()) == std::vector{4, 5, 1, 2, 3});
  }

  SUBCASE("")
  {
    auto v1 = std::vector<int>{};
    auto v2 = std::vector<int>{};

    auto pb = pipes::push_back(v1);
    pb = pipes::push_back(v2);

    src >> pb;

    CHECK(v1 == vals{});
    CHECK(v2 == vals{1, 2, 3});
  }

  SUBCASE("")
  {
    auto result = pipes::push_back(unique_sink(1, 2)).finalSink.value();

    CHECK(result == vals{1, 2});
  }

  SUBCASE("")
  {
    auto src = pipes::for_each(unique_source(1, 2, 3));

    auto pipeline = std::move(src) + pipes::push_back(unique_sink());

    auto result = std::move(pipeline).sink.value();
  }

  SUBCASE("")
  {
    auto src = pipes::for_each(unique_source(1, 2, 3));

    auto p = std::move(src) + pipes::push_back(unique_sink());

    auto connected =
      pipes::detail::ConnectedPipeline{std::move(p).source,
                                       endNode(std::move(p).sink)};
  }
  SUBCASE("")
  {
    auto src = pipes::for_each(unique_source(1, 2, 3));

    auto p = std::move(src) + pipes::push_back(unique_sink());

     auto connected = connectPipeline(std::move(p));

     auto result = std::move(connected).run();

     CHECK(result == vals{1, 2, 3});
  }

  SUBCASE("")
  {
     auto result = pipes::for_each(unique_source(1, 2, 3))
                   >> pipes::push_back(unique_sink());

     CHECK(result == vals{1, 2, 3});
  }
}

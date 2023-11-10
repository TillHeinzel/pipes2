#include <doctest/doctest.h>

#include <string>
#include <tuple>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("unzip")
{
  auto t = sink{};
  auto t1 = sink{};
  auto t2 = sink{};
  auto t3 = sink{};

  SUBCASE("")
  {
    pipes::zip(source{1, 2, 3}) >> pipes::unzip(t);

    CHECK(t == vals{1, 2, 3});
  }

  SUBCASE("")
  {
    pipes::zip(source{1, 2, 3}) >> pipes::unzip(pipes::push_back(t));

    CHECK(t == vals{1, 2, 3});
  }

  SUBCASE("")
  {
    pipes::zip(source{"1", "2", "3"}) >> pipes::unzip(t);

    CHECK(t == vals{"1", "2", "3"});
  }

  SUBCASE("")
  {
    pipes::zip(source{1, 2, 3}, source{"a", "b", "c"}) >> pipes::unzip(t1, t2);

    CHECK(t1 == vals{1, 2, 3});
    CHECK(t2 == vals{"a", "b", "c"});
  }

  SUBCASE("")
  {
    using t = std::tuple<int, std::string>;

    pipes::zip(source{1, 2, 3}, source{t{4, "a"}, t{5, "b"}, t{6, "c"}})
      >> pipes::unzip(t1, pipes::unzip(t2, t3));

    CHECK(t1 == vals{1, 2, 3});
    CHECK(t2 == vals{4, 5, 6});
    CHECK(t3 == vals{"a", "b", "c"});
  }

  SUBCASE("")
  {
    using t = std::tuple<int, std::string>;

    pipes::zip(source{1, 2, 3}, source{t{4, "a"}, t{5, "b"}, t{6, "c"}})
      >> pipes::unzip(t1, t2, t3);

    CHECK(t1 == vals{1, 2, 3});
    CHECK(t2 == vals{4, 5, 6});
    CHECK(t3 == vals{"a", "b", "c"});
  }

  SUBCASE("")
  {
    auto t1 = unique_source();
    auto t2 = unique_source();

    pipes::zip(unique_source(1, 2), unique_source(3, 4))
      >> pipes::unzip(t1, t2);

    CHECK(t1 == vals{1, 2});
    CHECK(t2 == vals{3, 4});
  }
}

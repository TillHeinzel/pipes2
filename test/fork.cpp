#include <doctest/doctest.h>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("fork")
{
  auto s1 = sink{};
  auto s2 = sink{};

  SUBCASE("")
  {
    source{1, 2, 3, 4} >> pipes::fork(s1);

    CHECK(s1 == vals{1, 2, 3, 4});
  }

  SUBCASE("")
  {
    source{1, 2, 3, 4} >> pipes::fork(pipes::drop(0) >> s1);

    CHECK(s1 == vals{1, 2, 3, 4});
  }

  SUBCASE("")
  {
    source{1, 2, 3, 4} >> pipes::fork(s1, s2);

    CHECK(s1 == vals{1, 2, 3, 4});
    CHECK(s2 == vals{1, 2, 3, 4});
  }

  SUBCASE("")
  {
    source{1, 2, 3, 4} >> pipes::fork( //
      pipes::drop(1) >> s1,            //
      pipes::drop(2) >> s2);

    CHECK(s1 == vals{2, 3, 4});
    CHECK(s2 == vals{3, 4});
  }

  SUBCASE("")
  {
    source{1, 2, 3, 4} >> pipes::fork( //
      s1,                              //
      pipes::drop(2) >> s2);

    CHECK(s1 == vals{1, 2, 3, 4});
    CHECK(s2 == vals{3, 4});
  }

  SUBCASE("")
  {
    using t = std::tuple<int, int>;

    source{t{1, 2}, t{3, 4}} >> pipes::fork(                      //
      pipes::transform([](int i, int j) { return i + j; }) >> s1, //
      s2);

    CHECK(s1 == vals{3, 7});
    CHECK(s2 == vals{t{1, 2}, t{3, 4}});
  }
  // todo: fail with move-only types
}

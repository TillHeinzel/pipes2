#include <doctest/doctest.h>

#include <tuple>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("stride")
{
  CHECK((source{0, 1, 2, 3, 4, 5, 6} >> pipes::stride(1) >> sink{})
        == vals{0, 1, 2, 3, 4, 5, 6});

  CHECK((source{0, 1, 2, 3, 4, 5, 6} >> pipes::stride(2) >> sink{})
        == vals{0, 2, 4, 6});

  CHECK((source{0, 1, 2, 3, 4, 5, 6} >> pipes::stride(3) >> sink{})
        == vals{0, 3, 6});

  auto noOffset = pipes::stride(3, 0);
  CHECK((source{0, 1, 2, 3, 4, 5, 6} >> noOffset >> sink{}) == vals{0, 3, 6});

  auto smallOffset = pipes::stride(3, 1);
  CHECK((source{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10} >> smallOffset >> sink{})
        == vals{1, 4, 7, 10});

  auto slightOffset = pipes::stride(3, 2);
  CHECK((source{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10} >> slightOffset >> sink{})
        == vals{2, 5, 8});

  auto largerOffset = pipes::stride(3, 7);
  CHECK((source{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10} >> largerOffset >> sink{})
        == vals{7, 10});

  auto megaOffset = pipes::stride(3, 7381875);
  CHECK((source{0, 1, 2, 3, 4, 5, 6, 7, 8, 9} >> megaOffset >> sink{})
        == vals{});

  CHECK((source{"0", "1", "2", "3", "4", "5"} >> pipes::stride(2) >> sink{})
        == vals{"0", "2", "4"});

  using t = std::tuple<int, int>;

  CHECK((source{t{0, 0}, t{1, 1}, t{2, 2}, t{3, 3}, t{4, 4}, t{5, 5}}
         >> pipes::stride(2) >> sink{})
        == vals{t{0, 0}, t{2, 2}, t{4, 4}});
}

TEST_CASE("wrong type cannot link")
{
  SUBCASE("drop")
  {
    auto sink = pipes::stride(2) >> std::vector<int>{};

    using GoodSource = std::vector<int>;
    using BadSource = std::vector<std::string>;

    static_assert(pipes::CanLink<GoodSource, decltype(sink)>);
    static_assert(!pipes::CanLink<BadSource, decltype(sink)>);
  }
}

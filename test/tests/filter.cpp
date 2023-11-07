#include <doctest/doctest.h>

#include <tuple>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

using t = std::tuple<int, int>;

TEST_CASE("filter")
{
  auto alwaysFalse = pipes::filter([](int i) { return false; });

  CHECK((source{} >> alwaysFalse >> sink{}) == vals{});

  auto alwaysTrue = pipes::filter([](int i) { return true; });

  CHECK((source{1, 2, 3, 4, 5} >> alwaysTrue >> sink{}) == vals{1, 2, 3, 4, 5});

  auto oddOnly = pipes::filter([](int i) { return i % 2 == 1; });

  CHECK((source{1, 2, 3, 4, 5} >> oddOnly >> sink{}) //
        == vals{1, 3, 5});

  auto sameOnly = pipes::filter([](int i, int j) { return i == j; });

  CHECK((source{t{1, 3}, t{2, 2}, t{3, 7}, t{6, 6}} >> sameOnly >> sink{})
        == vals{t{2, 2}, t{6, 6}});
}

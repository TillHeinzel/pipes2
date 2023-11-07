#include <doctest/doctest.h>

#include <string>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("partition")
{
  auto trues = sink{};
  auto falses = sink{};

  auto isEven = [](int n) { return n % 2 == 0; };
  auto identity = pipes::transform([](int n) { return n; });

  SUBCASE("to vectors")
  {
    source{1, 2, 3, 4} >> pipes::partition(isEven, trues, falses);

    CHECK(trues == vals{2, 4});
    CHECK(falses == vals{1, 3});
  }

  SUBCASE("to pipes")
  {
    source{1, 2, 3, 4}
      >> pipes::partition(isEven, identity >> trues, identity >> falses);

    CHECK(trues == vals{2, 4});
    CHECK(falses == vals{1, 3});
  }

  SUBCASE("multiple parameters")
  {
    using t = std::tuple<int, int>;

    auto isSame = [](int i, int j) { return i == j; };

    source{t{1, 1}, t{1, 2}, t{3, 3}, t{4, 5}}
      >> pipes::partition(isSame, trues, falses);

    CHECK(trues == vals{t{1, 1}, t{3, 3}});
    CHECK(falses == vals{t{1, 2}, t{4, 5}});
  }
}

#include <doctest/doctest.h>

#include <tuple>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

using t = std::tuple<int, int>;

TEST_CASE("drop")
{
  auto drop_first_3 = pipes::drop(3);

  CHECK((source{1, 2, 3, 4, 5} >> drop_first_3 >> sink{}) //
        == vals{4, 5});

  auto drop_until_4 = pipes::drop_until([](int i) { return i == 4; });

  CHECK((source{1, 2, 4, 5} >> drop_until_4 >> sink{}) //
        == vals{4, 5});

  auto drop_while_not_4 = pipes::drop_while([](int i) { return i != 4; });

  CHECK((source{1, 2, 4, 5} >> drop_while_not_4 >> sink{}) //
        == vals{4, 5});

  auto drop_until_same = pipes::drop_until([](int i, int j) { return i == j; });

  CHECK((source{t{1, 2}, t{2, 2}, t{3, 5}} >> drop_until_same >> sink{})
        == vals{t{2, 2}, t{3, 5}});
}

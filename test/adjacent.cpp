#include <doctest/doctest.h>

#include <pipes/pipes.hpp>

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("Adjacent")
{
  using t = std::tuple<int, int>;

  CHECK((pipes::adjacent(std::vector{1}) >> sink{}) //
        == vals{});

  CHECK((pipes::adjacent(std::vector{1, 2}) >> sink{}) //
        == vals{t{1, 2}});

  CHECK((pipes::adjacent(std::vector{1, 2, 3}) >> sink{}) //
        == vals{t{1, 2}, t{2, 3}});

  using tt = std::tuple<std::tuple<int, int>, std::tuple<int, int>>;
  using map = std::map<int, int>;

  CHECK((pipes::adjacent(map{{1, 2}, {2, 4}, {3, 6}}) >> sink{}) //
        == vals{tt{t{1, 2}, t{2, 4}}, tt{t{2, 4}, t{3, 6}}});

  auto f = []() { return pipes::adjacent(std::vector{1, 2, 3}); };

  CHECK((f() >> sink{}) //
        == vals{t{1, 2}, t{2, 3}});

  auto src = std::vector<int>{};

  auto pipe = pipes::adjacent(src);

  CHECK(&pipe.source.r.r == &src);
}

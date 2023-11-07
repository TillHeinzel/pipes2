#include <doctest/doctest.h>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("combinations")
{
  CHECK((pipes::combinations(source{}) >> sink{}) //
        == vals{});

  using t = std::tuple<int, int>;

  CHECK((pipes::combinations(source{1}) >> sink{}) //
        == vals{});

  CHECK((pipes::combinations(source{1, 2}) >> sink{}) //
        == vals{t{1, 2}});

  CHECK((pipes::combinations(source{1, 2, 3}) >> sink{}) //
        == vals{
          t{1, 2},
          t{1, 3},
          t{2, 3},
        });

  CHECK((pipes::combinations(source{1, 2, 3, 4}) >> sink{}) //
        == vals{
          t{1, 2},
          t{1, 3},
          t{1, 4},
          t{2, 3},
          t{2, 4},
          t{3, 4},
        });

  auto src = source{1, 2, 3};
  CHECK((pipes::combinations(src) >> sink{}) //
        == vals{
          t{1, 2},
          t{1, 3},
          t{2, 3},
        });

  using s = std::tuple<std::string, std::string>;

  CHECK((pipes::combinations(source{"a", "b", "c"}) >> sink{}) //
        == vals{
          s{"a", "b"},
          s{"a", "c"},
          s{"b", "c"},
        });
}

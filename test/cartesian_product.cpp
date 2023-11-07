#include <doctest/doctest.h>

#include <map>
#include <vector>
#include <tuple>

#include <pipes/pipes.hpp>

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("cartesian product")
{
  using t = std::tuple<int, std::string>;
  using tt = std::tuple<std::pair<int, std::string>, int>;
  using ttt = std::tuple<int, std::string, int>;

  using map = std::map<int, std::string>;

  CHECK((pipes::cartesian_product(source{1, 2}) >> sink{}) //
        == vals{1, 2});

  CHECK((pipes::cartesian_product(source{}, source{}) >> sink{}) //
        == vals{});

  CHECK((pipes::cartesian_product(source{1}, source{}) >> sink{}) //
        == vals{});

  CHECK((pipes::cartesian_product(source{}, source{"a"}) >> sink{}) //
        == vals{});

  CHECK((pipes::cartesian_product(source{1}, source{"a"}) >> sink{}) //
        == vals{t{1, "a"}});

  CHECK((pipes::cartesian_product(source{1, 2}, source{"a"}) >> sink{}) //
        == vals{t{1, "a"}, t{2, "a"}});

  CHECK((pipes::cartesian_product(source{1, 2}, source{"a", "b"}) >> sink{}) //
        == vals{t{1, "a"}, t{1, "b"}, t{2, "a"}, t{2, "b"}});

  CHECK((pipes::cartesian_product(map{{5, "a"}, {7, "b"}}, source{2, 3})
         >> sink{}) //
        == vals{tt{{5, "a"}, 2},
                tt{{5, "a"}, 3},
                tt{{7, "b"}, 2},
                tt{{7, "b"}, 3}});

  CHECK((pipes::cartesian_product(source{2, 3}, source{"a", "b"}, source{5, 7})
         >> sink{}) //
        == vals{ttt{2, "a", 5},
                ttt{2, "a", 7},
                ttt{2, "b", 5},
                ttt{2, "b", 7},
                ttt{3, "a", 5},
                ttt{3, "a", 7},
                ttt{3, "b", 5},
                ttt{3, "b", 7}});

  auto const makeProduct = []() {
    return pipes::cartesian_product(source{1, 2});
  };

  CHECK((makeProduct() >> sink{}) //
        == vals{1, 2});
}

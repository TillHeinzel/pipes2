#include <doctest/doctest.h>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("tuple_flatten")
{
  CHECK((std::vector<std::tuple<int>>{{1}, {2}, {3}} >> pipes::tuple_flatten()
         >> std::vector<int>{}) //
        == vals{1, 2, 3});

  CHECK((std::vector<std::tuple<int, int>>{{1, 2}, {3, 4}, {5, 6}}
         >> pipes::tuple_flatten() >> std::vector<int>{}) //
        == vals{1, 2, 3, 4, 5, 6});

  CHECK((std::vector<std::tuple<std::tuple<int>, std::tuple<int, int>>>{
           {{1}, {2, 3}},
           {{4}, {5, 6}}}
         >> pipes::tuple_flatten() >> pipes::tuple_flatten()
         >> std::vector<int>{}) //
        == vals{1, 2, 3, 4, 5, 6});
}

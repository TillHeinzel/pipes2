#include <doctest/doctest.h>

#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("tee")
{
  auto side = std::vector<int>{};

  SUBCASE("")
  {
    CHECK((source{1, 2, 3} >> pipes::tee(side) >> sink{}) //
          == vals{1, 2, 3});
    CHECK(side == vals{1, 2, 3});
  }

  SUBCASE("")
  {
    CHECK((source{1, 2, 3} >> pipes::tee(pipes::push_back(side)) >> sink{}) //
          == vals{1, 2, 3});
    CHECK(side == vals{1, 2, 3});
  }
}

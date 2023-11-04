#include "doctest.h"

#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "test_streaming.hpp"

TEST_CASE("generic_sink")
{
  auto sink = std::vector<int>{};

  source{1, 2, 3} >> pipes::generic_sink([&sink](int i) { sink.push_back(i); });

  CHECK(sink == vals{1, 2, 3});
}

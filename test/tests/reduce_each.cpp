#include "doctest.h"

#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "test_streaming.hpp"

TEST_CASE("reduce each")
{
  using sourc_ = source<std::vector<int>>;

  auto with_exlicit_init = pipes::reduce_each(std::plus{}, 0);

  CHECK((sourc_{{}, {1}, {2, 3}, {5, 6, 7}} >> with_exlicit_init >> sink{})
        == vals{0, 1, 5, 18});

  auto with_inferred_init = pipes::reduce_each(std::plus{});

  CHECK((sourc_{{}, {1}, {2, 3}, {5, 6, 7}} >> with_inferred_init >> sink{})
        == vals{0, 1, 5, 18});
}

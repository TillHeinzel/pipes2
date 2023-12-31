#include <doctest/doctest.h>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("take")
{
  CHECK((source{1, 2, 3, 4, 5} >> pipes::take(3) >> sink{}) //
        == vals{1, 2, 3});

  auto equals4 = [](int i) { return i == 4; };

  CHECK((source{1, 2, 3, 4, 5} >> pipes::take_until(equals4) >> sink{}) //
        == vals{1, 2, 3});

  auto unequals4 = [](int i) { return i != 4; };

  CHECK((source{1, 2, 3, 4, 5} >> pipes::take_while(unequals4) >> sink{}) //
        == vals{1, 2, 3});

  CHECK((unique_source(1, 2, 3, 4, 5) >> pipes::take(3) >> unique_sink()) //
        == vals{1, 2, 3});

  CHECK((unique_source(1, 2, 3, 4, 5) >> pipes::take_until(unique(equals4))
         >> unique_sink()) //
        == vals{1, 2, 3});
}

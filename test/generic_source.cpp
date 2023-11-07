#include <doctest/doctest.h>

#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("generic_source")
{
  CHECK((pipes::generic_source([](auto& sink) { sink.push(1); }) >> sink{}) //
        == vals{1});
}

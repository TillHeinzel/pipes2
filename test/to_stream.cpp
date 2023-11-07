#include <doctest/doctest.h>

#include <iostream>
#include <sstream>
#include <string>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("to_stream")
{
  auto ss = std::stringstream{};
  SUBCASE("")
  {
    auto& stream = (source{} >> pipes::to_stream(ss));

    CHECK(ss.str().empty());
    CHECK(&stream == &ss);
  }

  SUBCASE("")
  {
    source{'x'} >> pipes::to_stream(ss);
    CHECK(ss.str() == "x");
  }
  SUBCASE("")
  {
    source{'x', 'y'} >> pipes::to_stream(ss);
    CHECK(ss.str() == "xy");
  }  
  SUBCASE("")
  {
    source{1} >> pipes::to_stream(ss);
    CHECK(ss.str() == "1");
  }  
  
  SUBCASE("")
  {
    source{1,2} >> pipes::to_stream(ss);
    CHECK(ss.str() == "12");
  }

  SUBCASE("")
  {
    source{1.1, 2.2} >> pipes::to_stream(ss);
    CHECK(ss.str() == "1.12.2");
  }
}

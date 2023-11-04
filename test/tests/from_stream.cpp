#include "doctest.h"

#include <sstream>
#include <string>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "test_streaming.hpp"

TEST_CASE("from_stream")
{
  struct StreamHolder
  {
    std::stringstream stream{""};

    std::istream& operator()(char const* s)
    {
      stream.str(s);
      return stream;
    };
  };

  auto stream = StreamHolder{};

  SUBCASE("") { pipes::from_stream(stream("")) >> pipes::discard(); }

  SUBCASE("")
  {
    CHECK((pipes::from_stream(stream("")) >> pipes::push_back(sink{})) //
          == vals{});
  }

  SUBCASE("")
  {
    CHECK((pipes::from_stream(stream("x")) >> pipes::push_back(sink{})) //
          == vals{'x'});
  }

  SUBCASE("")
  {
    CHECK((pipes::from_stream(stream("xyz")) >> pipes::push_back(sink{})) //
          == vals{'x', 'y', 'z'});
  }

  SUBCASE("")
  {
    CHECK((pipes::from_stream<int>(stream("1")) >> pipes::push_back(sink{})) //
          == vals{1});
  }

  SUBCASE("")
  {
    CHECK((pipes::from_stream<int>(stream("1 2")) >> pipes::push_back(sink{})) //
          == vals{1, 2});
  }

  SUBCASE("")
  {
    CHECK((pipes::from_stream<double>(stream("1.1 2")) >> pipes::push_back(sink{})) //
          == vals{1.1, 2});
  }
}

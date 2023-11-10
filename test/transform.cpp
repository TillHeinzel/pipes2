#include <doctest/doctest.h>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("transform")
{
  auto identity = pipes::transform([](auto&& i) { return PIPES_FWD(i); });

  CHECK((source{} >> identity >> sink{}) //
        == vals{});

  CHECK((source{1} >> identity >> sink{}) //
        == vals{1});

  CHECK((source{1, 2, 3} >> identity >> sink{}) //
        == vals{1, 2, 3});

  auto timesTwo = pipes::transform([](int i) { return 2 * i; });

  CHECK((source{1, 2, 3} >> timesTwo >> sink{}) //
        == vals{2, 4, 6});

  auto plus1 = pipes::transform([](int i) { return i + 1; });

  CHECK((source{1, 2, 3} >> timesTwo >> plus1 >> sink{}) //
        == vals{3, 5, 7});

  auto add = pipes::transform([](int i, int j) { return i + j; });

  CHECK((pipes::zip(source{1, 2, 3}, source{3, 5, 7}) >> add >> sink{})
        == vals{4, 7, 10});

  auto appendSemicolon =
    pipes::transform([](std::string s) { return s + ";"; });

  CHECK((source{"1", "2", "3"} >> appendSemicolon >> sink{}) //
        == vals{"1;", "2;", "3;"});

  CHECK((pipes::for_each(unique_source(1, 2, 3)) >> identity
         >> pipes::push_back(unique_sink())) //
        == vals{1, 2, 3});
}

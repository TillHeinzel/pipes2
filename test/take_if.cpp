#pragma once
#include <doctest/doctest.h>

#include <pipes/pipes.hpp>

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("take_if")
{
  auto isOdd = [](int i) { return i % 2 == 1; };
  auto isMultipleOf3 = [](int i) { return i % 3 == 0; };
  auto isMultipleOf4 = [](int i) { return i % 4 == 0; };

  auto doubleIt = [](int i) { return 2 * i; };

  auto taken = sink{};
  auto rest = sink{};

  SUBCASE("")
  {
    source{} >> pipes::take_if(isOdd, taken) >> rest;

    CHECK(taken == vals{});
    CHECK(rest == vals{});
  }

  SUBCASE("")
  {
    source{1} >> pipes::take_if(isOdd, taken) >> rest;

    CHECK(taken == vals{1});
    CHECK(rest == vals{});
  }

  SUBCASE("")
  {
    source{2} >> pipes::take_if(isOdd, taken) >> rest;

    CHECK(taken == vals{});
    CHECK(rest == vals{2});
  }

  SUBCASE("")
  {
    source{1, 2, 3, 4} >> pipes::take_if(isOdd, taken) >> rest;

    CHECK(taken == vals{1, 3});
    CHECK(rest == vals{2, 4});
  }

  SUBCASE("")
  {
    auto to = pipes::transform(doubleIt) >> taken;

    source{1, 2, 3, 4} >> pipes::take_if(isOdd, to) >> rest;

    CHECK(taken == vals{2, 6});
    CHECK(rest == vals{2, 4});
  }

  SUBCASE("")
  {
    source{1, 2, 3, 4}
      >> pipes::take_if(isOdd, pipes::transform(doubleIt) >> taken) >> rest;

    CHECK(taken == vals{2, 6});
    CHECK(rest == vals{2, 4});
  }

  SUBCASE("")
  {
    auto isSingle = [](std::string const& s) { return s.size() == 1; };

    source{"1", "22", "333", "4"} >> pipes::take_if(isSingle, taken) >> rest;

    CHECK(taken == vals{"1", "4"});
    CHECK(rest == vals{"22", "333"});
  }

  SUBCASE("2 cases with default")
  {
    auto taken2 = sink{};

    source{1, 2, 3, 4, 5, 6, 7, 8}             //
      >> pipes::take_if(isMultipleOf3, taken)  //
      >> pipes::take_if(isMultipleOf4, taken2) //
      >> rest;

    CHECK(taken == vals{3, 6});
    CHECK(taken2 == vals{4, 8});
    CHECK(rest == vals{1, 2, 5, 7});
  }
}


TEST_CASE("wrong type cannot link")
{
  SUBCASE("take_if")
  {
    auto isEven = [](int i) { return i % 2 == 0; };

    auto target = std::vector<int>{};
    auto sw = (pipes::take_if(isEven, target) >> pipes::discard());

    using GoodSource = std::vector<int>;
    using BadSource = std::vector<std::string>;

    static_assert(pipes::CanLink<GoodSource, decltype(sw)>);
    static_assert(!pipes::CanLink<BadSource, decltype(sw)>);
  }
}

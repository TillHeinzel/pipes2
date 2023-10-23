#include "doctest.h"

#include <iostream>
#include <string>
#include <vector>

#include "pipes.hpp"

namespace std
{
  std::ostream& operator<<(std::ostream& stream, const std::vector<int>& v)
  {
    stream << "{";
    if(!v.empty())
    {
      stream << v.front();

      for(auto it = std::next(v.begin()); it != v.end(); ++it)
      {
        stream << ", " << *it;
      }
    }
    stream << "}";
    return stream;
  }
} // namespace std

TEST_CASE("test")
{
  // using namespace pipes::operators;

  SUBCASE("transform")
  {
    SUBCASE("Identity")
    {
      SUBCASE("Empty")
      {
        auto const source = std::vector<int>{};

        auto target = std::vector<int>{};

        source >>= pipes::transform([](int i) { return i; }) >>= target;

        REQUIRE(target.empty());
      }

      SUBCASE("one int")
      {
        auto const source = std::vector<int>{1};

        auto target = std::vector<int>{};

        source >>= pipes::transform([](int i) { return i; }) >>= target;

        REQUIRE(target == source);
      }

      SUBCASE("multiple ints")
      {
        auto const source = std::vector<int>{1, 2, 3, 4, 5};

        auto target = std::vector<int>{};

        source >>= pipes::transform([](int i) { return i; }) >>= target;

        REQUIRE(target == source);
      }
    }

    SUBCASE("times two")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::transform([](int i) { return 2 * i; }) >>= target;

      REQUIRE(target == std::vector{2, 4, 6, 8, 10});
    }

    SUBCASE("chained transforms")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::transform([](int i) { return 2 * i; }) >>=
        pipes::transform([](int i) { return i + 1; }) >>= target;

      REQUIRE(target == std::vector{3, 5, 7, 9, 11});
    }
  }

  SUBCASE("filter")
  {
    SUBCASE("empty")
    {
      auto const source = std::vector<int>{};

      auto target = std::vector<int>{};

      source >>= pipes::filter([](int i) { return false; }) >>= target;

      REQUIRE(target.empty());
    }

    SUBCASE("nonempty all good")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::filter([](int i) { return true; }) >>= target;

      REQUIRE(target == source);
    }

    SUBCASE("only odd")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::filter([](int i) { return i % 2 == 1; }) >>= target;

      REQUIRE(target == std::vector{1, 3, 5});
    }
  }

  SUBCASE("chained operations")
  {
    SUBCASE("filter transform")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; }) >>= target;

      REQUIRE(target == std::vector{2, 6, 10});
    }

    SUBCASE("prepare open sink")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      auto sink = pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; }) >>= target;

      source >>= sink;

      REQUIRE(target == std::vector{2, 6, 10});
    }

    SUBCASE("prepare operations-chain with 2 ops")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; });

      auto target = std::vector<int>{};

      source >>= chain >>= target;

      REQUIRE(target == std::vector{2, 6, 10});
    }

    SUBCASE("prepare operations-chain with 3 ops")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; }) >>=
        pipes::transform([](int i) { return i + 1; });

      auto target = std::vector<int>{};

      source >>= chain >>= target;

      REQUIRE(target == std::vector{3, 7, 11});
    }

    SUBCASE("prepare source without sink")
    {
      auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; }) >>=
        pipes::transform([](int i) { return i + 1; });

      auto target = std::vector<int>{};

      auto const orig = std::vector<int>{1, 2, 3, 4, 5};

      auto const source = orig >>= chain;
      source >>= target;

      REQUIRE(target == std::vector{3, 7, 11});
    }

    SUBCASE("prepare source without sink in multiple steps")
    {
      auto target = std::vector<int>{};

      auto const orig = std::vector<int>{1, 2, 3, 4, 5};

      auto const source1 = orig >>=
        pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; });
      auto const source = source1 >>=
        pipes::transform([](int i) { return i + 1; });

      source >>= target;

      REQUIRE(target == std::vector{3, 7, 11});
    }
  }

  // todo: sources without sinks
  // different operators, including <<= >>= << >>
  // additional pipes: reduce, flatten,
}

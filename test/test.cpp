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

  // todo: sources without sinks
  // sinks without sources
  // partially constructed chains
  // different operators, including <<= >>= << >>
  // additional pipes: filter, reduce, flatten,
}

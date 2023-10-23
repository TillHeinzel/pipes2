#include "doctest.h"

#include <iostream>
#include <string>
#include <vector>

#include "pipes.hpp"

namespace std
{
  template<class T>
  std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v)
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

        CHECK(target.empty());
      }

      SUBCASE("one int")
      {
        auto const source = std::vector<int>{1};

        auto target = std::vector<int>{};

        source >>= pipes::transform([](int i) { return i; }) >>= target;

        CHECK(target == source);
      }

      SUBCASE("multiple ints")
      {
        auto const source = std::vector<int>{1, 2, 3, 4, 5};

        auto target = std::vector<int>{};

        source >>= pipes::transform([](int i) { return i; }) >>= target;

        CHECK(target == source);
      }
    }

    SUBCASE("times two")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::transform([](int i) { return 2 * i; }) >>= target;

      CHECK(target == std::vector{2, 4, 6, 8, 10});
    }

    SUBCASE("chained transforms")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::transform([](int i) { return 2 * i; }) >>=
        pipes::transform([](int i) { return i + 1; }) >>= target;

      CHECK(target == std::vector{3, 5, 7, 9, 11});
    }

    SUBCASE("string")
    {
      auto const source = std::vector<std::string>{"1", "2", "3"};

      auto target = std::vector<std::string>{};

      auto intermediate =
        pipes::transform([](std::string s) { return s + ";"; }) >>= target;

      source >>= intermediate;

      CHECK(target == std::vector<std::string>{"1;", "2;", "3;"});
    }
  }

  SUBCASE("filter")
  {
    SUBCASE("empty")
    {
      auto const source = std::vector<int>{};

      auto target = std::vector<int>{};

      source >>= pipes::filter([](int i) { return false; }) >>= target;

      CHECK(target.empty());
    }

    SUBCASE("nonempty all good")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::filter([](int i) { return true; }) >>= target;

      CHECK(target == source);
    }

    SUBCASE("only odd")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      source >>= pipes::filter([](int i) { return i % 2 == 1; }) >>= target;

      CHECK(target == std::vector{1, 3, 5});
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

      CHECK(target == std::vector{2, 6, 10});
    }

    SUBCASE("prepare open sink")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto target = std::vector<int>{};

      auto sink = pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; }) >>= target;

      source >>= sink;

      CHECK(target == std::vector{2, 6, 10});
    }

    SUBCASE("prepare operations-chain with 2 ops")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; });

      auto target = std::vector<int>{};

      source >>= chain >>= target;

      CHECK(target == std::vector{2, 6, 10});
    }

    SUBCASE("prepare operations-chain with 3 ops")
    {
      auto const source = std::vector<int>{1, 2, 3, 4, 5};

      auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
        pipes::transform([](int i) { return 2 * i; }) >>=
        pipes::transform([](int i) { return i + 1; });

      auto target = std::vector<int>{};

      source >>= chain >>= target;

      CHECK(target == std::vector{3, 7, 11});
    }

    SUBCASE("prepare source without sink")
    {
      SUBCASE("int")
      {
        auto chain = pipes::filter([](int i) { return i % 2 == 1; }) >>=
          pipes::transform([](int i) { return 2 * i; }) >>=
          pipes::transform([](int i) { return i + 1; });

        auto target = std::vector<int>{};

        auto const orig = std::vector<int>{1, 2, 3, 4, 5};

        auto const source = orig >>= chain;
        source >>= target;

        CHECK(target == std::vector{3, 7, 11});
      }
      SUBCASE("string")
      {
        auto chain = pipes::filter([](std::string s) { return !s.empty(); }) >>=
          pipes::transform([](std::string s) { return s + ";"; });

        auto const orig = std::vector<std::string>{"1", "", "3"};
        auto target = std::vector<std::string>{};

        auto const source = orig >>= chain;
        source >>= target;

        CHECK(target == std::vector<std::string>{"1;", "3;"});
      }
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

      CHECK(target == std::vector{3, 7, 11});
    }
  }

  // todo: sources without sinks
  // different operators, including <<= >>= << >>
  // additional pipes: reduce, flatten,
}

TEST_CASE("Sink and ValidSource concepts")
{
  SUBCASE("int")
  {
    auto t = pipes::transform([](int i) { return 2 * i; });
    static_assert(!pipes::Sink<decltype(t), int>);

    auto sink = t >>= pipes::DiscardSink{};
    static_assert(pipes::Sink<decltype(sink), int>);
    static_assert(!pipes::Sink<decltype(sink), std::string>);
  }

  SUBCASE("string")
  {
    auto t = pipes::transform([](std::string) -> int { return 0; });
    static_assert(!pipes::Sink<decltype(t), int>);
    static_assert(!pipes::Sink<decltype(t), std::string>);

    auto sink = t >>= pipes::DiscardSink{};
    static_assert(pipes::Sink<decltype(sink), std::string>);
    static_assert(!pipes::Sink<decltype(sink), int>);

    auto sink2 = pipes::append(t, pipes::DiscardSink{});
    static_assert(pipes::Sink<decltype(sink2), std::string>);
    static_assert(!pipes::Sink<decltype(sink2), int>);

    auto source = std::vector<int>{0};
    static_assert(!pipes::ValidSource<decltype(source), decltype(t)>);
  }
}
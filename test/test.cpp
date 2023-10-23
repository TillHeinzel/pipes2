#include "doctest.h"

#include <iostream>
#include <string>
#include <vector>

#include "pipes.hpp"

template<typename T>
concept Streamable = requires(std::ostream& os, T value) {
  {
    os << value
  } -> std::convertible_to<std::ostream&>;
};

template<typename... Ts, size_t... Is>
std::ostream& println_tuple_impl(std::ostream& os,
                                 std::tuple<Ts...> tuple,
                                 std::index_sequence<Is...>)
{
  static_assert(sizeof...(Is) == sizeof...(Ts),
                "Indices must have same number of elements as tuple types!");
  static_assert(sizeof...(Ts) > 0, "Cannot insert empty tuple into stream.");
  static constexpr auto last =
    sizeof...(Ts) - 1; // assuming index sequence 0,...,N-1

  return ((os << std::get<Is>(tuple) << (Is != last ? "," : "")), ...);
}

namespace std
{
  template<class... Ts>
  std::ostream& operator<<(std::ostream& stream, std::tuple<Ts...> const& t)
  {
    stream << "<";
    println_tuple_impl(stream, t, std::index_sequence_for<Ts...>{});
    stream << ">";
    return stream;
  }

  template<class T>
  std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v)
    requires(Streamable<T>)

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

  SUBCASE("discard")
  {
    auto const source = std::vector<int>{1, 2, 3, 4};

    source >>= pipes::discard();
  }

  SUBCASE("drop until") {}
  SUBCASE("fork") {}
  SUBCASE("join") {}
  SUBCASE("partition") {}
  SUBCASE("switch") {}
  SUBCASE("stride") {}
  SUBCASE("take") {}
  SUBCASE("take while") {}
  SUBCASE("tee") {}
  SUBCASE("unzip") {}
  SUBCASE("flatten") {}
  SUBCASE("drop n") {}
  SUBCASE("reduce") {}

  SUBCASE("zip")
  {
    SUBCASE("single source")
    {
      SUBCASE("int")
      {
        auto const source = std::vector<int>{1, 2, 3};

        auto target = std::vector<std::tuple<int>>{};

        pipes::zip(source) >>= target;

        CHECK(target == std::vector<std::tuple<int>>{{1}, {2}, {3}});
      }

      SUBCASE("string")
      {
        auto const source = std::vector<std::string>{"1", "2", "3"};

        auto target = std::vector<std::tuple<std::string>>{};

        pipes::zip(source) >>= target;

        CHECK(target ==
              std::vector<std::tuple<std::string>>{{"1"}, {"2"}, {"3"}});
      }
    }

    SUBCASE("two source")
    {
      auto const source1 = std::vector<int>{1, 2, 3};
      auto const source2 = std::vector<std::string>{"a", "b", "c"};

      auto target = std::vector<std::tuple<int, std::string>>{};

      pipes::zip(source1, source2) >>= target;

      CHECK(target == std::vector<std::tuple<int, std::string>>{
                        {1, "a"},
                        {2, "b"},
                        {3, "c"}
      });
    }

    SUBCASE("two source, different lengths") {}
    SUBCASE("three source") {}
    SUBCASE("two subcases, unpack into transform") {}
    SUBCASE("one source, unpack into transform") {}
    SUBCASE("zero sources") {}
    SUBCASE("source with reference types") {}

    // todo: work with different input sources, such as map
  }
  SUBCASE("override") {}
  SUBCASE("set_aggregator") {}
  SUBCASE("map_aggregator") {}
  SUBCASE("from istream") {}
  SUBCASE("to ostream") {}

  SUBCASE("combinations with self") {}
  SUBCASE("combinations between two (cartesian product)") {}
  SUBCASE("Adjacent") {}
  SUBCASE("mux") {}

  // todo: work with maps
  // todo: work with sets
  // todo: allow temporaries as sources
  // todo: allow temporaries as targets, to be returned in the end
  // todo: different operators, including <<= >>= << >> |
  // todo: move-only types
  // todo: big types with lots of data (do not have to copy everything all the
  // time) todo: generators and take-from for generator
}

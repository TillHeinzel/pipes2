#include "doctest.h"

#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "pipes/pipes.hpp"

template<typename T>
concept Streamable = requires(std::ostream& os, T value) {
  {
    os << value
  } -> std::convertible_to<std::ostream&>;
};

template<class Tuple, size_t... Is>
std::ostream& println_tuple_impl(std::ostream& os,
                                 Tuple tuple,
                                 std::index_sequence<Is...>)
{
  return ((os << std::get<Is>(tuple)
              << (Is != std::tuple_size_v<Tuple> - 1 ? "," : "")),
          ...);
}

namespace std
{
  template<pipes::detail::Tuple_like Tup>
  std::ostream& operator<<(std::ostream& stream, Tup const& t)
  {
    stream << "<";
    println_tuple_impl(stream,
                       t,
                       std::make_index_sequence<std::tuple_size_v<Tup>>{});
    stream << ">";
    return stream;
  }

  template<std::ranges::range R>
  std::ostream& operator<<(std::ostream& stream, R const& v)
    requires(Streamable<std::ranges::range_value_t<R>>
             && !std::same_as<R, std::string>)

  {
    using namespace std::literals::string_literals;

    stream << "{"s;
    if(!v.empty())
    {
      stream << *v.begin();

      for(auto it = std::next(v.begin()); it != v.end(); ++it)
      {
        stream << ", "s << *it;
      }
    }
    stream << "}"s;
    return stream;
  }
} // namespace std

TEST_CASE("test")
{
  SUBCASE("pipes")
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
        auto target = std::vector<int>{};

        std::vector<int>{1, 2, 3, 4, 5} >>=
          pipes::transform([](int i) { return 2 * i; }) >>= target;

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

      SUBCASE("multiple input parameters")
      {
        auto const source1 = std::vector<int>{1, 2, 3};
        auto const source2 = std::vector<int>{3, 5, 7};

        auto target = std::vector<int>{};

        pipes::zip(source1, source2) >>=
          pipes::transform([](int i, int j) { return i + j; }) >>= target;

        CHECK(target == std::vector<int>{4, 7, 10});
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

      SUBCASE("multiple input parameters")
      {
        auto const source1 = std::vector<int>{1, 2, 3, 6};
        auto const source2 = std::vector<int>{3, 2, 7, 6};

        auto target = std::vector<std::tuple<int, int>>{};

        pipes::zip(source1, source2) >>=
          pipes::filter([](int i, int j) { return i == j; }) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {2, 2},
                {6, 6}
        });
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
          auto chain =
            pipes::filter([](std::string s) { return !s.empty(); }) >>=
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

      SUBCASE("prepare multi-parameter source without sink")
      {
        auto const source1 = std::vector<int>{1, 2, 3, 6};
        auto const source2 = std::vector<int>{3, 2, 7, 6};

        auto target = std::vector<std::tuple<int, int>>{};

        auto source = pipes::zip(source1, source2) >>=
          pipes::filter([](int i, int j) { return i == j; });

        source >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {2, 2},
                {6, 6}
        });
      }

      SUBCASE("prepare multi-parameter sink without source")
      {
        auto const source1 = std::vector<int>{1, 2, 3, 6};
        auto const source2 = std::vector<int>{3, 2, 7, 6};

        auto target = std::vector<std::tuple<int, int>>{};

        auto sink = pipes::filter([](int i, int j) { return i == j; }) >>=
          target;

        pipes::zip(source1, source2) >>= sink;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {2, 2},
                {6, 6}
        });
      }

      SUBCASE("zip filter transform")
      {
        auto const source1 = std::vector<int>{1, 2, 3, 6};
        auto const source2 = std::vector<int>{3, 2, 7, 6};

        auto target = std::vector<int>{};

        pipes::zip(source1, source2) >>=
          pipes::filter([](int i, int j) { return i != j; }) >>=
          pipes::transform([](int i, int j) { return i + j; }) >>= target;

        CHECK(target == std::vector<int>{4, 10});
      }
    }

    SUBCASE("drop")
    {
      SUBCASE("count")
      {
        auto source = std::vector<int>{1, 2, 3, 4, 5};

        auto sink = std::vector<int>{};
        source >>= pipes::drop(3) >>= sink;

        CHECK(sink == std::vector{4, 5});
      }
      SUBCASE("drop until")
      {
        auto source = std::vector<int>{1, 2, 3, 4, 5};

        auto sink = std::vector<int>{};
        source >>= pipes::drop_until([](int i) { return i == 4; }) >>= sink;

        CHECK(sink == std::vector{4, 5});
      }
      SUBCASE("drop while")
      {
        auto source = std::vector<int>{1, 2, 3, 4, 5};

        auto sink = std::vector<int>{};
        source >>= pipes::drop_while([](int i) { return i != 4; }) >>= sink;

        CHECK(sink == std::vector{4, 5});
      }
      SUBCASE("multiple parameters")
      {
        auto source = std::vector<std::tuple<int, int>>{
          {1, 2},
          {2, 2},
          {3, 5}
        };

        auto sink = std::vector<std::tuple<int, int>>{};
        source >>= pipes::drop_until([](int i, int j) { return i == j; }) >>=
          sink;

        CHECK(sink
              == std::vector<std::tuple<int, int>>{
                {2, 2},
                {3, 5}
        });
      }
    }

    SUBCASE("fork")
    {
      SUBCASE("to single vector")
      {
        auto source = std::vector{1, 2, 3, 4};
        auto sink = std::vector<int>{};

        source >>= pipes::fork(sink);

        CHECK(sink == source);
      }

      SUBCASE("to single pipeline")
      {
        auto source = std::vector{1, 2, 3, 4};
        auto sink = std::vector<int>{};

        auto target = pipes::drop(0) >>= sink;

        source >>= pipes::fork(target);

        CHECK(sink == source);
      }

      SUBCASE("to multiple vectors")
      {
        auto source = std::vector{1, 2, 3, 4};
        auto sink1 = std::vector<int>{};
        auto sink2 = std::vector<int>{};

        source >>= pipes::fork(sink1, sink2);

        CHECK(sink1 == source);
        CHECK(sink2 == source);
      }

      SUBCASE("to multiple pipelines")
      {
        auto source = std::vector{1, 2, 3, 4};
        auto sink1 = std::vector<int>{};
        auto sink2 = std::vector<int>{};

        auto target1 = pipes::drop(1) >>= sink1;
        auto target2 = pipes::drop(2) >>= sink2;

        source >>= pipes::fork(target1, target2);

        CHECK(sink1 == std::vector{2, 3, 4});
        CHECK(sink2 == std::vector{3, 4});
      }

      SUBCASE("to mixture of pipelines and vectors")
      {
        auto source = std::vector{1, 2, 3, 4};
        auto sink1 = std::vector<int>{};
        auto sink2 = std::vector<int>{};

        auto target2 = pipes::drop(2) >>= sink2;

        source >>= pipes::fork(sink1, target2);

        CHECK(sink1 == std::vector{1, 2, 3, 4});
        CHECK(sink2 == std::vector{3, 4});
      }

      SUBCASE("with unpacking")
      {
        auto source = std::vector<std::tuple<int, int>>{
          {1, 2},
          {3, 4}
        };
        auto sink = std::vector<int>{};

        auto target = pipes::transform([](int i, int j) { return i + j; }) >>=
          sink;

        source >>= pipes::fork(target);

        CHECK(sink == std::vector{3, 7});
      }
      // todo: fail with move-only types
    }

    SUBCASE("flatten")
    {
      SUBCASE("simple")
      {
        auto source = std::vector<std::vector<int>>{
          {1, 2}
        };

        auto target = std::vector<int>{};
        source >>= pipes::flatten() >>= target;

        CHECK(target == std::vector{1, 2});
      }

      SUBCASE("other types")
      {
        auto source = std::vector<std::map<int, int>>{
          {{1, 2}, {3, 4}},
          {{5, 6}},
          {{7, 8}, {9, 10}}
        };

        auto target = std::vector<std::pair<int, int>>{};
        source >>= pipes::flatten() >>= target;

        CHECK(target
              == std::vector<std::pair<int, int>>{
                {1,  2},
                {3,  4},
                {5,  6},
                {7,  8},
                {9, 10}
        });
      }

      SUBCASE("multiple layers")
      {
        auto source = std::vector<std::vector<std::vector<int>>>{
          {{1, 2}},
          {{3, 4}, {5, 6}}
        };

        auto target = std::vector<int>{};
        source >>= pipes::flatten() >>= pipes::flatten() >>= target;

        CHECK(target == std::vector{1, 2, 3, 4, 5, 6});
      }
    }

    SUBCASE("partition")
    {
      SUBCASE("to vectors")
      {
        auto input = std::vector<int>{1, 2, 3, 4};
        auto evens = std::vector<int>{};
        auto odds = std::vector<int>{};

        auto isEven = [](int n) { return n % 2 == 0; };

        input >>= pipes::partition(isEven, evens, odds);

        CHECK(evens == std::vector{2, 4});
        CHECK(odds == std::vector{1, 3});
      }

      SUBCASE("to pipes")
      {
        auto input = std::vector<int>{1, 2, 3, 4};
        auto evens = std::vector<int>{};
        auto odds = std::vector<int>{};

        auto isEven = [](int n) { return n % 2 == 0; };
        auto doubleIt = pipes::transform([](int n) { return 2 * n; });

        input >>=
          pipes::partition(isEven, doubleIt >>= evens, doubleIt >>= odds);

        CHECK(evens == std::vector{4, 8});
        CHECK(odds == std::vector{2, 6});
      }

      SUBCASE("multiple parameters")
      {
        auto input = std::vector<std::tuple<int, int>>{
          {1, 1},
          {1, 2},
          {3, 3},
          {4, 5}
        };
        auto same = std::vector<std::tuple<int, int>>{};
        auto diff = std::vector<std::tuple<int, int>>{};

        auto isSame = [](int i, int j) { return i == j; };

        input >>= pipes::partition(isSame, same, diff);

        CHECK(same
              == std::vector<std::tuple<int, int>>{
                {1, 1},
                {3, 3}
        });
        CHECK(diff
              == std::vector<std::tuple<int, int>>{
                {1, 2},
                {4, 5}
        });
      }
    }

    SUBCASE("switch")
    {
      SUBCASE("simple default")
      {
        auto numbers = std::vector<int>{1, 2, 3, 4};
        auto rest = std::vector<int>{};

        numbers >>= pipes::switch_(pipes::default_ >>= rest);

        CHECK(rest == numbers);
      }

      SUBCASE("simple case")
      {
        auto numbers = std::vector<int>{1, 2, 3, 4};
        auto target = std::vector<int>{};

        auto isEven = [](int i) { return i % 2 == 0; };

        numbers >>= pipes::switch_(pipes::case_(isEven) >>= target);

        CHECK(target == std::vector{2, 4});
      }

      SUBCASE("simple case with chained ops")
      {
        auto numbers = std::vector<int>{1, 2, 3, 4};
        auto target = std::vector<int>{};

        auto isEven = [](int i) { return i % 2 == 0; };
        auto doubleIt = pipes::transform([](int i) { return 2 * i; });

        numbers >>=
          pipes::switch_(pipes::case_(isEven) >>= doubleIt >>= target);

        CHECK(target == std::vector{4, 8});
      }

      SUBCASE("2 cases")
      {
        auto numbers = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
        auto m3 = std::vector<int>{};
        auto m4 = std::vector<int>{};

        numbers >>=
          pipes::switch_(pipes::case_([](int i) { return i % 3 == 0; }) >>= m3,
                         pipes::case_([](int i) { return i % 4 == 0; }) >>= m4);

        CHECK(m3 == std::vector{3, 6});
        CHECK(m4 == std::vector{4, 8});
      }

      SUBCASE("2 cases with default")
      {
        auto numbers = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
        auto m3 = std::vector<int>{};
        auto m4 = std::vector<int>{};
        auto rest = std::vector<int>{};

        numbers >>=
          pipes::switch_(pipes::case_([](int i) { return i % 3 == 0; }) >>= m3,
                         pipes::case_([](int i) { return i % 4 == 0; }) >>= m4,
                         pipes::default_ >>= rest);

        CHECK(m3 == std::vector{3, 6});
        CHECK(m4 == std::vector{4, 8});
        CHECK(rest == std::vector{1, 2, 5, 7});
      }

      SUBCASE("wrong type cannot link")
      {
        auto target = std::vector<int>{};
        auto isEven = [](int i) { return i % 2 == 0; };
        auto sw = pipes::switch_(pipes::case_(isEven) >>= target);

        using GoodSource = std::vector<int>;
        using BadSource = std::vector<std::string>;

        static_assert(pipes::CanLink<GoodSource, decltype(sw)>);
        static_assert(!pipes::CanLink<BadSource, decltype(sw)>);
      }

      SUBCASE("other type")
      {
        auto numbers =
          std::vector<std::string>{"1", "___", "22", "a", "aa", "_", "xxxx"};
        auto l1 = std::vector<std::string>{};
        auto l2 = std::vector<std::string>{};
        auto rest = std::vector<std::string>{};

        numbers >>= pipes::switch_(
          pipes::case_([](std::string s) { return s.size() == 1; }) >>= l1,
          pipes::case_([](std::string s) { return s.size() == 2; }) >>= l2,
          pipes::default_ >>= rest);

        CHECK(l1 == std::vector<std::string>{"1", "a", "_"});
        CHECK(l2 == std::vector<std::string>{"22", "aa"});
        CHECK(rest == std::vector<std::string>{"___", "xxxx"});
      }
      SUBCASE("multiple types")
      {
        auto numbers = std::vector<std::tuple<int, int>>{
          {1, 2},
          {3, 4}
        };

        auto target = std::vector<int>{};

        numbers >>= pipes::switch_(
          pipes::case_([](int i, int j) { return i + j > 4; }) >>=
          pipes::transform([](int i, int j) { return i + j; }) >>= target);

        CHECK(target == std::vector<int>{{7}});
      }
    }

    SUBCASE("stride")
    {
      SUBCASE("simple int")
      {
        auto const source = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto target = std::vector<int>{};

        source >>= pipes::stride(3) >>= target;

        CHECK(target == std::vector{0, 3, 6, 9});
      }

      SUBCASE("with offset")
      {
        auto const source = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto target = std::vector<int>{};

        SUBCASE("small")
        {
          source >>= pipes::stride(3, 1) >>= target;

          CHECK(target == std::vector{1, 4, 7, 10});
        }

        SUBCASE("still small")
        {
          source >>= pipes::stride(3, 2) >>= target;

          CHECK(target == std::vector{2, 5, 8});
        }

        SUBCASE("larger")
        {
          source >>= pipes::stride(3, 7) >>= target;

          CHECK(target == std::vector{7, 10});
        }

        SUBCASE("I SAID LARGER!!!1")
        {
          source >>= pipes::stride(3, 7381875) >>= target;

          CHECK(target == std::vector<int>{});
        }
      }

      SUBCASE("simple string")
      {
        auto const source =
          std::vector<std::string>{"0", "1", "2", "3", "4", "5"};

        auto target = std::vector<std::string>{};

        source >>= pipes::stride(2) >>= target;

        CHECK(target == std::vector<std::string>{"0", "2", "4"});
      }

      SUBCASE("multiple")
      {
        auto const source = std::vector<std::tuple<int, int>>{
          {0, 0},
          {1, 1},
          {2, 2},
          {3, 3},
          {4, 4},
          {5, 5},
        };

        auto target = std::vector<std::tuple<int, int>>{};

        source >>= pipes::stride(2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {0, 0},
                {2, 2},
                {4, 4}
        });
      }

      SUBCASE("wrong type cannot link")
      {
        auto target = std::vector<int>{};

        auto sink = pipes::stride(2) >>= target;

        using GoodSource = std::vector<int>;
        using BadSource = std::vector<std::string>;

        static_assert(pipes::CanLink<GoodSource, decltype(sink)>);
        static_assert(!pipes::CanLink<BadSource, decltype(sink)>);
      }
    }

    SUBCASE("take")
    {
      SUBCASE("count")
      {
        auto source = std::vector<int>{1, 2, 3, 4, 5};

        auto sink = std::vector<int>{};
        source >>= pipes::take(3) >>= sink;

        CHECK(sink == std::vector{1, 2, 3});
      }

      SUBCASE("take until")
      {
        auto source = std::vector<int>{1, 2, 3, 4, 5};

        auto sink = std::vector<int>{};
        source >>= pipes::take_until([](int i) { return i == 4; }) >>= sink;

        CHECK(sink == std::vector{1, 2, 3});
      }

      SUBCASE("take while")
      {
        auto source = std::vector<int>{1, 2, 3, 4, 5};

        auto sink = std::vector<int>{};
        source >>= pipes::take_while([](int i) { return i != 4; }) >>= sink;

        CHECK(sink == std::vector{1, 2, 3});
      }
    }

    SUBCASE("tee")
    {
      SUBCASE("")
      {
        auto source = std::vector<int>{1, 2, 3, 4, 5};

        auto sink = std::vector<int>{};
        auto intermediate = std::vector<int>{};

        source >>= pipes::tee(intermediate) >>= sink;

        CHECK(sink == source);
        CHECK(intermediate == source);
      }

      SUBCASE("")
      {
        auto source = std::vector<int>{1, 2, 3, 4, 5};

        auto sink = std::vector<int>{};
        auto intermediate = std::vector<int>{};

        source >>= pipes::tee(pipes::push_back(intermediate)) >>= sink;

        CHECK(sink == source);
        CHECK(intermediate == source);
      }
    }

    SUBCASE("unzip")
    {
      SUBCASE("")
      {
        auto s = std::vector<int>{1, 2, 3};

        auto t = std::vector<int>{};

        pipes::zip(s) >>= pipes::unzip(t);

        CHECK(t == s);
      }

      SUBCASE("")
      {
        auto s = std::vector<int>{1, 2, 3};

        auto t = std::vector<int>{};

        pipes::zip(s) >>= pipes::unzip(pipes::push_back(t));

        CHECK(t == s);
      }

      SUBCASE("")
      {
        auto s = std::vector<std::string>{"1", "2", "3"};

        auto t = std::vector<std::string>{};

        pipes::zip(s) >>= pipes::unzip(t);

        CHECK(t == s);
      }

      SUBCASE("")
      {
        auto target = std::vector<int>{};

        auto sink = pipes::unzip(target);

        using GoodSource = std::vector<std::tuple<int>>;
        using BadSource = std::vector<std::tuple<std::string>>;

        static_assert(pipes::CanLink<GoodSource, decltype(sink)>);
        static_assert(!pipes::CanLink<BadSource, decltype(sink)>);
      }

      SUBCASE("")
      {
        auto s1 = std::vector<int>{1, 2, 3};
        auto s2 = std::vector<std::string>{"a", "b", "c"};

        auto t1 = std::vector<int>{};
        auto t2 = std::vector<std::string>{};

        pipes::zip(s1, s2) >>= pipes::unzip(t1, t2);

        CHECK(t1 == s1);
        CHECK(t2 == s2);
      }

      SUBCASE("")
      {
        auto s1 = std::vector<int>{1, 2, 3};
        auto s2 = std::vector<std::tuple<int, std::string>>{
          {4, "a"},
          {5, "b"},
          {6, "c"}
        };

        auto t1 = std::vector<int>{};
        auto t2 = std::vector<int>{};
        auto t3 = std::vector<std::string>{};

        pipes::zip(s1, s2) >>= pipes::unzip(t1, pipes::unzip(t2, t3));

        CHECK(t1 == s1);
        CHECK(t2 == std::vector{4, 5, 6});
        CHECK(t3 == std::vector<std::string>{"a", "b", "c"});
      }
    }

    SUBCASE("reduce each")
    {
      SUBCASE("with intial value")
      {
        auto source = std::vector<std::vector<int>>{
          {},
          {1},
          {2, 3},
          {5, 6, 7}
        };

        auto sink = std::vector<int>{};

        source >>= pipes::reduce_each(std::plus{}, 0) >>= sink;

        CHECK(sink == std::vector{0, 1, 5, 18});
      }

      SUBCASE("with inferred intial value")
      {
        auto source = std::vector<std::vector<int>>{
          {},
          {1},
          {2, 3},
          {5, 6, 7}
        };

        auto sink = std::vector<int>{};

        source >>= pipes::reduce_each(std::plus{}) >>= sink;

        CHECK(sink == std::vector{0, 1, 5, 18});
      }
    }

    SUBCASE("chunk")
    {
      // todo: chunking the input into chunks of size N

      // todo: need to be able to define the chunk-type, e.g. std::vector or
      // std::map, with certain defaults possible
    }

    SUBCASE("add all")
    {
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{};
        auto const source2 = std::vector<int>{};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_all(source2) >>= target;

        CHECK(target == std::vector<std::tuple<int, int>>{});
      }

      static_assert(std::ranges::range<std::initializer_list<int>>);
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1};
        auto const source2 = std::vector<int>{};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_all(source2) >>= target;

        CHECK(target == std::vector<std::tuple<int, int>>{});
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{};
        auto const source2 = std::vector<int>{1};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_all(source2) >>= target;

        CHECK(target == std::vector<std::tuple<int, int>>{});
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1};
        auto const source2 = std::vector<int>{4};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_all(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 4}
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1};
        auto target = std::vector<std::tuple<int, int>>{};

        auto pipe = []() { return pipes::add_all(std::vector<int>{4}); };

        source1 >>= pipe() >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 4}
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1};
        auto const source2 = std::vector<int>{4, 5};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_all(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 4},
                {1, 5}
        });
      }

      SUBCASE("")
      {
        auto const source1 = std::vector<std::string>{"1"};
        auto const source2 = std::vector<int>{4, 5};
        auto target = std::vector<std::tuple<std::string, int>>{};

        source1 >>= pipes::add_all(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<std::string, int>>{
                {"1", 4},
                {"1", 5}
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<std::string>{"1"};
        auto const source2 = std::list<std::string>{"4", "5"};
        auto target = std::vector<std::tuple<std::string, std::string>>{};

        source1 >>= pipes::add_all(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<std::string, std::string>>{
                {"1", "4"},
                {"1", "5"}
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1, 2};
        auto const source2 = std::vector<int>{3, 4};
        auto const source3 = std::vector<int>{5, 6};
        auto target = std::vector<std::tuple<int, int, int>>{};

        source1 >>= pipes::add_all(source2) >>= pipes::add_all(source3) >>=
          target;

        CHECK(target
              == std::vector<std::tuple<int, int, int>>{
                {1, 3, 5},
                {1, 3, 6},
                {1, 4, 5},
                {1, 4, 6},
                {2, 3, 5},
                {2, 3, 6},
                {2, 4, 5},
                {2, 4, 6},
        });
      }
    }

    SUBCASE("add each")
    {
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{};
        auto const source2 = std::vector<int>{};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target == std::vector<std::tuple<int, int>>{});
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1};
        auto const source2 = std::vector<int>{};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target == std::vector<std::tuple<int, int>>{});
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{};
        auto const source2 = std::vector<int>{1};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target == std::vector<std::tuple<int, int>>{});
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1};
        auto const source2 = std::vector<int>{3};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 3}
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1};
        auto const source2 = []()
        { return pipes::add_each(std::vector<int>{3}); };
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= source2() >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 3}
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1, 2};
        auto const source2 = std::vector<int>{3};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 3}
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1};
        auto const source2 = std::vector<int>{3, 4};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 3}
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1, 2};
        auto const source2 = std::vector<int>{3, 4};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 3},
                {2, 4},
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1, 2};
        auto const source2 = std::list<int>{3, 4};
        auto target = std::vector<std::tuple<int, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 3},
                {2, 4},
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<std::string>{"1", "2"};
        auto const source2 = std::vector<int>{3, 4};
        auto target = std::vector<std::tuple<std::string, int>>{};

        source1 >>= pipes::add_each(source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<std::string, int>>{
                {"1", 3},
                {"2", 4},
        });
      }
      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1, 2};
        auto const source2 = std::list<int>{3, 4};
        auto const source3 = std::list<int>{5, 6};
        auto target = std::vector<std::tuple<int, int, int>>{};

        source1 >>= pipes::add_each(source2) >>= pipes::add_each(source3) >>=
          target;

        CHECK(target
              == std::vector<std::tuple<int, int, int>>{
                {1, 3, 5},
                {2, 4, 6},
        });
      }

      SUBCASE("")
      {
        auto const source1 = std::vector<int>{1, 2, 2, 2, 2};
        auto const source2 = std::list<int>{3, 4};
        auto const source3 = std::list<int>{5, 6, 6, 6};
        auto target = std::vector<std::tuple<int, int, int>>{};

        source1 >>= pipes::add_each(source2) >>= pipes::add_each(source3) >>=
          target;

        CHECK(target
              == std::vector<std::tuple<int, int, int>>{
                {1, 3, 5},
                {2, 4, 6},
        });
      }
    }
  }

  SUBCASE("sources")
  {
    SUBCASE("for each-iterables as inputs")
    {
      SUBCASE("vector")
      {
        auto source = std::vector<int>{1, 2, 3};

        auto target = std::vector<int>{};

        SUBCASE("explicit")
        {
          pipes::for_each(source) >>= pipes::push_back(target);

          CHECK(target == source);
        }

        SUBCASE("implicit")
        {
          source >>= pipes::push_back(target);

          CHECK(target == source);
        }
      }

      SUBCASE("initializer_list")
      {
        auto source = {1, 2};
        auto target = std::vector<int>{};

        SUBCASE("explicit")
        {
          pipes::for_each(source) >>= pipes::push_back(target);

          CHECK(target == std::vector{1, 2});
        }

        SUBCASE("implicit")
        {
          source >>= pipes::push_back(target);
          CHECK(target == std::vector{1, 2});
        }

        SUBCASE("inline explicit")
        {
          pipes::for_each({1, 2}) >>= pipes::push_back(target);
          CHECK(target == std::vector{1, 2});
        }
      }

      SUBCASE("map")
      {
        auto source = std::map<int, int>{
          {1, 11},
          {2, 22},
          {3, 33}
        };

        SUBCASE("explicit")
        {
          auto target = std::vector<std::pair<int, int>>{};
          pipes::for_each(source) >>= pipes::push_back(target);

          CHECK(
            target
            == std::vector<std::pair<int, int>>(source.begin(), source.end()));
        }

        SUBCASE("implicit")
        {
          auto target = std::vector<std::pair<int, int>>{};
          source >>= pipes::push_back(target);

          CHECK(
            target
            == std::vector<std::pair<int, int>>(source.begin(), source.end()));
        }

        SUBCASE("zip")
        {
          auto source2 = std::map<int, int>{
            {4, 44},
            {5, 55},
            {6, 66}
          };

          auto target = std::vector<std::tuple<int, int>>{};

          pipes::zip(source, source2) >>= pipes::transform(
            [](auto p1, auto p2) {
              return std::tuple{p1.first, p2.second};
            }) >>= pipes::push_back(target);

          CHECK(target
                == std::vector<std::tuple<int, int>>{
                  {1, 44},
                  {2, 55},
                  {3, 66}
          });
        }
      }

      SUBCASE("range views")
      {
        SUBCASE("map keys view") {}
        SUBCASE("map values view") {}
      }

      SUBCASE("temporaries")
      {
        auto target = std::vector<int>{};

        SUBCASE("explicit")
        {
          pipes::for_each(std::vector<int>{1, 2, 3}) >>=
            pipes::push_back(target);

          CHECK(target == std::vector<int>{1, 2, 3});
        }

        SUBCASE("implicit")
        {
          std::vector<int>{1, 2, 3} >>= pipes::push_back(target);

          CHECK(target == std::vector<int>{1, 2, 3});
        }

        SUBCASE("from function explicit")
        {
          auto f = []() { return pipes::for_each(std::vector{1, 2, 3}); };

          f() >>= pipes::push_back(target);

          CHECK(target == std::vector<int>{1, 2, 3});
        }

        SUBCASE("from function implicit")
        {
          auto f = []() {
            return std::vector{1, 2, 3} >>=
                   pipes::transform([](int i) { return i; });
          };

          f() >>= pipes::push_back(target);

          CHECK(target == std::vector<int>{1, 2, 3});
        }

        SUBCASE("for_each does not copy when it doesn't have to")
        {
          auto src = std::vector<int>{};

          auto pipe = pipes::for_each(src);

          CHECK(&pipe.source.r.r == &src);
        }
      }
    }

    SUBCASE("Adjacent")
    {
      SUBCASE("")
      {
        auto target = std::vector<std::tuple<int, int>>{};

        pipes::adjacent(std::vector{1}) >>= pipes::push_back(target);

        CHECK(target == std::vector<std::tuple<int, int>>{});
      }

      SUBCASE("")
      {
        auto target = std::vector<std::tuple<int, int>>{};

        pipes::adjacent(std::vector{1, 2}) >>= pipes::push_back(target);

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 2}
        });
      }
      SUBCASE("")
      {
        auto target = std::vector<std::tuple<int, int>>{};

        pipes::adjacent(std::vector{1, 2, 3}) >>= pipes::push_back(target);

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 2},
                {2, 3}
        });
      }

      SUBCASE("from function")
      {
        auto target = std::vector<std::tuple<int, int>>{};

        auto f = []() { return pipes::adjacent(std::vector{1, 2, 3}); };

        f() >>= pipes::push_back(target);

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 2},
                {2, 3}
        });
      }

      SUBCASE("does not copy when it doesn't have to")
      {
        auto src = std::vector<int>{};

        auto pipe = pipes::adjacent(src);

        CHECK(&pipe.source.r.r == &src);
      }

      SUBCASE("")
      {
        auto source = std::map<int, int>{
          {1, 2},
          {2, 4},
          {3, 6}
        };

        auto target =
          std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int>>>{};

        pipes::adjacent(source) >>= pipes::push_back(target);

        CHECK(target
              == std::vector<
                std::tuple<std::tuple<int, int>, std::tuple<int, int>>>{
                {{1, 2}, {2, 4}},
                {{2, 4}, {3, 6}}
        });
      }
    }

    SUBCASE("cartesian product")
    {
      SUBCASE("")
      {
        auto const inputs1 = std::vector<int>{1, 2};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1) >>=
          pipes::transform([](int i) { return std::to_string(i); }) >>=
          pipes::push_back(results);

        CHECK(results == std::vector<std::string>{"1", "2"});
      }

      SUBCASE("temporary from function")
      {
        auto const source = []() {
          return pipes::cartesian_product(std::vector<int>{1, 2});
        };

        auto results = std::vector<int>{};

        source() >>= pipes::push_back(results);

        CHECK(results == std::vector{1, 2});
      }

      SUBCASE("")
      {
        auto const inputs1 = std::vector<int>{};
        auto const inputs2 = std::vector<std::string>{};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1, inputs2) >>=
          pipes::transform([](int i, std::string const& s)
                           { return std::to_string(i) + '-' + s; }) >>=
          pipes::push_back(results);

        CHECK(results == std::vector<std::string>{});
      }

      SUBCASE("")
      {
        auto const inputs1 = std::vector<int>{1};
        auto const inputs2 = std::vector<std::string>{};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1, inputs2) >>=
          pipes::transform([](int i, std::string const& s)
                           { return std::to_string(i) + '-' + s; }) >>=
          pipes::push_back(results);

        CHECK(results == std::vector<std::string>{});
      }

      SUBCASE("")
      {
        auto const inputs1 = std::vector<int>{};
        auto const inputs2 = std::vector<std::string>{"up"};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1, inputs2) >>=
          pipes::transform([](int i, std::string const& s)
                           { return std::to_string(i) + '-' + s; }) >>=
          pipes::push_back(results);

        CHECK(results == std::vector<std::string>{});
      }

      SUBCASE("")
      {
        auto const inputs1 = std::vector<int>{1};
        auto const inputs2 = std::vector<std::string>{"up"};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1, inputs2) >>=
          pipes::transform([](int i, std::string const& s)
                           { return std::to_string(i) + '-' + s; }) >>=
          pipes::push_back(results);

        CHECK(results == std::vector<std::string>{"1-up"});
      }

      SUBCASE("")
      {
        auto const inputs1 = std::vector<int>{1, 2};
        auto const inputs2 = std::vector<std::string>{"up"};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1, inputs2) >>=
          pipes::transform([](int i, std::string const& s)
                           { return std::to_string(i) + '-' + s; }) >>=
          pipes::push_back(results);

        CHECK(results == std::vector<std::string>{"1-up", "2-up"});
      }

      SUBCASE("")
      {
        auto const inputs1 = std::vector<int>{1, 2};
        auto const inputs2 = std::vector<std::string>{"up", "down"};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1, inputs2) >>=
          pipes::transform([](int i, std::string const& s)
                           { return std::to_string(i) + '-' + s; }) >>=
          pipes::push_back(results);

        CHECK(results
              == std::vector<std::string>{"1-up", "1-down", "2-up", "2-down"});
      }

      SUBCASE("")
      {
        auto const inputs1 = std::map<int, std::string>{
          {5,   "up"},
          {7, "down"}
        };
        auto const inputs2 = std::vector<int>{2, 3};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1, inputs2) >>= pipes::transform(
          [](auto p, int i) {
            return std::to_string(i * p.first) + '-' + p.second;
          }) >>= pipes::push_back(results);

        CHECK(
          results
          == std::vector<std::string>{"10-up", "15-up", "14-down", "21-down"});
      }

      SUBCASE("")
      {
        auto const inputs1 = std::vector<int>{2, 3};
        auto const inputs2 = std::vector<std::string>{"up", "down"};
        auto const inputs3 = std::vector<int>{5, 7};

        auto results = std::vector<std::string>{};

        pipes::cartesian_product(inputs1, inputs2, inputs3) >>=
          pipes::transform([](int i, std::string const& s, int j)
                           { return std::to_string(i * j) + '-' + s; }) >>=
          pipes::push_back(results);

        CHECK(results
              == std::vector<std::string>{"10-up",
                                          "14-up",
                                          "10-down",
                                          "14-down",
                                          "15-up",
                                          "21-up",
                                          "15-down",
                                          "21-down"});
      }
    }

    SUBCASE("combinations with self") {}

    SUBCASE("interleave")
    {
      // todo: should just mix the input sources
    }
    SUBCASE("intersperse")
    {
      // todo: interleaves with a constant, so e.g. a comma for printing
    }
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

        SUBCASE("temporary from function")
        {
          auto const source = []() {
            return pipes::zip(std::vector<int>{1, 2, 3});
          };

          auto target = std::vector<std::tuple<int>>{};

          source() >>= target;

          CHECK(target == std::vector<std::tuple<int>>{{1}, {2}, {3}});
        }

        SUBCASE("string")
        {
          auto const source = std::vector<std::string>{"1", "2", "3"};

          auto target = std::vector<std::tuple<std::string>>{};

          pipes::zip(source) >>= target;

          CHECK(target
                == std::vector<std::tuple<std::string>>{{"1"}, {"2"}, {"3"}});
        }
      }

      SUBCASE("two sources")
      {
        auto const source1 = std::vector<int>{1, 2, 3};
        auto const source2 = std::vector<std::string>{"a", "b", "c"};

        auto target = std::vector<std::tuple<int, std::string>>{};

        pipes::zip(source1, source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, std::string>>{
                {1, "a"},
                {2, "b"},
                {3, "c"}
        });
      }

      SUBCASE("two sources, different lengths")
      {
        auto const source1 = std::vector<int>{1, 2, 3};
        auto const source2 = std::vector<std::string>{"a", "b"};

        auto target = std::vector<std::tuple<int, std::string>>{};

        pipes::zip(source1, source2) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, std::string>>{
                {1, "a"},
                {2, "b"}
        });
      }

      SUBCASE("three sources")
      {
        auto const source1 = std::vector<int>{1, 2, 3};
        auto const source2 = std::vector<std::string>{"a", "b", "c"};
        auto const source3 = std::vector<double>{1.1, 2.2, 3.3};

        auto target = std::vector<std::tuple<int, std::string, double>>{};

        pipes::zip(source1, source2, source3) >>= target;

        CHECK(target
              == std::vector<std::tuple<int, std::string, double>>{
                {1, "a", 1.1},
                {2, "b", 2.2},
                {3, "c", 3.3}
        });
      }

      SUBCASE("unpack tuple into transform")
      {
        auto const source1 = std::vector<int>{1, 2, 3};

        auto target = std::vector<int>{};

        pipes::zip(source1) >>=
          pipes::transform([](int i) { return 2 * i; }) >>= target;

        CHECK(target == std::vector<int>{2, 4, 6});
      }

      SUBCASE("zero sources") {}
      SUBCASE("source with reference types") {}

      SUBCASE("mixed source types")
      {
        auto const source1 = std::vector<int>{1, 2, 3};
        auto source2 = std::map<int, int>{
          {4, 44},
          {5, 55},
          {6, 66}
        };

        auto target = std::vector<std::tuple<int, int>>{};

        pipes::zip(source1, source2) >>= pipes::transform(
          [](int i, auto p) {
            return std::tuple{i, p.second};
          }) >>= pipes::push_back(target);

        CHECK(target
              == std::vector<std::tuple<int, int>>{
                {1, 44},
                {2, 55},
                {3, 66}
        });
      }
    }

    SUBCASE("generic source")
    {
      auto source = std::vector<int>{1, 2, 3};

      auto target = std::vector<int>{};

      pipes::generic_source([&source](auto& sink)
                            { sink.push(source.front()); }) >>=
        pipes::push_back(target);

      CHECK(target == std::vector{1});
    }

    SUBCASE("from istream") {}
  }

  SUBCASE("sinks")
  {
    SUBCASE("push_back")
    {
      SUBCASE("vector")
      {
        auto source = std::vector<int>{1, 2, 3};
        auto sink = std::vector<int>{};

        pipes::for_each(source) >>= pipes::push_back(sink);
        CHECK(sink == source);
      }

      SUBCASE("return value from pipeline")
      {
        auto source = std::vector<int>{1, 2, 3};
        auto sink = std::vector<int>{};

        auto& retval = pipes::for_each(source) >>= pipes::push_back(sink);
        CHECK(&sink == &retval);
      }

      SUBCASE("deque")
      {
        auto source = std::vector<int>{1, 2, 3};
        auto sink = std::deque<int>{};

        pipes::for_each(source) >>= pipes::push_back(sink);
        CHECK(sink == std::deque<int>(source.begin(), source.end()));
      }
      SUBCASE("list")
      {
        auto source = std::vector<int>{1, 2, 3};
        auto sink = std::list<int>{};

        pipes::for_each(source) >>= pipes::push_back(sink);
        CHECK(sink == std::list<int>(source.begin(), source.end()));
      }
    }

    SUBCASE("push_front")
    {
      SUBCASE("deque")
      {
        auto source = std::vector<int>{1, 2, 3};
        auto sink = std::deque<int>{};

        pipes::for_each(source) >>= pipes::push_front(sink);
        CHECK(sink == std::deque<int>{3, 2, 1});
      }

      SUBCASE("return value from pipeline")
      {
        auto source = std::vector<int>{1, 2, 3};
        auto sink = std::deque<int>{};

        auto& retval = pipes::for_each(source) >>= pipes::push_front(sink);
        CHECK(&sink == &retval);
      }

      SUBCASE("list")
      {
        auto source = std::vector<int>{1, 2, 3};
        auto sink = std::list<int>{};

        pipes::for_each(source) >>= pipes::push_front(sink);
        CHECK(sink == std::list<int>{3, 2, 1});
      }
      SUBCASE("forward_list")
      {
        auto source = std::vector<int>{1, 2, 3};
        auto sink = std::forward_list<int>{};

        pipes::for_each(source) >>= pipes::push_front(sink);
        CHECK(sink == std::forward_list<int>{3, 2, 1});
      }
    }

    SUBCASE("discard")
    {
      SUBCASE("generic")
      {
        auto const source = std::vector<int>{1, 2, 3, 4};

        static_assert(
          pipes::CanLink<decltype(source), decltype(pipes::discard())>);
      }

      SUBCASE("typed")
      {
        using SourceSection = std::vector<int>;

        static_assert(
          pipes::CanLink<SourceSection, decltype(pipes::discard<int>())>);

        static_assert(!pipes::CanLink<SourceSection,
                                      decltype(pipes::discard<std::string>())>);
      }

      SUBCASE("typed with tuple")
      {
        using SourceSection = std::vector<std::tuple<int, int>>;

        using Sink1 = decltype(pipes::discard<std::tuple<int, int>>());
        static_assert(pipes::CanLink<SourceSection, Sink1>);

        using Sink2 = decltype(pipes::discard<int, int>());
        static_assert(pipes::CanLink<SourceSection, Sink2>);
      }

      SUBCASE("typed with tuple of tuple (only unpack one level at a time!)")
      {
        using SourceSection = std::vector<std::tuple<std::tuple<int>>>;

        using Sink1 = decltype(pipes::discard<std::tuple<std::tuple<int>>>());
        static_assert(pipes::CanLink<SourceSection, Sink1>);

        using Sink2 = decltype(pipes::discard<std::tuple<int>>());
        static_assert(pipes::CanLink<SourceSection, Sink2>);

        using Sink3 = decltype(pipes::discard<int>());
        static_assert(!pipes::CanLink<SourceSection, Sink3>);
      }

      // todo: typed with types that can be converted to?
    }

    SUBCASE("insert_or_assign")
    {
      SUBCASE("map")
      {
        auto source = std::vector<std::pair<int, std::string>>{
          {1, "1"},
          {2, "2"},
          {3, "3"},
          {3, "4"}
        };
        auto sink = std::map<int, std::string>{};

        pipes::for_each(source) >>= pipes::insert_or_assign(sink);

        // uses insert_or_assign, so the last element pushed in for a
        // particular key is pushed
        CHECK(sink
              == std::map<int, std::string>{
                {1, "1"},
                {2, "2"},
                {3, "4"}
        });
      }
    }

    SUBCASE("insert")
    {
      SUBCASE("set")
      {
        auto source = std::vector{1, 2, 3, 3};
        auto sink = std::set<int>{};

        pipes::for_each(source) >>= pipes::insert(sink);

        CHECK(sink == std::set<int>{1, 2, 3});
      }
      SUBCASE("map")
      {
        auto source = std::vector<std::pair<int, std::string>>{
          {1, "1"},
          {2, "2"},
          {3, "3"},
          {3, "4"}
        };
        auto sink = std::map<int, std::string>{};

        pipes::for_each(source) >>= pipes::insert(sink);

        // uses insert, so only the first element for a particular key is
        // actually inserted.
        CHECK(sink
              == std::map<int, std::string>{
                {1, "1"},
                {2, "2"},
                {3, "3"}
        });
      }
    }

    SUBCASE("to_iterator")
    {
      SUBCASE("")
      {
        auto const source = std::vector{1, 2, 3};
        auto sink = std::vector<int>{4, 5, 6};

        pipes::for_each(source) >>= pipes::to_iterator(sink.begin());

        CHECK(sink == source);
      }

      SUBCASE("")
      {
        auto const source = std::vector{1, 2, 3};
        auto sink = std::vector<int>{};

        pipes::for_each(source) >>=
          pipes::to_iterator(std::back_inserter(sink));

        CHECK(sink == source);
      }

      SUBCASE("")
      {
        auto const source = std::vector<std::string>{"1", "2", "3"};
        auto sink = std::vector<std::string>{};

        pipes::for_each(source) >>=
          pipes::to_iterator(std::back_inserter(sink));

        CHECK(sink == source);
      }
      SUBCASE("")
      {
        auto const source = std::vector{1, 2, 3};
        auto sink = std::vector<int>{4, 5, 6};

        auto it = pipes::for_each(source) >>= pipes::to_iterator(sink.begin());

        CHECK(it == sink.end());
      }
    }

    SUBCASE("set_aggregator")
    {
      SUBCASE("")
      {
        auto const source = std::vector<int>{1, 2};
        auto sink = std::set<int>{};

        pipes::for_each(source) >>= pipes::set_aggregator(sink, std::plus{});

        CHECK(sink == std::set<int>{1, 2});
      }

      SUBCASE("")
      {
        auto const source = std::vector<int>{1, 3};
        auto sink = std::set<int>{1};

        pipes::for_each(source) >>= pipes::set_aggregator(sink, std::plus{});

        CHECK(sink == std::set<int>{1 + 1, 3});
      }

      SUBCASE("")
      {
        auto const source = std::vector<int>{1};
        auto sink = std::set<int>{1, 2};

        pipes::for_each(source) >>= pipes::set_aggregator(sink, std::plus{});

        CHECK(sink == std::set<int>{1 + 1 + 2});
      }
    }
    SUBCASE("map_aggregator")
    {
      SUBCASE("")
      {
        auto const source = std::vector<std::pair<int, int>>{
          {1, 1},
          {2, 2}
        };
        auto sink = std::map<int, int>{};

        pipes::for_each(source) >>= pipes::map_aggregator(sink, std::plus{});

        CHECK(sink
              == std::map<int, int>{
                {1, 1},
                {2, 2}
        });
      }

      SUBCASE("")
      {
        auto const source = std::vector<std::pair<int, int>>{
          {1, 1},
          {2, 5},
          {3, 3},
          {3, 4}
        };
        auto sink = std::map<int, int>{
          {2, 2}
        };

        pipes::for_each(source) >>= pipes::map_aggregator(sink, std::plus{});

        CHECK(sink
              == std::map<int, int>{
                {1,     1},
                {2, 2 + 5},
                {3, 3 + 4}
        });
      }
    }

    SUBCASE("to ostream") {}

    SUBCASE("generic sink ")
    {
      const auto source = std::vector{1, 2, 3};
      auto sink = std::vector<int>{};

      pipes::for_each(source) >>=
        pipes::generic_sink([&sink](int i) { sink.push_back(i); });

      CHECK(sink == source);
    }

    SUBCASE("reduce") {}

    SUBCASE("defaults")
    {
      SUBCASE("vector default is push_back")
      {
        auto const source = std::vector{1, 2, 3};
        auto sink = std::vector<int>{};

        pipes::for_each(source) >>= sink;
        CHECK(sink == source);
      }

      SUBCASE("map default is insert_or_assign")
      {
        auto source = std::vector<std::pair<int, std::string>>{
          {1, "1"},
          {2, "2"},
          {3, "3"},
          {3, "4"}
        };
        auto sink = std::map<int, std::string>{};

        pipes::for_each(source) >>= sink;

        // uses insert_or_assign, so the last element pushed in for a
        // particular key is pushed
        CHECK(sink
              == std::map<int, std::string>{
                {1, "1"},
                {2, "2"},
                {3, "4"}
        });
      }
    }
  }

  SUBCASE("use as output iterator for std::algorithms")
  {
    // todo:
    // std::copy(v.begin(), v.end(), pipes::output_iterator() >>=
    // pipes::transform(...) >>= target);
  }

  // todo: allow temporaries as targets, to be returned in the end
  //
  // todo: different operators, including <<= >>= << >> |
  //
  // todo: move-only types
  //
  // todo: big types with lots of data (do not have to copy everything all the
  // time)
  //
  // todo: streams
  //
  // todo: generators and take-from for generator with effectively infinite
  // sources
  //
  // todo: make work with tuples! pipelines for tuples! that would be cool and
  // useful
  //
  // todo: make examples that use the single-header generated from the headers
  // and links with cmake
}

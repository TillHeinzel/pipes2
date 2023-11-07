#include <doctest/doctest.h>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("chained operations")
{
  auto isOdd = [](int i) { return i % 2 == 1; };
  auto isNotEmpty = [](std::string const& s) { return !s.empty(); };
  auto isSame = [](auto const& lhs, auto const& rhs) { return lhs == rhs; };
  auto isNotSame = [](auto const& lhs, auto const& rhs) { return lhs != rhs; };

  auto timesTwo = [](int i) { return 2 * i; };
  auto addOne = [](int i) { return i + 1; };
  auto addSemicolon = [](std::string s) { return s + ";"; };

  SUBCASE("filter transform")
  {
    CHECK((source{1, 2, 3, 4, 5} >> pipes::filter(isOdd)
           >> pipes::transform(timesTwo) >> sink{})
          == vals{2, 6, 10});
  }

  SUBCASE("prepare open sink")
  {
    auto sinkChain =
      pipes::filter(isOdd) >> pipes::transform(timesTwo) >> sink{};

    CHECK((source{1, 2, 3, 4, 5} >> sinkChain) //
          == vals{2, 6, 10});
  }

  SUBCASE("prepare operations-chain with 2 ops")
  {
    auto chain = pipes::filter(isOdd) >> pipes::transform(timesTwo);

    CHECK((source{1, 2, 3, 4, 5} >> chain >> sink{}) //
          == vals{2, 6, 10});
  }

  SUBCASE("prepare operations-chain with 3 ops")
  {
    auto chain = pipes::filter(isOdd) >> pipes::transform(timesTwo)
                 >> pipes::transform(addOne);

    CHECK((source{1, 2, 3, 4, 5} >> chain >> sink{}) //
          == vals{3, 7, 11});
  }

  SUBCASE("prepare src without sink")
  {
    SUBCASE("int")
    {
      auto chain = pipes::filter(isOdd) >> pipes::transform(timesTwo)
                   >> pipes::transform(addOne);

      auto const src = source{1, 2, 3, 4, 5} >> chain;

      CHECK((src >> sink{}) //
            == vals{3, 7, 11});
    }
    SUBCASE("string")
    {
      auto chain = pipes::filter(isNotEmpty) >> pipes::transform(addSemicolon);

      auto const src = source{"1", "", "3"} >> chain;

      CHECK((src >> sink{}) //
            == vals{"1;", "3;"});
    }
  }

  SUBCASE("prepare src without sink in multiple steps")
  {
    auto const orig = source{1, 2, 3, 4, 5};

    auto const source1 =
      orig >> pipes::filter(isOdd) >> pipes::transform(timesTwo);

    auto const src = source1 >> pipes::transform(addOne);

    CHECK((src >> sink{}) == vals{3, 7, 11});
  }

  SUBCASE("")
  {
    using t = std::tuple<int, int>;

    auto const source1 = source{1, 2, 3, 6};
    auto const source2 = source{3, 2, 7, 6};

    SUBCASE("prepare multi-parameter src without sink")
    {
      auto src = pipes::zip(source1, source2) >> pipes::filter(isSame);

      CHECK((src >> sink{}) == vals{t{2, 2}, t{6, 6}});
    }

    SUBCASE("prepare multi-parameter sink without src")
    {
      auto sink_ = pipes::filter(isSame) >> sink{};

      CHECK((pipes::zip(source1, source2) >> sink_) //
            == vals{t{2, 2}, t{6, 6}});
    }

    SUBCASE("zip filter transform")
    {
      CHECK((pipes::zip(source1, source2) >> pipes::filter(isNotSame)
             >> pipes::transform(std::plus{}) >> sink{})
            == vals{4, 10});
    }
  }
}

#include <doctest/doctest.h>

#include <string>
#include <tuple>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("switch")
{
  auto isEven = [](int i) { return i % 2 == 0; };
  auto isMultipleOf3 = [](int i) { return i % 3 == 0; };
  auto isMultipleOf4 = [](int i) { return i % 4 == 0; };

  auto doubleIt = pipes::transform([](int i) { return 2 * i; });

  auto rest = sink{};
  auto s1 = sink{};
  auto s2 = sink{};

  SUBCASE("")
  {
    source{1, 2, 3, 4} >> pipes::switch_(pipes::default_ >> rest);

    CHECK(rest == vals{1, 2, 3, 4});
  }

  SUBCASE("")
  {
    source{1, 2, 3, 4} >> pipes::switch_(pipes::case_(isEven) >> s1);

    CHECK(s1 == vals{2, 4});
  }

  SUBCASE("simple case with chained ops")
  {
    source{1, 2, 3, 4}
      >> pipes::switch_(pipes::case_(isEven) >> doubleIt >> s1);

    CHECK(s1 == vals{4, 8});
  }

  SUBCASE("2 cases")
  {
    source{1, 2, 3, 4, 5, 6, 7, 8}
      >> pipes::switch_(pipes::case_(isMultipleOf3) >> s1,
                        pipes::case_(isMultipleOf4) >> s2);

    CHECK(s1 == vals{3, 6});
    CHECK(s2 == vals{4, 8});
  }

  SUBCASE("2 cases with default")
  {
    source{1, 2, 3, 4, 5, 6, 7, 8}
      >> pipes::switch_(pipes::case_(isMultipleOf3) >> s1,
                        pipes::case_(isMultipleOf4) >> s2,
                        pipes::default_ >> rest);

    CHECK(s1 == vals{3, 6});
    CHECK(s2 == vals{4, 8});
    CHECK(rest == vals{1, 2, 5, 7});
  }

  SUBCASE("other type")
  {
    source{"1", "___", "22", "a", "aa", "_", "xxxx"} >> pipes::switch_(
      pipes::case_([](std::string s) { return s.size() == 1; }) >> s1,
      pipes::case_([](std::string s) { return s.size() == 2; }) >> s2,
      pipes::default_ >> rest);

    CHECK(s1 == vals{"1", "a", "_"});
    CHECK(s2 == vals{"22", "aa"});
    CHECK(rest == vals{"___", "xxxx"});
  }

  SUBCASE("multiple types")
  {
    auto sumLargerThan4 = [](int i, int j) { return i + j > 4; };

    using t = std::tuple<int, int>;

    source{t{1, 2}, t{3, 4}} //
      >> pipes::switch_(pipes::case_(sumLargerThan4) >> s1);

    CHECK(s1 == vals{t{3, 4}});
  }

  SUBCASE("case with (left-to-right) >>")
  {
    auto c1 = pipes::case_(isEven) >> doubleIt;
    auto c = c1 >> s1;

    source{1, 2, 3, 4} >> pipes::switch_(c);

    CHECK(s1 == vals{4, 8});
  }

  SUBCASE("")
  {
    auto taken1 = unique_sink();
    auto taken2 = unique_sink();
    auto rest = unique_sink();

    unique_source(1, 2, 3, 4, 5, 6, 7, 8)
      >> pipes::switch_(pipes::case_(unique(isMultipleOf3)) >> taken1,
                        pipes::case_(unique(isMultipleOf4)) >> taken2,
                        pipes::default_ >> rest);

    CHECK(taken1 == vals{3, 6});
    CHECK(taken2 == vals{4, 8});
    CHECK(rest == vals{1, 2, 5, 7});
  }
}

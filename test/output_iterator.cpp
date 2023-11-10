#include <doctest/doctest.h>

#include <algorithm>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("output_iterator")
{
  SUBCASE("")
  {
    auto const from = std::vector{1, 2, 3, 4};

    auto to = std::vector<int>{};

    std::copy(from.begin(), from.end(), pipes::output_iterator() >> to);

    CHECK(to == vals{1, 2, 3, 4});
  }

  SUBCASE("")
  {
    auto const from = std::vector{1, 2, 3, 4};

    auto to = std::vector<int>{};

    std::copy(from.begin(),
              from.end(),
              pipes::output_iterator() >> pipes::push_back(to));

    CHECK(to == vals{1, 2, 3, 4});
  }

  SUBCASE("")
  {
    auto const from = std::vector{1, 2, 3, 4};

    auto to = std::vector<int>{};

    std::copy(from.begin(),
              from.end(),
              pipes::output_iterator()
                >> pipes::filter([](auto i) { return i % 2 == 0; })
                >> pipes::transform([](auto i) { return 2 * i; })
                >> pipes::push_back(to));

    CHECK(to == vals{4, 8});
  }

  SUBCASE("")
  {
    auto const from = std::vector{1, 2, 3, 4};

    auto to = std::vector<int>{};
    auto it = pipes::output_iterator()
              >> pipes::filter([](auto i) { return i % 2 == 0; })
              >> pipes::transform([](auto i) { return 2 * i; })
              >> pipes::push_back(to);

    std::copy(from.begin(), from.end(), it);

    CHECK(to == vals{4, 8});
  }

  SUBCASE("")
  {
    auto const from = source{"1", "2", "3"};

    auto to = std::vector<std::string>{};

    std::copy(from.begin(), from.end(), pipes::output_iterator() >> to);

    CHECK(to == vals{"1", "2", "3"});
  }
}

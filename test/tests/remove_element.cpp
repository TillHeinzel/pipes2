#include "doctest.h"

#include <string>
#include <tuple>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("remove_element")
{
  auto s1 = sink{};
  auto s2 = sink{};
  auto s3 = sink{};

  SUBCASE("")
  {
    using t = std::tuple<int, int>;

    source{t{1, 2}, t{3, 4}, t{5, 6}} //
      >> pipes::remove_element(s1)    //
      >> pipes::push_back(s2);

    CHECK(s1 == vals{1, 3, 5});
    CHECK(s2 == vals{2, 4, 6});
  }

  SUBCASE("")
  {
    using t = std::tuple<int, int, int>;

    source{t{1, 2, 3}, t{4, 5, 6}, t{7, 8, 9}} //
      >> pipes::remove_element(s1)             //
      >> pipes::remove_element(s2)             //
      >> pipes::push_back(s3);

    CHECK(s1 == vals{1, 4, 7});
    CHECK(s2 == vals{2, 5, 8});
    CHECK(s3 == vals{3, 6, 9});
  }

  SUBCASE("")
  {
    using t = std::tuple<int, int, int>;

    source{t{1, 2, 3}, t{4, 5, 6}, t{7, 8, 9}} //
      >> pipes::remove_elements(s1, s2)        //
      >> pipes::push_back(s3);

    CHECK(s1 == vals{1, 4, 7});
    CHECK(s2 == vals{2, 5, 8});
    CHECK(s3 == vals{3, 6, 9});
  }

  SUBCASE("")
  {
    using t = std::tuple<int, int, int>;

    source{t{1, 2, 3}, t{4, 5, 6}, t{7, 8, 9}} //
      >> pipes::remove_elements(s1, s2)        //
      >> pipes::push_back(s3);

    CHECK(s1 == vals{1, 4, 7});
    CHECK(s2 == vals{2, 5, 8});
    CHECK(s3 == vals{3, 6, 9});
  }

  SUBCASE("")
  {
    using t = std::tuple<int, int, int>;

    source{t{1, 2, 3}, t{4, 5, 6}, t{7, 8, 9}} //
      >> pipes::remove_element(s1)             //
      >> pipes::push_back(s3);

    using tt = std::tuple<int, int>;
    CHECK(s1 == vals{1, 4, 7});
    CHECK(s3 == vals{tt{2, 3}, tt{5, 6}, tt{8, 9}});
  }
}

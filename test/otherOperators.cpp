#include <doctest/doctest.h>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("different operators")
{
  auto timesTwo = pipes::transform([](int i) { return 2 * i; });
    auto plus1 = pipes::transform([](int i) { return i + 1; });

  SUBCASE(">>")
  {
    CHECK((source{1, 2, 3} >> timesTwo >> sink{}) //
          == vals{2, 4, 6});


    CHECK((source{1, 2, 3} >> timesTwo >> plus1 >> sink{}) //
          == vals{3, 5, 7});
  }

  SUBCASE(">>=")
  {
    CHECK((source{1, 2, 3} >>= timesTwo >>= sink{}) //
          == vals{2, 4, 6});

    CHECK((source{1, 2, 3} >>= timesTwo >>= plus1 >> sink{}) //
          == vals{3, 5, 7});
  }

  SUBCASE("<<")
  {
    CHECK((sink{} << timesTwo << source{1, 2, 3}) //
          == vals{2, 4, 6});

    CHECK((sink{} << plus1 << timesTwo << source{1, 2, 3}) //
          == vals{3, 5, 7});
  }
  
  SUBCASE("<<=")
  {
    CHECK((sink{} <<= timesTwo <<= source{1, 2, 3}) //
          == vals{2, 4, 6});

    CHECK((sink{} <<= plus1 <<= timesTwo <<= source{1, 2, 3}) //
          == vals{3, 5, 7});
  }
}

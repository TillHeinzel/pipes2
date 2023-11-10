#include <doctest/doctest.h>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("mix_in")
{
  CHECK((pipes::for_each(source{}) >> //
         pipes::mix_in(source{}) >> sink{})
        == vals{});

  CHECK((pipes::for_each(source{1}) >> //
         pipes::mix_in(source{}) >> sink{})
        == vals{});

  CHECK((pipes::for_each(source{}) >> //
         pipes::mix_in(source{1}) >> sink{})
        == vals{});

  CHECK((pipes::for_each(source{1}) >> //
         pipes::mix_in(source{2}) >> sink{})
        == vals{1, 2});

  CHECK((pipes::for_each(source{1, 2}) >> //
         pipes::mix_in(source{3}) >> sink{})
        == vals{1, 3});

  CHECK((pipes::for_each(source{1}) >> //
         pipes::mix_in(source{3, 4}) >> sink{})
        == vals{1, 3});

  CHECK((pipes::for_each(source{1, 3}) >> //
         pipes::mix_in(source{2, 4}) >> sink{})
        == vals{1, 2, 3, 4});

  CHECK((pipes::for_each(source{"1", "3"}) >> //
         pipes::mix_in(source{"2", "4"}) >> sink{})
        == vals{"1", "2", "3", "4"});

  CHECK((pipes::for_each(source{1, 4}) >>  //
         pipes::mix_in(source{2, 5}, 1) >> //
         pipes::mix_in(source{3, 6}, 2) >> sink{})
        == vals{1, 2, 3, 4, 5, 6});

  CHECK((pipes::for_each(source{1, 5}) >>  //
         pipes::mix_in(source{2, 6}, 1) >> //
         pipes::mix_in(source{3, 7}, 2) >> //
         pipes::mix_in(source{4, 8}, 3) >> //
         sink{})
        == vals{1, 2, 3, 4, 5, 6, 7, 8});

  auto makeMixIn = [](auto... xs) { return pipes::mix_in(source{xs...}); };

  CHECK((pipes::for_each(source{1, 3}) >> //
         makeMixIn(2, 4) >> sink{})
        == vals{1, 2, 3, 4});

  CHECK((pipes::for_each(unique_source(1, 3)) >> //
         pipes::mix_in(unique_source(2, 4)) >> unique_sink())
        == vals{1, 2, 3, 4});
}

TEST_CASE("interleave")
{
  CHECK((pipes::interleave(source{}) >> sink{}) //
        == vals{});

  CHECK((pipes::interleave(source{1, 2, 3, 4, 5}) >> sink{}) //
        == vals{1, 2, 3, 4, 5});

  CHECK((pipes::interleave(source{1, 3}, source{2, 4}) >> sink{}) //
        == vals{1, 2, 3, 4});

  CHECK((pipes::interleave(source{"1", "3"}, source{"2", "4"}) >> sink{}) //
        == vals{"1", "2", "3", "4"});

  CHECK(
    (pipes::interleave(source{1, 4}, source{2, 5}, source{3, 6}) >> sink{}) //
    == vals{1, 2, 3, 4, 5, 6});
}

#include <doctest/doctest.h>

#include <list>
#include <string>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("add_all")
{
  using t = std::tuple<int, int>;
  using tt = std::tuple<int, std::string>;
  using ts = std::tuple<std::string, std::string>;
  using ttt = std::tuple<int, int, int>;
  using list = std::list<std::string>;

  CHECK((pipes::for_each(source{}) >> //
         pipes::add_all(source{}) >> sink{})
        == vals{});

  CHECK((pipes::for_each(source{1}) >> //
         pipes::add_all(source{}) >> sink{})
        == vals{});

  CHECK((pipes::for_each(source{}) >> //
         pipes::add_all(source{1}) >> sink{})
        == vals{});

  CHECK((pipes::for_each(source{1}) >> //
         pipes::add_all(source{4}) >> sink{})
        == vals{t{1, 4}});

  CHECK((pipes::for_each(source{1}) >> //
         pipes::add_all(source{4, 5}) >> sink{})
        == vals{t{1, 4}, t{1, 5}});

  CHECK((pipes::for_each(source{1}) >> //
         pipes::add_all(source{"4", "5"}) >> sink{})
        == vals{tt{1, "4"}, tt{1, "5"}});

  CHECK((pipes::for_each(source{"1"}) >> //
         pipes::add_all(list{"4", "5"}) >> sink{})
        == vals{ts{"1", "4"}, ts{"1", "5"}});

  CHECK((pipes::for_each(source{1, 2}) >> //
         pipes::add_all(source{3, 4}) >>  //
         pipes::add_all(source{5, 6}) >> sink{})
        == vals{
          ttt{1, 3, 5},
          ttt{1, 3, 6},
          ttt{1, 4, 5},
          ttt{1, 4, 6},
          ttt{2, 3, 5},
          ttt{2, 3, 6},
          ttt{2, 4, 5},
          ttt{2, 4, 6},
        });

  auto makeAddAll = [](auto... xs) { return pipes::add_all(source{xs...}); };

  CHECK((pipes::for_each(source{1}) >> //
         makeAddAll(4) >> sink{})
        == vals{t{1, 4}});
}

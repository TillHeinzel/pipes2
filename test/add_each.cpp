#include <doctest/doctest.h>

#include <list>
#include <string>
#include <tuple>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("add_each")
{
  using t = std::tuple<int, int>;
  using tt = std::tuple<int, std::string>;
  using ttt = std::tuple<int, int, int>;
  using list_t = std::list<int>;

  CHECK((pipes::for_each(source{}) >>         //
         pipes::add_each(source{}) >> sink{}) //
        == vals{});

  CHECK((pipes::for_each(source{1}) >> //
         pipes::add_each(source{}) >> sink{})
        == vals{});

  CHECK((pipes::for_each(source{}) >>          //
         pipes::add_each(source{1}) >> sink{}) //
        == vals{});

  CHECK((pipes::for_each(source{1}) >>         //
         pipes::add_each(source{3}) >> sink{}) //
        == vals{t{1, 3}});

  CHECK((pipes::for_each(source{1, 2}) >>      //
         pipes::add_each(source{3}) >> sink{}) //
        == vals{t{1, 3}});

  CHECK((pipes::for_each(source{1}) >>            //
         pipes::add_each(source{3, 4}) >> sink{}) //
        == vals{t{1, 3}});

  CHECK((pipes::for_each(source{1, 2}) >>         //
         pipes::add_each(source{3, 4}) >> sink{}) //
        == vals{t{1, 3}, t{2, 4}});

  CHECK((pipes::for_each(source{1, 2}) >>         //
         pipes::add_each(list_t{3, 4}) >> sink{}) //
        == vals{t{1, 3}, t{2, 4}});

  CHECK((pipes::for_each(source{1, 2}) >>             //
         pipes::add_each(source{"3", "4"}) >> sink{}) //
        == vals{tt{1, "3"}, tt{2, "4"}});

  CHECK((pipes::for_each(source{1, 2}) >>         //
         pipes::add_each(list_t{3, 4}) >>         //
         pipes::add_each(list_t{5, 6}) >> sink{}) //
        == vals{ttt{1, 3, 5}, ttt{2, 4, 6}});

  CHECK((pipes::for_each(source{1, 2, 2, 2, 2}) >>      //
         pipes::add_each(list_t{3, 4}) >>               //
         pipes::add_each(list_t{5, 6, 6, 6}) >> sink{}) //
        == vals{ttt{1, 3, 5}, ttt{2, 4, 6}});

  auto const makeAddEach = [](auto... xs)
  { return pipes::add_each(source{xs...}); };

  CHECK((pipes::for_each(source{1})   //
         >> makeAddEach(3) >> sink{}) //
        == vals{t{1, 3}});
}

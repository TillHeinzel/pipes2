#include "doctest.h"

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/sink.hpp"
#include "support/source.hpp"
#include "support/test_streaming.hpp"

TEST_CASE("zip")
{
  {
    using res = std::vector<std::tuple<int>>;

    CHECK((pipes::zip(source{1, 2, 3}) >> res{}) //
          == res{{1}, {2}, {3}});
  }

  {
    using res = std::vector<std::tuple<std::string>>;

    CHECK((pipes::zip(source{"1", "2", "3"}) >> res{}) //
          == res{{"1"}, {"2"}, {"3"}});
  }
  {
    using res = std::vector<std::tuple<int, std::string>>;

    CHECK((pipes::zip(source{1, 2, 3}, source{"a", "b", "c"}) >> res{}) //
          == res{{1, "a"}, {2, "b"}, {3, "c"}});

    CHECK((pipes::zip(source{1, 2, 3}, source{"a", "b"}) >> res{}) //
          == res{{1, "a"}, {2, "b"}});
  }
  {
    using res = std::vector<std::tuple<int, std::string, double>>;

    CHECK(
      (pipes::zip(source{1, 2, 3}, source{"a", "b", "c"}, source{1.1, 2.2, 3.3})
       >> res{}) //
      == res{{1, "a", 1.1}, {2, "b", 2.2}, {3, "c", 3.3}});
  }

  {
    CHECK((pipes::zip(source{1, 2, 3})
           >> pipes::transform([](int i) { return 2 * i; }) >> sink{})
          == vals{2, 4, 6});
  }

  {
    auto replaceKey = [](auto p, int i) { return std::tuple{i, p.second}; };

    using m = std::map<int, int>;
    using t = std::tuple<int, int>;

    CHECK((pipes::zip(m{{4, 44}, {5, 55}, {6, 66}}, source{1, 2, 3})
           >> pipes::transform(replaceKey) >> sink{})
          == vals{t{1, 44}, t{2, 55}, t{3, 66}});
  }

  {
    auto const makeZip = []() { return pipes::zip(std::vector<int>{1, 2, 3}); };

    using res = std::vector<std::tuple<int>>;

    CHECK((makeZip() >> res{}) == res{{1}, {2}, {3}});
  }

  {
    auto source1 = std::map<int, int>{{1, 11}, {2, 22}, {3, 33}};
    auto source2 = std::map<int, int>{{4, 44}, {5, 55}, {6, 66}};

    auto target = std::vector<std::tuple<int, int>>{};

    auto replaceValue = pipes::transform(
      [](auto p1, auto p2) {
        return std::tuple{p1.first, p2.second};
      });

    CHECK((pipes::zip(source1, source2) >> replaceValue
           >> sink{}) //
          == std::vector<std::tuple<int, int>>{{1, 44}, {2, 55}, {3, 66}});
  }
}

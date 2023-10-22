#include "doctest.h"

#include <vector>

namespace pipes
{
  class Output
  {
  };

  Output operator>>=(const std::vector<int>&, Output) { return {}; }

  Output operator>>=(Output, std::vector<int>&) { return {}; }

  template<class T>
  Output transform(T)
  {
    return {};
  };
} // namespace pipes

TEST_CASE("test")
{
  SUBCASE("transform")
  {
    auto const source = std::vector<int>{};

    auto target = std::vector<int>{};

    source >>= pipes::transform([](int i) { return 2 * i; }) >>= target;
  }
}

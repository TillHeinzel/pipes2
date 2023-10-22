#include "doctest.h"

#include <vector>
#include <string>

#include "pipes.hpp"

TEST_CASE("test")
{
  SUBCASE("transform")
  {
    SUBCASE("Empty")
    {
      auto const source = std::vector<int>{};

      auto target = std::vector<int>{};

      source >>= pipes::transform([](int i) { return 2 * i; }) >>= target;
    }

    SUBCASE("one int") 
    {

    }

    SUBCASE("Invalid input parameter does not work")
    {

    }
  }
}

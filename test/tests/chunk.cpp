#include "doctest.h"

#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "pipes/pipes.hpp"

#include "support/test_streaming.hpp"

TEST_CASE("pipes")
{
  SUBCASE("chunk")
  {
    // todo: chunking the input into chunks of size N

    // todo: need to be able to define the chunk-type, e.g. std::vector or
    // std::map, with certain defaults possible
  }
}

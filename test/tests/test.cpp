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

#include "test_streaming.hpp"

TEST_CASE("test")
{
  SUBCASE("use as output iterator for std::algorithms")
  {
    // todo:
    // std::copy(v.begin(), v.end(), pipes::output_iterator() >>=
    // pipes::transform(...) >>= target);
  }

  // todo: different operators, including <<= >>= << >> |
  //
  // todo: move-only types
  //
  // todo: big types with lots of data (do not have to copy everything all the
  // time)
  //
  // todo: streams
  //
  // todo: generators and take-from for generator with effectively infinite
  // sources
  //
  // todo: make work with tuples! pipelines for tuples! that would be cool and
  // useful
  //
  // todo: make examples that use the single-header generated from the headers
  // and links with cmake
}

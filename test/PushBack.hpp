#pragma once

#include <vector>

#include "impl.hpp"

namespace pipes
{
  struct PushBackSink
  {
    static constexpr bool isOpenSink = true;

    std::vector<int>& v;

    void push(int i) { v.push_back(i); }
  };
}

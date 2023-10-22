#pragma once

#include <vector>

namespace pipes
{
  struct PushBackSink
  {
    std::vector<int>& v;

    void push(int i) { v.push_back(i); }
  };
}
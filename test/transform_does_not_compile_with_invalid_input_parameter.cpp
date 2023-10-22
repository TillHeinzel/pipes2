#include "pipes.hpp"

#include <vector>
#include <string>

void f() 
{
  auto const source = std::vector<int>{};

  auto target = std::vector<int>{};

  source >>= pipes::transform([](std::string s) { return 0; });
}

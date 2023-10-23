#include "pipes.hpp"

#include <string>
#include <vector>

void f()
{
  auto const source = std::vector<int>{0};

  auto target = std::vector<int>{};

  source >>= pipes::transform([](std::string s) -> int { return 0; });
}

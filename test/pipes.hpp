
#include <concepts>
#include <vector>

namespace pipes
{
  class Output
  {
  };

  Output operator>>=(const std::vector<int>&, Output) { return {}; }

  Output operator>>=(Output, std::vector<int>&) { return {}; }

  template<typename T>
  Output transform(T)
    requires std::invocable<T, int> &&
             std::same_as<std::invoke_result_t<T, int>, int>
  {
    return {};
  };
} // namespace pipes

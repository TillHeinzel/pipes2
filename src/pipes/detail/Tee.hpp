#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class Side>
  struct Tee
  {
    Side v;

    auto push(auto& next, auto t)
    {
      v.push(t);
      next.push(std::move(t));
    }
  };

} // namespace pipes::detail

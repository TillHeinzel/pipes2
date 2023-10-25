#pragma once

#include "../GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F>
  struct Transform
  {
    F f;

    auto push(auto& next, auto&&... ts)
      PIPES_RETURN(next.push(f(PIPES_FWD(ts)...)));
  };

  namespace api
  {
    template<typename F>
    auto transform(F f) PIPES_RETURN(pipe(Transform{f}))
  } // namespace api
} // namespace pipes::detail

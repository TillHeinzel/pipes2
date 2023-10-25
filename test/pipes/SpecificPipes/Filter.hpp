#pragma once

#include "../GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F>
  struct Filter
  {
    F f;

    auto push(auto& next, auto&&... t)
      PIPES_RETURN(f(t...) ? next.push(PIPES_FWD(t)...) : void());
  };

  namespace api
  {
    template<typename F>
    auto filter(F f) PIPES_RETURN(Section{Filter{f}});
  } // namespace api
} // namespace pipes::detail
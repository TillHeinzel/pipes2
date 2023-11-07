#pragma once

#include <concepts>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{

  template<class F>
  struct Case
  {
    F f;
  };

  template<class F, class S>
  struct CaseSink
  {
    F f;
    S sink;

    template<class... T>
    auto check(T const&... t) PIPES_RETURN(f(t...));

    template<class... T>
    auto push(T&&... t) PIPES_RETURN(sink.push(PIPES_FWD(t)...));
  };

  template<class... Cases>
  struct Switch
  {
    std::tuple<Cases...> cases;

    template<class... T>
      requires(SinkFor<Cases, T...> && ...)
    void push(T&&... t)
    {
      auto checkPush = [&t...](auto&& c) {
        if(c.check(t...))
        {
          c.push(PIPES_FWD(t)...);
          return true;
        }
        return false;
      };

      callFirstSuccess(cases, checkPush);
    }
  };

} // namespace pipes::detail
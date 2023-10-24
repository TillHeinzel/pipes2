#pragma once

#include "../GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F>
  struct Transform
  {
    F f;

    template<class T>
    void push(detail::SinkFor<std::invoke_result_t<F, T>> auto& next, T t)
    {
      next.push(f(t));
    }
  };

  namespace api
  {
    template<typename F>
    auto transform(F f) PIPES_RETURN(RawNodes{Transform{f}})
  } // namespace api
} // namespace pipes::detail

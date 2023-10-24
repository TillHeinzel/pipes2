#pragma once

#include "../GenericImplementation/impl.hpp"

namespace pipes::detail
{
  struct Discard
  {
    void push(auto const&) {}
  };

  template<class... T>
  struct TypedDiscard
  {
    void push(T const&...) {}
  };

  namespace api
  {
    auto discard() PIPES_RETURN(Sink{Discard{}});

    template<class... T>
    auto discard() PIPES_RETURN(Sink{TypedDiscard<T...>{}});
  } // namespace api
} // namespace pipes::detail

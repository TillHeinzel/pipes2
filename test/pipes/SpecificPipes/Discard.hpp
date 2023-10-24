#pragma once

#include "../GenericImplementation/impl.hpp"

namespace pipes::detail
{
  struct Discard
  {
    void push(auto const&) {}
  };

  namespace api
  {
    auto discard() PIPES_RETURN(Sink{Discard{}});
  } // namespace api
} // namespace pipes::detail

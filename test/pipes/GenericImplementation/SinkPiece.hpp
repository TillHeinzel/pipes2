#pragma once

#include "../Utility/FWD.hpp"
#include "../Utility/RETURN.hpp"

#include "Node.hpp"

namespace pipes::detail
{
  template<class S>
  struct SinkPiece
  {
    S actualSink;

    template<class... Ts>
      requires(SinkFor<S, Ts...>)
    auto operator()(Ts&&... ts) PIPES_RETURN(actualSink.push(PIPES_FWD(ts)...));

    template<class... Ts>
      requires(!SinkFor<S, Ts...>)
    auto operator()(Ts&&... ts)
      PIPES_RETURN(actualSink.push(std::tuple{PIPES_FWD(ts)...}));

    template<class... Ts>
    auto push(Ts&&... ts) PIPES_RETURN((*this)(PIPES_FWD(ts)...));

    // no recursive reference here, to avoid issues with tuples of tuples being
    // unpacked
    template<class... Ts>
    auto push(std::tuple<Ts...> ts) PIPES_RETURN(std::apply(*this, ts));
  };
} // namespace pipes::detail
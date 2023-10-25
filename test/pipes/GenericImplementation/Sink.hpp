#pragma once

#include <tuple>

#include "../Utility/FWD.hpp"
#include "../Utility/RETURN.hpp"

#include "Node.hpp"

namespace pipes::detail
{
  template<typename S, class... T>
  concept SinkFor = requires(S s, T... t) { s.push(t...); };

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

  template<class S, class... Ops>
  struct SinkSection
  {
    SinkSection(S finalSink, Section<Ops...> ops) :
      finalSink(PIPES_FWD(finalSink)), ops(PIPES_FWD(ops))
    {
    }

    SinkSection(SinkPiece<S> finalSink, Section<Ops...> ops) :
      finalSink(PIPES_FWD(finalSink)), ops(PIPES_FWD(ops))
    {
    }

    SinkSection(S finalSink) : finalSink(PIPES_FWD(finalSink)), ops() {}

    SinkSection(SinkPiece<S> finalSink) : finalSink(PIPES_FWD(finalSink)), ops() {}

    SinkPiece<S> finalSink;
    Section<Ops...> ops;
  };
} // namespace pipes::detail

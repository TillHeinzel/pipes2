#pragma once

#include "../Utility/RETURN.hpp"
#include "../Utility/tuples.hpp"

#include "BuildingBlocks.hpp"

namespace pipes::detail
{

  template<class X>
  concept hasValue = requires(X x) { x.value(); };

  template<class F>
  struct FlowNode
  {
    F f;

    template<class... Ts>
    auto push(Ts&&... ts) PIPES_RETURN(f(PIPES_FWD(ts)...));

    // no recursive reference here, to avoid issues with tuples of tuples being
    // unpacked
    template<class... Ts>
    auto push(std::tuple<Ts...> ts) PIPES_RETURN(std::apply(f, ts));

    template<class... Ts>
      requires(!std::regular_invocable<F, Ts...>)
    auto push(Ts&&... ts) PIPES_RETURN(f(std::tuple{PIPES_FWD(ts)...}));

    decltype(auto) value()
      requires(hasValue<F>)
    {
      return f.value();
    }
  };

  template<class S>
  struct EndNode
  {
    S s;

    auto operator()(auto&&... ts) PIPES_RETURN(s.push(PIPES_FWD(ts)...));

    decltype(auto) value()
      requires(hasValue<S>)
    {
      return s.value();
    }
  };

  template<class Next, class Op>
  struct PieceNode
  {
    Next next;
    Op op;

    auto operator()(auto&&... ts) PIPES_RETURN(op.push(next, PIPES_FWD(ts)...));

    decltype(auto) value()
      requires(hasValue<Next>)
    {
      return next.value();
    }
  };

  auto endNode(auto s) { return FlowNode{EndNode{s}}; }

  auto pieceNode(auto op, auto next)
  {
    return FlowNode{
      PieceNode{next, op}
    };
  }
} // namespace pipes::detail

namespace pipes::detail
{
  auto connect_to_sink_impl(auto s) -> decltype(s) { return s; }

  template<class Piece, class... Pieces>
  auto connect_to_sink_impl(auto sink, Piece piece, Pieces... pieces)
    PIPES_RETURN(connect_to_sink_impl(pieceNode(piece, sink), pieces...));

  auto connect_to_sink_f(auto s)
  {
    return [s](auto... n) { return connect_to_sink_impl(s, n...); };
  }

  template<class... Pieces>
  auto connect_to_sink(Section<Pieces...> pipe, auto sink)
    PIPES_RETURN(std::apply(connect_to_sink_f(endNode(sink)),
                            reverse(pipe.pieces)));

} // namespace pipes::detail

namespace pipes::detail
{
  template<typename S, class... T>
  concept SinkFor = requires(S s, T... t) { s.push(t...); };

  struct DummySink
  {
    void push(auto&&...) {}
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<typename Source, typename Sink>
  concept CanPushRef =
    requires(Source source, Sink sink) { source.push(sink); };

  template<typename Source, typename Sink>
  concept CanPushMove =
    requires(Source source, Sink sink) { source.push(std::move(sink)); };

  template<typename Source, typename Sink>
  concept ValidConnectedPipeline =
    CanPushRef<Source, Sink> && !CanPushMove<Source, Sink>;
} // namespace pipes::detail

namespace pipes::detail
{
  template<class Source, class Sink>
    requires(ValidConnectedPipeline<Source, Sink>)
  struct ConnectedPipeline
  {
    Source source;
    Sink sink;

    decltype(auto) run() &&
    {
      source.push(sink);
      if constexpr(hasValue<Sink>)
      {
        return sink.value();
      }
    }
  };

  auto connectPipeline(auto p)
    PIPES_RETURN(ConnectedPipeline{p.source, connect_to_sink(p.pipe, p.sink)});

} // namespace pipes::detail

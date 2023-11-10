#pragma once

#include <tuple>

#include "../Utility/HOF.hpp"

#include "BuildingBlocks.hpp"

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
  template<class X>
  concept hasValue = requires(X x) { x.value(); };

  template<class F>
  struct FlowNode
  {
    F f;

    template<class... Ts>
      requires(std::regular_invocable<F, Ts...>)
    void push(Ts&&... ts)
    {
      f(PIPES_FWD(ts)...);
    }

    // no recursive reference here, to avoid issues with tuples of tuples being
    // unpacked
    template<class... Ts>
      requires(std::regular_invocable<F, Ts...>)
    auto push(std::tuple<Ts...> ts)
    {
      std::apply(f, std::move(ts));
    }

    template<class... Ts>
      requires(!std::regular_invocable<F, Ts...>)
    void push(Ts... ts)
    {
      f(std::make_tuple(std::move(ts)...));
    }

    decltype(auto) value()
      requires(hasValue<F>)
    {
      return std::move(f).value();
    }
  };

  template<class S>
  struct EndNode
  {
    S s;

    template<class... Ts>
      requires(SinkFor<S, Ts...>)
    void operator()(Ts&&... ts)
    {
      s.push(PIPES_FWD(ts)...);
    }

    decltype(auto) value()
      requires(hasValue<S>)
    {
      return std::move(s).value();
    }
  };

  template<class Next, class Op>
  struct PieceNode
  {
    Next next;
    Op op;

    void operator()(auto... ts)
    {
      op.push(next, std::move(ts)...);
    }

    decltype(auto) value()
      requires(hasValue<Next>)
    {
      return std::move(next).value();
    }
  };

  auto endNode(auto s) { return FlowNode{EndNode{std::move(s)}}; }

  auto pieceNode(auto op, auto next)
  {
    return FlowNode{PieceNode{std::move(next), std::move(op)}};
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class F>
  auto operator+(auto piece, FlowNode<F> sink)
  {
    return pieceNode(std::move(piece), std::move(sink));
  }

  template<class... Pieces>
  auto connect_to_sink(Section<Pieces...> pipe, auto sink)
  {
    return std::apply([s = endNode(std::move(sink))](auto... n) mutable
                      { return (std::move(n) + ... + std::move(s)); },
                      std::move(pipe).pieces);
  }
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
  struct ConnectedPipeline
  {
    Source source;
    Sink sink;

    decltype(auto) run() &&
    {
      source.push(sink);
      if constexpr(hasValue<Sink>)
      {
        return std::move(sink).value();
      }
    }
  };

  auto connectPipeline(auto p)
  {
    return ConnectedPipeline{std::move(p).source,
                             connect_to_sink(std::move(p).pipe, std::move(p).sink)};
  }

} // namespace pipes::detail

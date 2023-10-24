#pragma once

#include <tuple>

#include "../Utility/FWD.hpp"
#include "../Utility/RETURN.hpp"

#include "Node.hpp"

namespace pipes::detail
{
  template<typename T, class I>
  concept SinkFor = requires(T t, I i) { t.push(i); };

  template<class S>
  struct EndPiece
  {
    S actualSink;

    template<class... Ts>
    auto operator()(Ts&&... ts) PIPES_RETURN(actualSink.push(PIPES_FWD(ts)...));

    template<class Ts>
    auto push(Ts&& ts) PIPES_RETURN((*this)(PIPES_FWD(ts)));

    // no recursive reference here, to avoid issues with tuples of tuples being
    // unpacked
    template<class... Ts>
    auto push(std::tuple<Ts...> ts) PIPES_RETURN(std::apply(*this, ts));
  };

  template<class S, class... Ops>
  struct Sink
  {
    Sink(S finalSink, RawNodes<Ops...> ops) :
      finalSink(PIPES_FWD(finalSink)), ops(PIPES_FWD(ops))
    {
    }

    Sink(EndPiece<S> finalSink, RawNodes<Ops...> ops) :
      finalSink(PIPES_FWD(finalSink)), ops(PIPES_FWD(ops))
    {
    }

    Sink(S finalSink) : finalSink(PIPES_FWD(finalSink)), ops() {}

    Sink(EndPiece<S> finalSink) : finalSink(PIPES_FWD(finalSink)), ops() {}

    EndPiece<S> finalSink;
    RawNodes<Ops...> ops;
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<typename S>
  concept RootSource = requires(S) { typename S::OutputType; };

  struct DummySink
  {
    void push(auto const&) {}
  };

  template<typename Chain, typename T>
  concept ValidChainFor =
    SinkFor<decltype(connect_links(std::declval<Chain>(), DummySink{})), T>;

  template<typename S, typename Ops>
  concept ValidSource =
    RootSource<S> && ValidChainFor<Ops, typename S::OutputType>;

  template<RootSource Root, class... Ops>
  struct Source
  {
    Source(Root root, RawNodes<Ops...> ops)
      requires(ValidSource<Root, RawNodes<Ops...>>)
      : root(root), ops(ops)
    {
    }

    Source(Root root) : root(root), ops() {}

    Root root;
    RawNodes<Ops...> ops;
  };

} // namespace pipes::detail

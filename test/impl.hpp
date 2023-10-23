#pragma once

#include "FWD.hpp"
#include "Node.hpp"

namespace pipes
{
  template<typename T, class I>
  concept SinkFor = requires(T t, I i) { t.push(i); };

  template<class FinalSink, class... Ops>
  struct PSink
  {
    FinalSink finalSink;
    RawNodes<Ops...> ops;

    auto collapse() const { return connect_links(ops, finalSink); }
  };

  template<class S, class T>
  concept ValidSink = SinkFor<decltype(std::declval<S>().collapse()), T>;

} // namespace pipes

namespace pipes
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

    Root root;
    RawNodes<Ops...> ops;
  };

} // namespace pipes

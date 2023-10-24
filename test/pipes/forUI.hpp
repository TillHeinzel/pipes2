#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<RootSource S>
  auto finish_impl(S source, SinkFor<typename S::OutputType> auto sink)
    PIPES_FWD(source.push(sink));

  template<class... Ops>
  auto finish(RootSource auto source, RawNodes<Ops...> ops, auto finalSink)
    PIPES_FWD(finish_impl(source, connect_links(ops, finalSink)))
} // namespace pipes

namespace pipes::detail
{
  template<class... Ops1, class... Ops2>
  auto append(RawNodes<Ops1...> ops1, RawNodes<Ops2...> ops2)
    PIPES_FWD(ops1 + ops2);

  template<class... Ts, class... Ops>
  auto append(Source<Ts...> source, RawNodes<Ops...> laterOps)
    PIPES_FWD(Source{source.root, source.ops + laterOps});

  template<class... Ops, class... Ts>
  auto append(RawNodes<Ops...> earlierOps, Sink<Ts...> sink)
    PIPES_FWD(Sink{sink.finalSink, earlierOps + sink.ops});

  template<class... T1s, class... T2s>
  auto append(Source<T1s...> source, Sink<T2s...> sink)
    PIPES_FWD(finish(source.root, source.ops + sink.ops, sink.finalSink));
} // namespace pipes

namespace pipes::detail
{
  auto collapse(auto const& s) PIPES_FWD(connect_links(s.ops, s.finalSink));

  template<class S, class T>
  concept ValidSink = SinkFor<decltype(collapse(std::declval<S>())), T>;

  template<typename X, typename T>
  concept ValidReceiverFor = ValidChainFor<X, T> || ValidSink<X, T>;
} // namespace pipes
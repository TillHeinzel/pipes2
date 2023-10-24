#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<RootSource S>
  auto finish_impl(S source, SinkFor<typename S::OutputType> auto sink)
    PIPES_RETURN(source.push(sink));

  template<class... Ops>
  auto finish(RootSource auto source, RawNodes<Ops...> ops, auto finalSink)
    PIPES_RETURN(finish_impl(source, connect_links(ops, finalSink)))
} // namespace pipes

namespace pipes::detail
{
  template<class... Ops1, class... Ops2>
  auto link(RawNodes<Ops1...> ops1, RawNodes<Ops2...> ops2)
    PIPES_RETURN(ops1 + ops2);

  template<class... Ts, class... Ops>
  auto link(Source<Ts...> source, RawNodes<Ops...> laterOps)
    PIPES_RETURN(Source{source.root, source.ops + laterOps});

  template<class... Ops, class... Ts>
  auto link(RawNodes<Ops...> earlierOps, Sink<Ts...> sink)
    PIPES_RETURN(Sink{sink.finalSink, earlierOps + sink.ops});

  template<class... T1s, class... T2s>
  auto link(Source<T1s...> source, Sink<T2s...> sink)
    PIPES_RETURN(finish(source.root, source.ops + sink.ops, sink.finalSink));
} // namespace pipes

namespace pipes::detail
{
  auto collapse(auto const& s) PIPES_RETURN(connect_links(s.ops, s.finalSink));

  template<class S, class T>
  concept ValidSink = SinkFor<decltype(collapse(std::declval<S>())), T>;

  template<typename X, typename T>
  concept ValidReceiverFor = ValidChainFor<X, T> || ValidSink<X, T>;
} // namespace pipes

#include "SpecificPipes/ForEach.hpp"
#include "SpecificPipes/PushBack.hpp"

namespace pipes::detail
{
  template<class T>
  auto link(std::vector<T> const& v, ValidReceiverFor<T> auto n)
    PIPES_RETURN(link(api::forEach(v), n));

  template<class T, class... Ops>
  auto link(Source<Ops...> source, std::vector<T>& v)
    PIPES_RETURN(link(source , api::push_back(v)));

  template<class T, class... Ops>
  auto link(RawNodes<Ops...> n, std::vector<T>& v)
    PIPES_RETURN(link(n, api::push_back(v)));
}

namespace pipes::detail::api
{
  template<class Source, class Sink>
  concept CanLink = requires(Source source, Sink sink) { link(source, sink); };
}
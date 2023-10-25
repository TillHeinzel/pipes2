#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<Source S>
  auto finish_impl(S source, SinkFor<typename S::OutputType> auto sink)
    PIPES_RETURN(source.push(sink));

  template<class... Ops>
  auto finish(Source auto source, Section<Ops...> ops, auto finalSink)
    PIPES_RETURN(finish_impl(source, connect_to_sink(ops, finalSink)))
} // namespace pipes

namespace pipes::detail
{
  template<class... Ops1, class... Ops2>
  auto link(Section<Ops1...> ops1, Section<Ops2...> ops2)
    PIPES_RETURN(ops1 + ops2);

  template<class... Ts, class... Ops>
  auto link(SourceSection<Ts...> source, Section<Ops...> laterOps)
    PIPES_RETURN(SourceSection{source.source, source.ops + laterOps});

  template<class... Ops, class... Ts>
  auto link(Section<Ops...> earlierOps, SinkSection<Ts...> sink)
    PIPES_RETURN(SinkSection{sink.finalSink, earlierOps + sink.ops});

  template<class... T1s, class... T2s>
  auto link(SourceSection<T1s...> source, SinkSection<T2s...> sink)
    PIPES_RETURN(finish(source.source, source.ops + sink.ops, sink.finalSink));
} // namespace pipes

namespace pipes::detail
{
  auto collapse(auto const& s) PIPES_RETURN(connect_to_sink(s.ops, s.finalSink));

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
  auto link(SourceSection<Ops...> source, std::vector<T>& v)
    PIPES_RETURN(link(source , api::push_back(v)));

  template<class T, class... Ops>
  auto link(Section<Ops...> n, std::vector<T>& v)
    PIPES_RETURN(link(n, api::push_back(v)));
}

namespace pipes::detail::api
{
  template<class SourceSection, class SinkSection>
  concept CanLink = requires(SourceSection source, SinkSection sink) { link(source, sink); };
}
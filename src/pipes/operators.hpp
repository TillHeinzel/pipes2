#pragma once

#include "detail/Utility/HOF.hpp"

#include "detail/GenericImplementation/impl.hpp"

#include "detail/OutputIterator.hpp"
#include "detail/Switch.hpp"

namespace pipes::detail
{
  template<typename Source, typename Sink, typename Pipe>
  concept ValidPipeline = requires(Source source, Sink sink, Pipe pipe) {
    connectPipeline(Pipeline{source, sink, pipe}).run();
  };

  template<typename Source, typename Pipe>
  concept ValidSource = ValidPipeline<Source, DummySink, Pipe>;
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ops1, class... Ops2>
  decltype(auto) link(Section<Ops1...> ops1, Section<Ops2...> ops2)
  {
    return std::move(ops1) + std::move(ops2);
  }

  template<class Source_, class... Ts, class... Ops>
  decltype(auto) link(SourceSection<Source_, Ts...> source,
                      Section<Ops...> laterOps)
  {
    return std::move(source) + std::move(laterOps);
  }

  template<class... Ops, class... Ts>
  decltype(auto) link(Section<Ops...> earlierOps, SinkSection<Ts...> sink)
  {
    return std::move(earlierOps) + std::move(sink);
  }

  template<class Source, class... T1s, class Sink, class... T2s>
  decltype(auto) link(SourceSection<Source, T1s...> source,
                      SinkSection<Sink, T2s...> sink)
  {
    return connectPipeline(std::move(source) + std::move(sink)).run();
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ops>
  decltype(auto) link(DefaultSourceAble auto&& r, SinkSection<Ops...> n)
  {
    return link(defaultSource(PIPES_FWD(r)), std::move(n));
  }

  template<class... Ts>
  decltype(auto) link(DefaultSourceAble auto&& r, Section<Ts...> n)
  {
    return link(defaultSource(PIPES_FWD(r)), std::move(n));
  }

  template<class... Ops>
  decltype(auto) link(SourceSection<Ops...> source, DefaultSinkAble auto&& p)
  {
    return link(std::move(source), defaultSink(PIPES_FWD(p)));
  }

  template<class... Ops>
  decltype(auto) link(Section<Ops...> n, DefaultSinkAble auto&& p)
  {
    return link(std::move(n), defaultSink(PIPES_FWD(p)));
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class F, class... Pieces1, class... Pieces2>
  auto link(CaseSection<F, Pieces1...> c, Section<Pieces2...> s)
  {
    return CaseSection{std::move(c).f, std::move(c).pipe + std::move(s)};
  }

  template<class F, class... Pieces>
  auto link(CaseSection<F, Pieces...> c, UsableAsSink auto&& s)
  {
    return CaseSink{std::move(c).f, useAsSink(std::move(c).pipe + asSinkSection(PIPES_FWD(s)))};
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts, class... Pieces>
  auto link(OutputIteratorSection<Ts...> lhs, Section<Pieces...> rhs)
  {
    return OutputIteratorSection{std::move(lhs).pipe + std::move(rhs)};
  }

  template<class... Ts>
  auto link(OutputIteratorSection<Ts...> d, UsableAsSink auto&& s)
  {
    return OutputIterator{
      useAsSink(std::move(d).pipe + asSinkSection(PIPES_FWD(s)))};
  }
} // namespace pipes::detail

namespace pipes::detail::api
{
  template<class SourceSection, class SinkSection>
  concept CanLink =
    requires(SourceSection source, SinkSection sink) { link(source, sink); };
}

namespace pipes::detail
{
  template<class Source, class Sink>
  decltype(auto) operator>>=(Source&& source, Sink&& sink)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }

  template<class Source, class Sink>
  decltype(auto) operator>>(Source&& source, Sink&& sink)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }

  template<class Source, class Sink>
  decltype(auto) operator<<(Sink&& sink, Source&& source)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }

  template<class Source, class Sink>
  decltype(auto) operator<<=(Sink&& sink, Source&& source)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }
} // namespace pipes::detail

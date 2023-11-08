#pragma once

#include "detail/Utility/FWD.hpp"

#include "detail/GenericImplementation/impl.hpp"

#include "detail/Switch.hpp"

#include "useAsSink.hpp"

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
  auto link(Section<Ops1...> ops1, Section<Ops2...> ops2)
    PIPES_RETURN(ops1 + ops2);

  template<class Source_, class... Ts, class... Ops>
    requires(ValidSource<Source_, Section<Ts..., Ops...>>)
  auto link(SourceSection<Source_, Ts...> source, Section<Ops...> laterOps)
    PIPES_RETURN(source + laterOps);

  template<class... Ops, class... Ts>
  auto link(Section<Ops...> earlierOps, SinkSection<Ts...> sink)
    PIPES_RETURN(earlierOps + sink);

  template<class Source, class... T1s, class Sink, class... T2s>
    requires(ValidPipeline<Source, Sink, Section<T1s..., T2s...>>)
  auto link(SourceSection<Source, T1s...> source,
            SinkSection<Sink, T2s...> sink)
    PIPES_RETURN(connectPipeline(source + sink).run());
} // namespace pipes::detail

namespace pipes::detail
{
  auto link(auto&& r, auto n)
    PIPES_RETURN(link(defaultSource(PIPES_FWD(r)), n));

  template<class... Ops>
  auto link(SourceSection<Ops...> source, auto&& p)
    PIPES_RETURN(link(source, defaultSink(PIPES_FWD(p))));

  template<class... Ops>
  auto link(Section<Ops...> n, auto&& p)
    PIPES_RETURN(link(n, defaultSink(PIPES_FWD(p))));
} // namespace pipes::detail

namespace pipes::detail
{
  template<class F, class... Pieces1, class... Pieces2>
  auto link(CaseSection<F, Pieces1...> c, Section<Pieces2...> s)
  {
    return CaseSection{c.f, c.pipe + s};
  }

  template<class F, class... Pieces>
  auto link(CaseSection<F, Pieces...> c, UsableAsSink auto&& s)
  {
    return CaseSink{c.f, useAsSink(c.pipe + asSinkSection(PIPES_FWD(s)))};
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
